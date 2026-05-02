#include "SerialTab.h"
#include "PortConfigWidget.h"
#include "TerminalWidget.h"
#include "MacroPanel.h"
#include "PlotWidget.h"
#include "ProtocolPanel.h"
#include "ScriptPanel.h"
#include "LogPanel.h"
#include "../core/SerialManager.h"
#include "../core/DataLogger.h"
#include "../core/MacroManager.h"
#include "../core/AutoSender.h"
#include "../core/ScriptEngine.h"
#include "../protocol/ProtocolParser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QTabWidget>
#include <QMessageBox>
#include <QStatusBar>

SerialTab::SerialTab(QWidget *parent) : QWidget(parent)
{
    // Core objects
    m_serial   = new SerialManager(this);
    m_logger   = new DataLogger(this);
    m_macros   = new MacroManager(this);
    m_autoSend = new AutoSender(this);
    m_scripts  = new ScriptEngine(this);
    m_proto    = new ProtocolParser(this);

    m_autoSend->setSendCallback([this](const QByteArray &data) -> bool {
        return m_serial->sendData(data);
    });
    m_scripts->setSendCallback([this](const QByteArray &data) -> bool {
        return m_serial->sendData(data);
    });

    setupLayout();
    connectSignals();
}

SerialTab::~SerialTab()
{
    disconnectPort();
}

void SerialTab::setupLayout()
{
    // Main layout: top = port config bar, bottom = content
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Port config bar
    m_portCfg = new PortConfigWidget(this);
    mainLayout->addWidget(m_portCfg);

    // Horizontal splitter: terminal | side panel
    m_hSplitter = new QSplitter(Qt::Horizontal, this);
    m_hSplitter->setObjectName("hSplitter");

    // Terminal widget (left)
    m_terminal = new TerminalWidget(this);
    m_hSplitter->addWidget(m_terminal);

    // Right side tab panel
    m_sideTabs = new QTabWidget(this);
    m_sideTabs->setObjectName("sideTabs");
    m_sideTabs->setTabPosition(QTabWidget::North);

    m_macroPanel  = new MacroPanel(m_macros, m_autoSend, this);
    m_plotWidget  = new PlotWidget(this);
    m_protoPanel  = new ProtocolPanel(m_proto, this);
    m_scriptPanel = new ScriptPanel(m_scripts, this);
    m_logPanel    = new LogPanel(m_logger, this);

    m_sideTabs->addTab(m_macroPanel,  "🔧 宏命令");
    m_sideTabs->addTab(m_plotWidget,  "📈 波形图");
    m_sideTabs->addTab(m_protoPanel,  "🔬 协议解析");
    m_sideTabs->addTab(m_scriptPanel, "📜 脚本");
    m_sideTabs->addTab(m_logPanel,    "📋 日志");

    m_sideTabs->setMinimumWidth(280);
    m_sideTabs->setMaximumWidth(500);

    m_hSplitter->addWidget(m_sideTabs);
    m_hSplitter->setStretchFactor(0, 3);
    m_hSplitter->setStretchFactor(1, 1);
    m_hSplitter->setSizes({800, 320});

    mainLayout->addWidget(m_hSplitter, 1);
}

void SerialTab::connectSignals()
{
    // Serial signals
    connect(m_serial, &SerialManager::dataReceived, this, &SerialTab::onDataReceived);
    connect(m_serial, &SerialManager::errorOccurred, this, &SerialTab::onPortError);
    connect(m_serial, &SerialManager::portOpened, this, [this]() {
        emit connectionChanged(true);
        emit statusMessage(QString("已连接：%1").arg(m_serial->portName()));
        m_portCfg->setConnected(true);
    });
    connect(m_serial, &SerialManager::portClosed, this, [this]() {
        emit connectionChanged(false);
        emit statusMessage("已断开");
        m_portCfg->setConnected(false);
    });

    // Port config
    connect(m_portCfg, &PortConfigWidget::connectRequested, this, &SerialTab::onConnectClicked);
    connect(m_portCfg, &PortConfigWidget::portNameChanged, this, &SerialTab::portNameChanged);

    // Terminal send
    connect(m_terminal, &TerminalWidget::sendRequested, this, &SerialTab::onSendRequested);

    // Macro panel send
    connect(m_macroPanel, &MacroPanel::sendRequested, this, &SerialTab::onSendRequested);

    // Proto panel send
    connect(m_protoPanel, &ProtocolPanel::sendRequested, this, &SerialTab::onSendRequested);

    // Script output
    connect(m_scripts, &ScriptEngine::output, m_scriptPanel, &ScriptPanel::appendOutput);
    connect(m_scripts, &ScriptEngine::error, m_scriptPanel, &ScriptPanel::appendError);

    // Protocol parsed frames -> proto panel
    connect(m_proto, &ProtocolParser::frameParsed, m_protoPanel, &ProtocolPanel::onFrameParsed);
}

void SerialTab::onConnectClicked()
{
    if (m_serial->isOpen()) {
        disconnectPort();
        return;
    }
    SerialConfig cfg = m_portCfg->config();
    if (cfg.portName.isEmpty()) {
        emit statusMessage("未选择端口");
        return;
    }
    m_serial->open(cfg);
}

void SerialTab::onDataReceived(const QByteArray &data)
{
    m_terminal->appendRx(data);
    m_logger->logRx(data);
    m_proto->feed(data);
    m_plotWidget->feedData(data);
    m_scripts->feedData(data);
}

void SerialTab::onSendRequested(const QByteArray &data)
{
    if (!m_serial->isOpen()) {
        emit statusMessage("端口未打开");
        return;
    }
    if (m_serial->sendData(data)) {
        m_terminal->appendTx(data);
        m_logger->logTx(data);
    }
}

void SerialTab::onPortError(const QString &err)
{
    emit statusMessage("错误：" + err);
    m_terminal->appendError(err);
}

void SerialTab::disconnectPort()
{
    m_autoSend->stop();
    m_serial->close();
}

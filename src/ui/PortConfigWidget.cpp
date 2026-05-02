#include "PortConfigWidget.h"
#include "widgets/MaterialButton.h"
#include <QSerialPortInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>

PortConfigWidget::PortConfigWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("portConfigWidget");
    setupUi();
}

void PortConfigWidget::setupUi()
{
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(6);

    // Status dot
    m_statusDot = new QLabel("●", this);
    m_statusDot->setObjectName("statusDot");
    m_statusDot->setToolTip("连接状态");
    layout->addWidget(m_statusDot);

    // Port
    auto *portLabel = new QLabel("端口：", this);
    portLabel->setObjectName("cfgLabel");
    layout->addWidget(portLabel);
    m_portCombo = new QComboBox(this);
    m_portCombo->setObjectName("cfgCombo");
    m_portCombo->setMinimumWidth(90);
    layout->addWidget(m_portCombo);

    m_refreshBtn = new QPushButton("⟳", this);
    m_refreshBtn->setObjectName("refreshBtn");
    m_refreshBtn->setFixedSize(28, 28);
    m_refreshBtn->setToolTip("刷新端口列表");
    layout->addWidget(m_refreshBtn);

    // Separator
    auto *sep1 = new QFrame(this);
    sep1->setFrameShape(QFrame::VLine);
    sep1->setObjectName("cfgSep");
    layout->addWidget(sep1);

    // Baud rate
    layout->addWidget(new QLabel("波特率：", this));
    m_baudCombo = new QComboBox(this);
    m_baudCombo->setObjectName("cfgCombo");
    for (auto baud : {1200,2400,4800,9600,19200,38400,57600,115200,230400,460800,921600})
        m_baudCombo->addItem(QString::number(baud), baud);
    m_baudCombo->setCurrentText("115200");
    layout->addWidget(m_baudCombo);

    // Data bits
    layout->addWidget(new QLabel("数据位：", this));
    m_dataBits = new QComboBox(this);
    m_dataBits->setObjectName("cfgCombo");
    m_dataBits->addItem("5", QSerialPort::Data5);
    m_dataBits->addItem("6", QSerialPort::Data6);
    m_dataBits->addItem("7", QSerialPort::Data7);
    m_dataBits->addItem("8", QSerialPort::Data8);
    m_dataBits->setCurrentText("8");
    layout->addWidget(m_dataBits);

    // Stop bits
    layout->addWidget(new QLabel("停止位：", this));
    m_stopBits = new QComboBox(this);
    m_stopBits->setObjectName("cfgCombo");
    m_stopBits->addItem("1",   QSerialPort::OneStop);
    m_stopBits->addItem("1.5", QSerialPort::OneAndHalfStop);
    m_stopBits->addItem("2",   QSerialPort::TwoStop);
    layout->addWidget(m_stopBits);

    // Parity
    layout->addWidget(new QLabel("校验位：", this));
    m_parity = new QComboBox(this);
    m_parity->setObjectName("cfgCombo");
    m_parity->addItem("无",  QSerialPort::NoParity);
    m_parity->addItem("偶校验",  QSerialPort::EvenParity);
    m_parity->addItem("奇校验",   QSerialPort::OddParity);
    m_parity->addItem("标记",  QSerialPort::MarkParity);
    m_parity->addItem("空格", QSerialPort::SpaceParity);
    layout->addWidget(m_parity);

    // Flow control
    layout->addWidget(new QLabel("流控：", this));
    m_flowCtrl = new QComboBox(this);
    m_flowCtrl->setObjectName("cfgCombo");
    m_flowCtrl->addItem("无",     QSerialPort::NoFlowControl);
    m_flowCtrl->addItem("RTS/CTS",  QSerialPort::HardwareControl);
    m_flowCtrl->addItem("XON/XOFF", QSerialPort::SoftwareControl);
    layout->addWidget(m_flowCtrl);

    layout->addStretch();

    // Connect button
    m_connectBtn = new QPushButton("连接", this);
    m_connectBtn->setObjectName("connectBtn");
    m_connectBtn->setMinimumWidth(90);
    m_connectBtn->setCheckable(false);
    layout->addWidget(m_connectBtn);

    // Populate ports
    refreshPorts();

    connect(m_refreshBtn, &QPushButton::clicked, this, &PortConfigWidget::refreshPorts);
    connect(m_connectBtn, &QPushButton::clicked, this, &PortConfigWidget::connectRequested);
    connect(m_portCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PortConfigWidget::onPortChanged);
}

void PortConfigWidget::refreshPorts()
{
    m_portCombo->blockSignals(true);
    QString prev = m_portCombo->currentText();
    m_portCombo->clear();
    for (const auto &info : QSerialPortInfo::availablePorts()) {
        m_portCombo->addItem(info.portName() + " - " + info.description(), info.portName());
    }
    // Restore previous selection
    int idx = m_portCombo->findData(prev);
    if (idx >= 0) m_portCombo->setCurrentIndex(idx);
    m_portCombo->blockSignals(false);
}

void PortConfigWidget::onPortChanged(int index)
{
    emit portNameChanged(m_portCombo->itemData(index).toString());
}

SerialConfig PortConfigWidget::config() const
{
    SerialConfig cfg;
    cfg.portName = m_portCombo->currentData().toString();
    cfg.baudRate = m_baudCombo->currentData().toInt();
    cfg.dataBits = static_cast<QSerialPort::DataBits>(m_dataBits->currentData().toInt());
    cfg.stopBits = static_cast<QSerialPort::StopBits>(m_stopBits->currentData().toInt());
    cfg.parity   = static_cast<QSerialPort::Parity>(m_parity->currentData().toInt());
    cfg.flowCtrl = static_cast<QSerialPort::FlowControl>(m_flowCtrl->currentData().toInt());
    return cfg;
}

void PortConfigWidget::setConnected(bool connected)
{
    m_connected = connected;
    m_statusDot->setProperty("connected", connected);
    m_statusDot->style()->unpolish(m_statusDot);
    m_statusDot->style()->polish(m_statusDot);
    m_connectBtn->setText(connected ? "断开" : "连接");
    m_portCombo->setEnabled(!connected);
    m_baudCombo->setEnabled(!connected);
    m_dataBits->setEnabled(!connected);
    m_stopBits->setEnabled(!connected);
    m_parity->setEnabled(!connected);
    m_flowCtrl->setEnabled(!connected);
    m_refreshBtn->setEnabled(!connected);
}

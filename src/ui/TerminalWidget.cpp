#include "TerminalWidget.h"
#include <QScrollBar>
#include <QDateTime>
#include <QTextCharFormat>
#include <QKeyEvent>
#include <QShortcut>
#include <QFrame>

TerminalWidget::TerminalWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("terminalWidget");
    setupUi();
}

void TerminalWidget::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Toolbar row
    auto *toolbar = new QWidget(this);
    toolbar->setObjectName("termToolbar");
    auto *tbLayout = new QHBoxLayout(toolbar);
    tbLayout->setContentsMargins(8, 4, 8, 4);
    tbLayout->setSpacing(8);

    tbLayout->addWidget(new QLabel("模式：", toolbar));
    m_modeCombo = new QComboBox(toolbar);
    m_modeCombo->setObjectName("cfgCombo");
    m_modeCombo->addItem("文本", TextMode);
    m_modeCombo->addItem("HEX",  HexMode);
    tbLayout->addWidget(m_modeCombo);

    m_showHex = new QCheckBox("显示 HEX", toolbar);
    tbLayout->addWidget(m_showHex);

    m_timestamp = new QCheckBox("时间戳", toolbar);
    m_timestamp->setChecked(true);
    tbLayout->addWidget(m_timestamp);

    m_autoScroll = new QCheckBox("自动滚动", toolbar);
    m_autoScroll->setChecked(true);
    tbLayout->addWidget(m_autoScroll);

    tbLayout->addStretch();

    m_rxLabel = new QLabel("RX：0", toolbar);
    m_rxLabel->setObjectName("rxLabel");
    tbLayout->addWidget(m_rxLabel);

    m_txLabel = new QLabel("TX：0", toolbar);
    m_txLabel->setObjectName("txLabel");
    tbLayout->addWidget(m_txLabel);

    m_clearBtn = new QPushButton("清除", toolbar);
    m_clearBtn->setObjectName("clearBtn");
    m_clearBtn->setFixedHeight(28);
    tbLayout->addWidget(m_clearBtn);

    layout->addWidget(toolbar);

    // Separator
    auto *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setObjectName("separator");
    layout->addWidget(sep);

    // Display area
    m_display = new QPlainTextEdit(this);
    m_display->setObjectName("termDisplay");
    m_display->setReadOnly(true);
    m_display->setUndoRedoEnabled(false);
    m_display->setMaximumBlockCount(10000);
    QFont font("Consolas", 10);
    font.setFixedPitch(true);
    m_display->setFont(font);
    layout->addWidget(m_display, 1);

    // Sep
    auto *sep2 = new QFrame(this);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setObjectName("separator");
    layout->addWidget(sep2);

    // Input row
    auto *inputRow = new QWidget(this);
    inputRow->setObjectName("inputRow");
    auto *inputLayout = new QHBoxLayout(inputRow);
    inputLayout->setContentsMargins(8, 6, 8, 6);
    inputLayout->setSpacing(6);

    m_inputLine = new QLineEdit(inputRow);
    m_inputLine->setObjectName("termInput");
    m_inputLine->setPlaceholderText("输入要发送的消息...");
    inputLayout->addWidget(m_inputLine, 1);

    m_hexInput = new HexEditor(inputRow);
    m_hexInput->setObjectName("hexInput");
    m_hexInput->setPlaceholderText("HEX 字节，例如：01 03 00 00 00 01");
    m_hexInput->hide();
    inputLayout->addWidget(m_hexInput, 1);

    m_eolCombo = new QComboBox(inputRow);
    m_eolCombo->setObjectName("cfgCombo");
    m_eolCombo->addItem("\\r\\n", "\r\n");
    m_eolCombo->addItem("\\n",    "\n");
    m_eolCombo->addItem("\\r",    "\r");
    m_eolCombo->addItem("无",   "");
    inputLayout->addWidget(m_eolCombo);

    m_sendBtn = new QPushButton("发送", inputRow);
    m_sendBtn->setObjectName("sendBtn");
    m_sendBtn->setMinimumWidth(70);
    inputLayout->addWidget(m_sendBtn);

    layout->addWidget(inputRow);

    // Connections
    connect(m_sendBtn,   &QPushButton::clicked,  this, &TerminalWidget::onSendClicked);
    connect(m_clearBtn,  &QPushButton::clicked,  this, &TerminalWidget::onClearClicked);
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TerminalWidget::onModeChanged);
    connect(m_showHex, &QCheckBox::toggled, this, [this](bool on) {
        m_showHexDisplay = on;
    });
    connect(m_timestamp, &QCheckBox::toggled, this, [this](bool on) {
        m_showTimestamp = on;
    });

    // Enter to send
    connect(m_inputLine, &QLineEdit::returnPressed, this, &TerminalWidget::onSendClicked);
}

void TerminalWidget::appendRx(const QByteArray &data)
{
    m_rxCount += data.size();
    m_rxLabel->setText(QString("RX：%1").arg(m_rxCount));

    QString ts;
    if (m_showTimestamp)
        ts = QString("<span style='color:#888'>[%1]</span> ")
             .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"));

    QString content;
    if (m_showHexDisplay)
        content = QString("<span style='color:#4FC3F7'>%1</span>").arg(formatHex(data));
    else {
        QString text = QString::fromUtf8(data);
        text = text.toHtmlEscaped();
        content = QString("<span style='color:#A5D6A7'>%1</span>").arg(text);
    }

    m_display->appendHtml(ts + content);
    scrollToBottom();
}

void TerminalWidget::appendTx(const QByteArray &data)
{
    m_txCount += data.size();
    m_txLabel->setText(QString("TX：%1").arg(m_txCount));

    QString ts;
    if (m_showTimestamp)
        ts = QString("<span style='color:#888'>[%1]</span> ")
             .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"));

    QString content;
    if (m_showHexDisplay)
        content = QString("<span style='color:#FFB74D'>%1</span>").arg(formatHex(data));
    else {
        QString text = QString::fromUtf8(data).toHtmlEscaped();
        content = QString("<span style='color:#FFB74D'>→ %1</span>").arg(text);
    }

    m_display->appendHtml(ts + content);
    scrollToBottom();
}

void TerminalWidget::appendError(const QString &text)
{
    m_display->appendHtml(
        QString("<span style='color:#EF5350'>⚠ %1</span>").arg(text.toHtmlEscaped()));
    scrollToBottom();
}

void TerminalWidget::clearDisplay()
{
    m_display->clear();
    m_rxCount = m_txCount = 0;
    m_rxLabel->setText("RX：0");
    m_txLabel->setText("TX：0");
}

void TerminalWidget::onSendClicked()
{
    QByteArray data;
    if (m_mode == TextMode) {
        QString text = m_inputLine->text();
        if (text.isEmpty()) return;
        data = text.toUtf8();
        QString eol = m_eolCombo->currentData().toString();
        data.append(eol.toUtf8());
    } else {
        data = m_hexInput->hexData();
        if (data.isEmpty()) return;
    }
    emit sendRequested(data);
}

void TerminalWidget::onModeChanged(int index)
{
    m_mode = static_cast<Mode>(m_modeCombo->itemData(index).toInt());
    bool isHex = (m_mode == HexMode);
    m_inputLine->setVisible(!isHex);
    m_hexInput->setVisible(isHex);
    m_eolCombo->setEnabled(!isHex);
}

void TerminalWidget::onClearClicked()
{
    clearDisplay();
}

void TerminalWidget::onSendLineEnterPressed()
{
    onSendClicked();
}

QString TerminalWidget::formatHex(const QByteArray &data) const
{
    QString result;
    for (int i = 0; i < data.size(); i++) {
        result += QString("%1 ").arg((quint8)data[i], 2, 16, QLatin1Char('0')).toUpper();
        if ((i+1) % 16 == 0) result += "<br>";
    }
    return result.trimmed();
}

void TerminalWidget::scrollToBottom()
{
    if (m_autoScroll && m_autoScroll->isChecked()) {
        QScrollBar *sb = m_display->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}

#include "ProtocolPanel.h"
#include "../protocol/ModbusParser.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QDateTime>

ProtocolPanel::ProtocolPanel(ProtocolParser *parser, QWidget *parent)
    : QWidget(parent), m_parser(parser)
{
    setupUi();
}

void ProtocolPanel::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    // Protocol selector
    auto *row = new QHBoxLayout();
    row->addWidget(new QLabel("协议：", this));
    m_protoCombo = new QComboBox(this);
    m_protoCombo->addItem("自动",        ProtocolParser::Auto);
    m_protoCombo->addItem("Modbus RTU",  ProtocolParser::Modbus_RTU);
    m_protoCombo->addItem("Modbus ASCII",ProtocolParser::Modbus_ASCII);
    m_protoCombo->addItem("自定义",      ProtocolParser::Custom);
    row->addWidget(m_protoCombo);
    row->addStretch();
    m_statsLabel = new QLabel("帧数：0", this);
    row->addWidget(m_statsLabel);
    m_clearBtn = new QPushButton("清除", this);
    m_clearBtn->setObjectName("clearBtn");
    row->addWidget(m_clearBtn);
    layout->addLayout(row);

    // Frame table
    m_frameTable = new QTableWidget(0, 5, this);
    m_frameTable->setObjectName("frameTable");
    m_frameTable->setHorizontalHeaderLabels({"时间", "协议", "功能码", "描述", "原始数据 (HEX)"});
    m_frameTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_frameTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_frameTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_frameTable->setAlternatingRowColors(true);
    m_frameTable->verticalHeader()->hide();
    layout->addWidget(m_frameTable, 1);

    setupModbusBuilder();
    layout->addWidget(m_modbusGroup);

    connect(m_protoCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ProtocolPanel::onProtocolChanged);
    connect(m_clearBtn, &QPushButton::clicked, this, &ProtocolPanel::onClearTable);
}

void ProtocolPanel::setupModbusBuilder()
{
    m_modbusGroup = new QGroupBox("Modbus 帧构建器", this);
    auto *form = new QFormLayout(m_modbusGroup);

    m_mbAddr = new QSpinBox(this); m_mbAddr->setRange(0, 247); m_mbAddr->setValue(1);
    form->addRow("从站地址：", m_mbAddr);

    m_mbFunc = new QComboBox(this);
    m_mbFunc->addItem("FC03 Read Holding Regs", 3);
    m_mbFunc->addItem("FC04 Read Input Regs",   4);
    m_mbFunc->addItem("FC06 Write Single Reg",  6);
    m_mbFunc->addItem("FC10 Write Multi Regs",  16);
    form->addRow("功能：", m_mbFunc);

    m_mbReg = new QSpinBox(this); m_mbReg->setRange(0, 65535);
    form->addRow("寄存器：", m_mbReg);

    m_mbCount = new QSpinBox(this); m_mbCount->setRange(1, 125); m_mbCount->setValue(1);
    form->addRow("数量/数值：", m_mbCount);

    m_mbValue = new QLineEdit(this);
    m_mbValue->setPlaceholderText("FC10：逗号分隔的数值，例如：100,200,300");
    form->addRow("Values:", m_mbValue);

    m_mbPreview = new QLineEdit(this);
    m_mbPreview->setReadOnly(true);
    m_mbPreview->setObjectName("hexPreview");
    form->addRow("预览：", m_mbPreview);

    auto *btnRow = new QHBoxLayout();
    m_mbBuildBtn = new QPushButton("构建并发送", this);
    m_mbBuildBtn->setObjectName("sendBtn");
    auto *previewBtn = new QPushButton("预览", this);
    previewBtn->setObjectName("macroBtn");
    btnRow->addWidget(previewBtn);
    btnRow->addWidget(m_mbBuildBtn);
    form->addRow(btnRow);

    connect(previewBtn, &QPushButton::clicked, this, [this]() {
        QByteArray frame = buildModbusFrame();
        m_mbPreview->setText(frame.toHex(' ').toUpper());
    });
    connect(m_mbBuildBtn, &QPushButton::clicked, this, &ProtocolPanel::onBuildFrame);
}

QByteArray ProtocolPanel::buildModbusFrame()
{
    int func  = m_mbFunc->currentData().toInt();
    int addr  = m_mbAddr->value();
    int reg   = m_mbReg->value();
    int count = m_mbCount->value();
    QString valStr = m_mbValue->text();

    switch (func) {
    case 3:  return ModbusParser::buildReadHolding(addr, reg, count);
    case 4:  return ModbusParser::buildReadInput(addr, reg, count);
    case 6:  return ModbusParser::buildWriteSingle(addr, reg, count);
    case 16: {
        QList<quint16> vals;
        for (const auto &v : valStr.split(',', Qt::SkipEmptyParts)) {
            bool ok; quint16 n = v.trimmed().toUShort(&ok);
            if (ok) vals << n;
        }
        if (vals.isEmpty()) vals << (quint16)count;
        return ModbusParser::buildWriteMultiple(addr, reg, vals);
    }
    default: return {};
    }
}

void ProtocolPanel::onBuildFrame()
{
    QByteArray frame = buildModbusFrame();
    if (!frame.isEmpty()) {
        m_mbPreview->setText(frame.toHex(' ').toUpper());
        emit sendRequested(frame);
    }
}

void ProtocolPanel::onClearTable()
{
    m_frameTable->setRowCount(0);
    m_frameCount = 0;
    m_statsLabel->setText("帧数：0");
}

void ProtocolPanel::onProtocolChanged(int index)
{
    ProtocolParser::Protocol p = static_cast<ProtocolParser::Protocol>(
        m_protoCombo->itemData(index).toInt());
    m_parser->setProtocol(p);
    m_modbusGroup->setVisible(p == ProtocolParser::Modbus_RTU || p == ProtocolParser::Auto);
}

void ProtocolPanel::onFrameParsed(const ParsedFrame &frame)
{
    m_frameCount++;
    m_statsLabel->setText(QString("帧数：%1").arg(m_frameCount));

    int row = m_frameTable->rowCount();
    m_frameTable->insertRow(row);
    m_frameTable->setItem(row, 0, new QTableWidgetItem(
        QDateTime::currentDateTime().toString("HH:mm:ss.zzz")));
    m_frameTable->setItem(row, 1, new QTableWidgetItem(frame.protocol));
    m_frameTable->setItem(row, 2, new QTableWidgetItem(
        QString("0x%1").arg(frame.functionCode, 2, 16, QLatin1Char('0')).toUpper()));
    m_frameTable->setItem(row, 3, new QTableWidgetItem(frame.description));
    m_frameTable->setItem(row, 4, new QTableWidgetItem(
        frame.raw.toHex(' ').toUpper()));

    m_frameTable->scrollToBottom();

    // Limit rows
    if (m_frameTable->rowCount() > 500)
        m_frameTable->removeRow(0);
}

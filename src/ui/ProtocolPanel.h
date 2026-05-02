#pragma once
#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include "../protocol/ProtocolParser.h"

class ProtocolPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ProtocolPanel(ProtocolParser *parser, QWidget *parent = nullptr);

    void onFrameParsed(const ParsedFrame &frame);

signals:
    void sendRequested(const QByteArray &data);

private slots:
    void onProtocolChanged(int index);
    void onBuildFrame();
    void onClearTable();

private:
    void setupUi();
    void setupModbusBuilder();
    QByteArray buildModbusFrame();

    ProtocolParser *m_parser = nullptr;

    QComboBox    *m_protoCombo   = nullptr;
    QTableWidget *m_frameTable   = nullptr;
    QPushButton  *m_clearBtn     = nullptr;
    QLabel       *m_statsLabel   = nullptr;

    // Modbus builder
    QGroupBox    *m_modbusGroup  = nullptr;
    QSpinBox     *m_mbAddr       = nullptr;
    QComboBox    *m_mbFunc       = nullptr;
    QSpinBox     *m_mbReg        = nullptr;
    QSpinBox     *m_mbCount      = nullptr;
    QLineEdit    *m_mbValue      = nullptr;
    QPushButton  *m_mbBuildBtn   = nullptr;
    QLineEdit    *m_mbPreview     = nullptr;

    int m_frameCount = 0;
};

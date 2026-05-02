#pragma once
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QByteArray>
#include <QString>

struct SerialConfig {
    QString portName;
    qint32  baudRate   = 115200;
    QSerialPort::DataBits    dataBits = QSerialPort::Data8;
    QSerialPort::StopBits    stopBits = QSerialPort::OneStop;
    QSerialPort::Parity      parity   = QSerialPort::NoParity;
    QSerialPort::FlowControl flowCtrl = QSerialPort::NoFlowControl;
    int     readTimeout = 100; // ms
};

class SerialManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialManager(QObject *parent = nullptr);
    ~SerialManager();

    bool open(const SerialConfig &cfg);
    void close();
    bool isOpen() const;

    bool sendData(const QByteArray &data);
    bool sendText(const QString &text, bool addNewline = true);
    bool sendHex(const QString &hexStr);

    SerialConfig config() const { return m_config; }
    QString portName() const { return m_config.portName; }

    static QStringList availablePorts();
    static QList<QSerialPortInfo> portInfoList();

signals:
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &error);
    void portOpened();
    void portClosed();
    void bytesWritten(qint64 bytes);

private slots:
    void onReadyRead();
    void onError(QSerialPort::SerialPortError error);

private:
    QSerialPort  *m_port;
    SerialConfig  m_config;
    qint64        m_rxBytes = 0;
    qint64        m_txBytes = 0;
};

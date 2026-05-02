#include "SerialManager.h"
#include <QDebug>

SerialManager::SerialManager(QObject *parent)
    : QObject(parent)
    , m_port(new QSerialPort(this))
{
    connect(m_port, &QSerialPort::readyRead,  this, &SerialManager::onReadyRead);
    connect(m_port, &QSerialPort::errorOccurred, this, &SerialManager::onError);
    connect(m_port, &QSerialPort::bytesWritten, this, &SerialManager::bytesWritten);
}

SerialManager::~SerialManager()
{
    close();
}

bool SerialManager::open(const SerialConfig &cfg)
{
    if (m_port->isOpen())
        m_port->close();

    m_config = cfg;
    m_port->setPortName(cfg.portName);
    m_port->setBaudRate(cfg.baudRate);
    m_port->setDataBits(cfg.dataBits);
    m_port->setStopBits(cfg.stopBits);
    m_port->setParity(cfg.parity);
    m_port->setFlowControl(cfg.flowCtrl);

    if (!m_port->open(QIODevice::ReadWrite)) {
        emit errorOccurred(tr("Failed to open %1: %2")
                           .arg(cfg.portName, m_port->errorString()));
        return false;
    }
    m_rxBytes = m_txBytes = 0;
    emit portOpened();
    return true;
}

void SerialManager::close()
{
    if (m_port->isOpen()) {
        m_port->close();
        emit portClosed();
    }
}

bool SerialManager::isOpen() const
{
    return m_port && m_port->isOpen();
}

bool SerialManager::sendData(const QByteArray &data)
{
    if (!isOpen()) return false;
    qint64 written = m_port->write(data);
    if (written < 0) {
        emit errorOccurred(m_port->errorString());
        return false;
    }
    m_txBytes += written;
    return true;
}

bool SerialManager::sendText(const QString &text, bool addNewline)
{
    QString s = text;
    if (addNewline) s += "\r\n";
    return sendData(s.toUtf8());
}

bool SerialManager::sendHex(const QString &hexStr)
{
    // Remove spaces and convert
    QString clean = hexStr.simplified().remove(' ');
    if (clean.length() % 2 != 0) {
        emit errorOccurred(tr("Invalid hex string length"));
        return false;
    }
    QByteArray data = QByteArray::fromHex(clean.toLatin1());
    return sendData(data);
}

QStringList SerialManager::availablePorts()
{
    QStringList ports;
    for (const auto &info : QSerialPortInfo::availablePorts())
        ports << info.portName();
    return ports;
}

QList<QSerialPortInfo> SerialManager::portInfoList()
{
    return QSerialPortInfo::availablePorts();
}

void SerialManager::onReadyRead()
{
    QByteArray data = m_port->readAll();
    if (!data.isEmpty()) {
        m_rxBytes += data.size();
        emit dataReceived(data);
    }
}

void SerialManager::onError(QSerialPort::SerialPortError error)
{
    if (error != QSerialPort::NoError) {
        emit errorOccurred(m_port->errorString());
        if (error == QSerialPort::ResourceError) {
            close();
        }
    }
}

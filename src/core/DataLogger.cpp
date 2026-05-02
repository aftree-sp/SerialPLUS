#include "DataLogger.h"
#include <QDir>

DataLogger::DataLogger(QObject *parent) : QObject(parent) {}

DataLogger::~DataLogger()
{
    stopLogging();
}

bool DataLogger::startLogging(const QString &filePath)
{
    stopLogging();
    m_file.setFileName(filePath);
    if (!m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return false;
    m_stream.setDevice(&m_file);
    m_stream << QString("=== Log started at %1 ===\n")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    m_stream.flush();
    emit loggingStarted(filePath);
    return true;
}

void DataLogger::stopLogging()
{
    if (m_file.isOpen()) {
        m_stream << QString("=== Log stopped at %1 ===\n")
                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
        m_stream.flush();
        m_file.close();
        emit loggingStopped();
    }
}

void DataLogger::logRx(const QByteArray &data)
{
    writeEntry("RX", data);
}

void DataLogger::logTx(const QByteArray &data)
{
    writeEntry("TX", data);
}

void DataLogger::writeEntry(const QString &dir, const QByteArray &data)
{
    if (!m_file.isOpen()) return;

    QString ts;
    if (m_showTimestamp)
        ts = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");

    QString content;
    if (m_hexMode) {
        content = data.toHex(' ').toUpper();
    } else {
        content = QString::fromUtf8(data);
        content.replace('\r', "");
    }

    m_stream << ts << "[" << dir << "] " << content << "\n";
    m_stream.flush();
}

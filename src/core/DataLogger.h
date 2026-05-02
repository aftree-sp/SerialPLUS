#pragma once
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDateTime>

class DataLogger : public QObject
{
    Q_OBJECT
public:
    explicit DataLogger(QObject *parent = nullptr);
    ~DataLogger();

    bool startLogging(const QString &filePath);
    void stopLogging();
    bool isLogging() const { return m_file.isOpen(); }
    QString filePath() const { return m_file.fileName(); }

    void setTimestamp(bool enabled) { m_showTimestamp = enabled; }
    void setHexMode(bool hex) { m_hexMode = hex; }
    void setTabName(const QString &name) { m_tabName = name; }

public slots:
    void logRx(const QByteArray &data);
    void logTx(const QByteArray &data);

signals:
    void loggingStarted(const QString &path);
    void loggingStopped();

private:
    void writeEntry(const QString &dir, const QByteArray &data);

    QFile        m_file;
    QTextStream  m_stream;
    bool         m_showTimestamp = true;
    bool         m_hexMode = false;
    QString      m_tabName;
};

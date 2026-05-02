#pragma once
#include <QObject>
#include <QByteArray>
#include <QList>
#include <QString>

struct ParsedFrame {
    QString  protocol;
    quint8   functionCode = 0;
    quint16  startAddr    = 0;
    quint16  quantity     = 0;
    QList<quint16> values;
    bool     valid = false;
    QString  description;
    QByteArray raw;
};

class ProtocolParser : public QObject
{
    Q_OBJECT
public:
    explicit ProtocolParser(QObject *parent = nullptr);

    enum Protocol { Modbus_RTU, Modbus_ASCII, Custom, Auto };
    void setProtocol(Protocol p) { m_protocol = p; }
    Protocol protocol() const { return m_protocol; }

    void feed(const QByteArray &data);
    void reset();

signals:
    void frameParsed(const ParsedFrame &frame);
    void parseError(const QString &msg);

protected:
    Protocol m_protocol = Auto;
    QByteArray m_buffer;
};

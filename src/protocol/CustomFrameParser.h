#pragma once
#include "ProtocolParser.h"
#include <QString>
#include <QList>

struct CustomFrameConfig {
    QByteArray header;     // SOF bytes
    QByteArray footer;     // EOF bytes
    int        lenOffset   = -1; // byte offset of length field (-1 = disabled)
    int        lenSize     = 1;  // length field size in bytes
    bool       lengthIncludesHeader = false;
    bool       hasCrc8     = false;
    bool       hasCrc16    = false;
    int        crcOffset   = -1;
    int        maxFrameLen = 256;
};

class CustomFrameParser : public QObject
{
    Q_OBJECT
public:
    explicit CustomFrameParser(QObject *parent = nullptr);

    void setConfig(const CustomFrameConfig &cfg);
    void feed(const QByteArray &data);
    void reset();

signals:
    void frameParsed(const QByteArray &frame);
    void parseError(const QString &msg);

private:
    CustomFrameConfig m_cfg;
    QByteArray m_buf;

    bool tryParseFrame();
    quint8  calcCrc8(const QByteArray &data);
    quint16 calcCrc16(const QByteArray &data);
};

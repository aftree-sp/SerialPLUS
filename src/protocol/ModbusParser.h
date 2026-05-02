#pragma once
#include "ProtocolParser.h"
#include <QByteArray>

class ModbusParser
{
public:
    static ParsedFrame parseRTU(const QByteArray &buf);
    static quint16 crc16(const QByteArray &data);

    // Build common Modbus RTU request frames
    static QByteArray buildReadHolding(quint8 addr, quint16 reg, quint16 count);
    static QByteArray buildReadInput(quint8 addr, quint16 reg, quint16 count);
    static QByteArray buildWriteSingle(quint8 addr, quint16 reg, quint16 value);
    static QByteArray buildWriteMultiple(quint8 addr, quint16 reg, const QList<quint16> &values);
};

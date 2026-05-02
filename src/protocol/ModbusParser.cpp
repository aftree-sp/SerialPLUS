#include "ModbusParser.h"
#include <QtEndian>

static const quint16 crcTable[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0x7400, 0xB4C1, 0xB581, 0x7540, 0xB701, 0x77C0, 0x7680, 0xB641,
    0xB201, 0x72C0, 0x7380, 0xB341, 0x7100, 0xB1C1, 0xB081, 0x7040,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

quint16 ModbusParser::crc16(const QByteArray &data)
{
    quint16 crc = 0xFFFF;
    for (quint8 b : data) {
        quint8 idx = (quint8)(crc ^ b);
        crc = (crc >> 8) ^ crcTable[idx];
    }
    return crc;
}

// Determine expected frame length based on function code
static int expectedFrameLen(const QByteArray &buf)
{
    if (buf.size() < 2) return -1;
    quint8 fc = (quint8)buf[1];
    int sz = buf.size();

    switch (fc) {
    case 0x01: case 0x02: case 0x03: case 0x04:
        // Request: addr(1)+fc(1)+startReg(2)+qty(2)+crc(2) = 8
        // Response: addr(1)+fc(1)+byteCount(1)+data(N)+crc(2) >= 5
        if (sz >= 3) {
            int byteCount = (quint8)buf[2];
            return 3 + byteCount + 2; // response
        }
        return 8; // request
    case 0x05: case 0x06:
        return 8;
    case 0x0F: case 0x10:
        if (sz >= 7) {
            int byteCount = (quint8)buf[6];
            return 7 + byteCount + 2;
        }
        return -1;
    default:
        return -1;
    }
}

ParsedFrame ModbusParser::parseRTU(const QByteArray &buf)
{
    ParsedFrame frame;
    frame.raw = buf;

    if (buf.size() < 4) return frame;

    int len = expectedFrameLen(buf);
    if (len < 0 || buf.size() < len) return frame;

    QByteArray frameData = buf.left(len);
    quint16 expectedCrc = crc16(frameData.left(len - 2));
    quint16 actualCrc = (quint8)frameData[len-2] | ((quint8)frameData[len-1] << 8);

    if (expectedCrc != actualCrc) return frame;

    frame.raw          = frameData;
    frame.protocol     = "Modbus RTU";
    frame.functionCode = (quint8)frameData[1];
    frame.valid        = true;

    // Parse values
    switch (frame.functionCode) {
    case 0x03: case 0x04:
        if (frameData.size() >= 3) {
            int byteCount = (quint8)frameData[2];
            for (int i = 0; i + 1 < byteCount; i += 2) {
                quint16 val = ((quint8)frameData[3+i] << 8) | (quint8)frameData[3+i+1];
                frame.values.append(val);
            }
            frame.description = QString("FC%1 Response: %2 registers")
                                .arg(frame.functionCode).arg(frame.values.size());
        }
        break;
    case 0x05:
        if (frameData.size() >= 6) {
            frame.startAddr = ((quint8)frameData[2] << 8) | (quint8)frameData[3];
            frame.values.append(((quint8)frameData[4] << 8) | (quint8)frameData[5]);
            frame.description = QString("FC05 Write Coil @%1 = %2").arg(frame.startAddr).arg(frame.values[0]);
        }
        break;
    case 0x06:
        if (frameData.size() >= 6) {
            frame.startAddr = ((quint8)frameData[2] << 8) | (quint8)frameData[3];
            frame.values.append(((quint8)frameData[4] << 8) | (quint8)frameData[5]);
            frame.description = QString("FC06 Write Register @%1 = %2").arg(frame.startAddr).arg(frame.values[0]);
        }
        break;
    default:
        frame.description = QString("FC%1").arg(frame.functionCode, 2, 16, QLatin1Char('0')).toUpper();
    }

    return frame;
}

static QByteArray appendCrc(QByteArray data)
{
    quint16 crc = ModbusParser::crc16(data);
    data.append((char)(crc & 0xFF));
    data.append((char)(crc >> 8));
    return data;
}

QByteArray ModbusParser::buildReadHolding(quint8 addr, quint16 reg, quint16 count)
{
    QByteArray d;
    d.append(addr); d.append(0x03);
    d.append((char)(reg >> 8)); d.append((char)(reg & 0xFF));
    d.append((char)(count >> 8)); d.append((char)(count & 0xFF));
    return appendCrc(d);
}

QByteArray ModbusParser::buildReadInput(quint8 addr, quint16 reg, quint16 count)
{
    QByteArray d;
    d.append(addr); d.append(0x04);
    d.append((char)(reg >> 8)); d.append((char)(reg & 0xFF));
    d.append((char)(count >> 8)); d.append((char)(count & 0xFF));
    return appendCrc(d);
}

QByteArray ModbusParser::buildWriteSingle(quint8 addr, quint16 reg, quint16 value)
{
    QByteArray d;
    d.append(addr); d.append(0x06);
    d.append((char)(reg >> 8)); d.append((char)(reg & 0xFF));
    d.append((char)(value >> 8)); d.append((char)(value & 0xFF));
    return appendCrc(d);
}

QByteArray ModbusParser::buildWriteMultiple(quint8 addr, quint16 reg, const QList<quint16> &values)
{
    QByteArray d;
    d.append(addr); d.append(0x10);
    d.append((char)(reg >> 8)); d.append((char)(reg & 0xFF));
    quint16 count = values.size();
    d.append((char)(count >> 8)); d.append((char)(count & 0xFF));
    d.append((char)(count * 2));
    for (auto v : values) {
        d.append((char)(v >> 8)); d.append((char)(v & 0xFF));
    }
    return appendCrc(d);
}

#include "CustomFrameParser.h"

CustomFrameParser::CustomFrameParser(QObject *parent) : QObject(parent) {}

void CustomFrameParser::setConfig(const CustomFrameConfig &cfg) { m_cfg = cfg; }

void CustomFrameParser::feed(const QByteArray &data)
{
    m_buf.append(data);
    while (tryParseFrame()) {}
    // Discard if buffer too large
    if (m_buf.size() > m_cfg.maxFrameLen * 4)
        m_buf.remove(0, m_buf.size() - m_cfg.maxFrameLen * 2);
}

void CustomFrameParser::reset() { m_buf.clear(); }

bool CustomFrameParser::tryParseFrame()
{
    if (m_cfg.header.isEmpty()) return false;

    // Find header
    int start = m_buf.indexOf(m_cfg.header);
    if (start < 0) { m_buf.clear(); return false; }
    if (start > 0) m_buf.remove(0, start);

    if (m_buf.size() < m_cfg.header.size()) return false;

    // Find footer
    if (!m_cfg.footer.isEmpty()) {
        int end = m_buf.indexOf(m_cfg.footer, m_cfg.header.size());
        if (end < 0) return false;
        int frameLen = end + m_cfg.footer.size();
        if (m_buf.size() < frameLen) return false;

        QByteArray frame = m_buf.left(frameLen);
        m_buf.remove(0, frameLen);
        emit frameParsed(frame);
        return true;
    }

    // Use length field
    if (m_cfg.lenOffset >= 0) {
        int hdrSize = m_cfg.header.size();
        if (m_buf.size() < m_cfg.lenOffset + m_cfg.lenSize + hdrSize) return false;

        int length = 0;
        for (int i = 0; i < m_cfg.lenSize; i++) {
            length = (length << 8) | (quint8)m_buf[m_cfg.lenOffset + i];
        }
        if (m_cfg.lengthIncludesHeader) length -= hdrSize;
        int totalLen = hdrSize + m_cfg.lenOffset + m_cfg.lenSize + length;
        if (m_cfg.hasCrc8 || m_cfg.hasCrc16) totalLen += (m_cfg.hasCrc16 ? 2 : 1);

        if (m_buf.size() < totalLen || totalLen > m_cfg.maxFrameLen) {
            if (totalLen > m_cfg.maxFrameLen) {
                m_buf.remove(0, 1);
                emit parseError("Frame too large, skipping byte");
            }
            return false;
        }
        QByteArray frame = m_buf.left(totalLen);
        m_buf.remove(0, totalLen);
        emit frameParsed(frame);
        return true;
    }

    return false;
}

quint8 CustomFrameParser::calcCrc8(const QByteArray &data)
{
    quint8 crc = 0;
    for (quint8 b : data) crc ^= b;
    return crc;
}

quint16 CustomFrameParser::calcCrc16(const QByteArray &data)
{
    quint16 crc = 0xFFFF;
    for (quint8 b : data) {
        crc ^= (quint16)b << 8;
        for (int i = 0; i < 8; i++)
            crc = (crc & 0x8000) ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}

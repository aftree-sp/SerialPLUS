#include "ProtocolParser.h"
#include "ModbusParser.h"

ProtocolParser::ProtocolParser(QObject *parent) : QObject(parent) {}

void ProtocolParser::feed(const QByteArray &data)
{
    m_buffer.append(data);

    // Try modbus RTU framing: min 4 bytes (addr+func+crc16)
    if (m_protocol == Auto || m_protocol == Modbus_RTU) {
        while (m_buffer.size() >= 4) {
            // Attempt to find a valid Modbus RTU frame
            ParsedFrame frame = ModbusParser::parseRTU(m_buffer);
            if (frame.valid) {
                m_buffer.remove(0, frame.raw.size());
                emit frameParsed(frame);
            } else {
                // Skip one byte and try again
                m_buffer.remove(0, 1);
            }
        }
    }
}

void ProtocolParser::reset()
{
    m_buffer.clear();
}

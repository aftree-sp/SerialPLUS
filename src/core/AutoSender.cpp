#include "AutoSender.h"

AutoSender::AutoSender(QObject *parent) : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &AutoSender::onTimer);
}

void AutoSender::setData(const QByteArray &data) { m_data = data; }
void AutoSender::setInterval(int ms) { m_interval = qMax(10, ms); }
void AutoSender::setRepeatCount(int count) { m_repeatCount = count; }
void AutoSender::setSendCallback(std::function<bool(const QByteArray&)> cb) { m_callback = cb; }

void AutoSender::start()
{
    if (!m_callback || m_data.isEmpty()) return;
    m_sentCount = 0;
    m_timer.start(m_interval);
}

void AutoSender::stop()
{
    m_timer.stop();
}

bool AutoSender::isRunning() const
{
    return m_timer.isActive();
}

void AutoSender::onTimer()
{
    if (!m_callback) return;
    m_callback(m_data);
    m_sentCount++;
    emit sent(m_sentCount);

    if (m_repeatCount > 0 && m_sentCount >= m_repeatCount) {
        stop();
        emit finished();
    }
}

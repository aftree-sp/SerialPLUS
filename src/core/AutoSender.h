#pragma once
#include <QObject>
#include <QTimer>
#include <QByteArray>
#include <QString>
#include <functional>

class AutoSender : public QObject
{
    Q_OBJECT
public:
    explicit AutoSender(QObject *parent = nullptr);

    void setData(const QByteArray &data);
    void setInterval(int ms);
    void setRepeatCount(int count); // -1 = infinite
    void setSendCallback(std::function<bool(const QByteArray&)> cb);

    void start();
    void stop();
    bool isRunning() const;

    int sentCount() const { return m_sentCount; }
    int interval() const { return m_interval; }

signals:
    void sent(int count);
    void finished();

private slots:
    void onTimer();

private:
    QTimer               m_timer;
    QByteArray           m_data;
    int                  m_interval    = 1000;
    int                  m_repeatCount = -1;
    int                  m_sentCount   = 0;
    std::function<bool(const QByteArray&)> m_callback;
};

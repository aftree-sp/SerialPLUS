#pragma once
#include <QStatusBar>
#include <QLabel>

class StatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit StatusBar(QWidget *parent = nullptr);

    void setConnected(bool connected);
    void setPortInfo(const QString &port, int baud);
    void setRxTx(qint64 rx, qint64 tx);

private:
    QLabel *m_connLabel  = nullptr;
    QLabel *m_portLabel  = nullptr;
    QLabel *m_rxTxLabel  = nullptr;
    QLabel *m_timeLabel  = nullptr;
};

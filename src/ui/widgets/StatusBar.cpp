#include "StatusBar.h"
#include <QTimer>
#include <QDateTime>

StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent)
{
    setSizeGripEnabled(false);

    m_connLabel = new QLabel("● Disconnected", this);
    m_connLabel->setStyleSheet("color: #888; padding: 0 8px;");
    addPermanentWidget(m_connLabel);

    m_portLabel = new QLabel("", this);
    m_portLabel->setStyleSheet("color: #888; padding: 0 8px;");
    addPermanentWidget(m_portLabel);

    m_rxTxLabel = new QLabel("RX: 0  TX: 0", this);
    m_rxTxLabel->setStyleSheet("color: #888; padding: 0 8px;");
    addPermanentWidget(m_rxTxLabel);

    m_timeLabel = new QLabel(this);
    m_timeLabel->setStyleSheet("color: #666; padding: 0 8px;");
    addPermanentWidget(m_timeLabel);

    // Clock timer
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        m_timeLabel->setText(QDateTime::currentDateTime().toString("HH:mm:ss"));
    });
    timer->start(1000);
    m_timeLabel->setText(QDateTime::currentDateTime().toString("HH:mm:ss"));
}

void StatusBar::setConnected(bool connected)
{
    if (connected) {
        m_connLabel->setText("● Connected");
        m_connLabel->setStyleSheet("color: #4CAF50; padding: 0 8px; font-weight: 600;");
    } else {
        m_connLabel->setText("● Disconnected");
        m_connLabel->setStyleSheet("color: #888; padding: 0 8px;");
        m_portLabel->setText("");
    }
}

void StatusBar::setPortInfo(const QString &port, int baud)
{
    m_portLabel->setText(QString("%1 @ %2 bps").arg(port).arg(baud));
}

void StatusBar::setRxTx(qint64 rx, qint64 tx)
{
    m_rxTxLabel->setText(QString("RX: %1  TX: %2").arg(rx).arg(tx));
}

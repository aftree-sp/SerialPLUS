#include "MaterialCard.h"
#include <QPainter>
#include <QPaintEvent>
#include <QGraphicsDropShadowEffect>

MaterialCard::MaterialCard(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground);
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(12);
    shadow->setOffset(0, 3);
    shadow->setColor(QColor(0, 0, 0, 80));
    setGraphicsEffect(shadow);
}

void MaterialCard::setElevation(int dp)
{
    m_elevation = dp;
    auto *shadow = qobject_cast<QGraphicsDropShadowEffect*>(graphicsEffect());
    if (shadow) {
        shadow->setBlurRadius(dp * 4);
        shadow->setOffset(0, dp);
    }
}

void MaterialCard::setBorderRadius(int r)
{
    m_radius = r;
    update();
}

void MaterialCard::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0x31, 0x31, 0x45));
    p.drawRoundedRect(rect().adjusted(2, 2, -2, -2), m_radius, m_radius);
}

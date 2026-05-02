#include "MaterialButton.h"
#include <QPainter>
#include <QPaintEvent>

MaterialButton::MaterialButton(const QString &text, QWidget *parent, ButtonType type)
    : QPushButton(text, parent), m_type(type), m_accent(QColor("#7C4DFF"))
{
    setButtonType(type);
    setCursor(Qt::PointingHandCursor);
}

void MaterialButton::setButtonType(ButtonType type)
{
    m_type = type;
    setFlat(type == Text || type == Outlined);
}

void MaterialButton::setAccentColor(const QColor &color)
{
    m_accent = color;
    update();
}

void MaterialButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRect r = rect().adjusted(1, 1, -1, -1);

    switch (m_type) {
    case Filled: {
        QColor bg = isDown() ? m_accent.darker(120) :
                    underMouse() ? m_accent.lighter(115) : m_accent;
        p.setPen(Qt::NoPen);
        p.setBrush(bg);
        p.drawRoundedRect(r, 6, 6);
        p.setPen(Qt::white);
        p.drawText(r, Qt::AlignCenter, text());
        break;
    }
    case Outlined: {
        p.setPen(QPen(m_accent, 1.5));
        p.setBrush(underMouse() ? QColor(m_accent.red(), m_accent.green(), m_accent.blue(), 30) : Qt::transparent);
        p.drawRoundedRect(r, 6, 6);
        p.setPen(m_accent);
        p.drawText(r, Qt::AlignCenter, text());
        break;
    }
    case Text:
    case FAB:
    default:
        QPushButton::paintEvent(event);
        break;
    }
}

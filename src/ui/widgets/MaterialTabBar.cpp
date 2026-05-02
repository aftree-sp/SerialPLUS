#include "MaterialTabBar.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>

MaterialTabBar::MaterialTabBar(QWidget *parent) : QTabBar(parent)
{
    setDrawBase(false);
    setExpanding(false);
}

QSize MaterialTabBar::tabSizeHint(int index) const
{
    QSize s = QTabBar::tabSizeHint(index);
    s.setHeight(40);
    return s;
}

void MaterialTabBar::paintEvent(QPaintEvent *event)
{
    // Use default paint for now; could customize ink ripple etc.
    QTabBar::paintEvent(event);
}

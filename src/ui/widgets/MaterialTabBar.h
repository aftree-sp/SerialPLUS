#pragma once
#include <QTabBar>

class MaterialTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit MaterialTabBar(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize tabSizeHint(int index) const override;
};

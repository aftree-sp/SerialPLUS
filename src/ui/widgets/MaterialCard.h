#pragma once
#include <QWidget>

class MaterialCard : public QWidget
{
    Q_OBJECT
public:
    explicit MaterialCard(QWidget *parent = nullptr);
    void setElevation(int dp);
    void setBorderRadius(int r);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_elevation = 2;
    int m_radius    = 8;
};

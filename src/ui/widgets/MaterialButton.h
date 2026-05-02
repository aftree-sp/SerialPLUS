#pragma once
#include <QPushButton>
#include <QString>

class MaterialButton : public QPushButton
{
    Q_OBJECT
public:
    enum ButtonType { Filled, Outlined, Text, FAB };

    explicit MaterialButton(const QString &text = "", QWidget *parent = nullptr,
                            ButtonType type = Text);

    void setButtonType(ButtonType type);
    void setAccentColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    ButtonType m_type;
    QColor     m_accent;
};

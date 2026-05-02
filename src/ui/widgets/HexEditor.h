#pragma once
#include <QLineEdit>
#include <QByteArray>

// Hex input field: auto-formats input as space-separated HEX bytes
class HexEditor : public QLineEdit
{
    Q_OBJECT
public:
    explicit HexEditor(QWidget *parent = nullptr);

    QByteArray hexData() const;
    void setHexData(const QByteArray &data);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void formatInput();

private:
    bool m_formatting = false;
};

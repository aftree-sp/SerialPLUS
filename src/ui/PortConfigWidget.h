#pragma once
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include "../core/SerialManager.h"

class MaterialButton;

class PortConfigWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PortConfigWidget(QWidget *parent = nullptr);

    SerialConfig config() const;
    void setConnected(bool connected);

signals:
    void connectRequested();
    void portNameChanged(const QString &name);

private slots:
    void refreshPorts();
    void onPortChanged(int index);

private:
    void setupUi();

    QComboBox    *m_portCombo   = nullptr;
    QComboBox    *m_baudCombo   = nullptr;
    QComboBox    *m_dataBits    = nullptr;
    QComboBox    *m_stopBits    = nullptr;
    QComboBox    *m_parity      = nullptr;
    QComboBox    *m_flowCtrl    = nullptr;
    QPushButton  *m_refreshBtn  = nullptr;
    QPushButton  *m_connectBtn  = nullptr;
    QLabel       *m_statusDot   = nullptr;
    bool          m_connected   = false;
};

#pragma once
#include <QWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QString>

class SerialManager;
class DataLogger;
class MacroManager;
class AutoSender;
class ScriptEngine;
class ProtocolParser;
class PortConfigWidget;
class TerminalWidget;
class MacroPanel;
class PlotWidget;
class ProtocolPanel;
class ScriptPanel;
class LogPanel;

class SerialTab : public QWidget
{
    Q_OBJECT
public:
    explicit SerialTab(QWidget *parent = nullptr);
    ~SerialTab();

    void disconnectPort();

signals:
    void portNameChanged(const QString &name);
    void connectionChanged(bool connected);
    void statusMessage(const QString &msg);

private slots:
    void onConnectClicked();
    void onDataReceived(const QByteArray &data);
    void onSendRequested(const QByteArray &data);
    void onPortError(const QString &err);

private:
    void setupLayout();
    void connectSignals();

    // Core
    SerialManager   *m_serial     = nullptr;
    DataLogger      *m_logger     = nullptr;
    MacroManager    *m_macros     = nullptr;
    AutoSender      *m_autoSend   = nullptr;
    ScriptEngine    *m_scripts    = nullptr;
    ProtocolParser  *m_proto      = nullptr;

    // UI
    QSplitter       *m_vSplitter  = nullptr;
    QSplitter       *m_hSplitter  = nullptr;
    PortConfigWidget *m_portCfg   = nullptr;
    TerminalWidget  *m_terminal   = nullptr;
    QTabWidget      *m_sideTabs   = nullptr;
    MacroPanel      *m_macroPanel = nullptr;
    PlotWidget      *m_plotWidget = nullptr;
    ProtocolPanel   *m_protoPanel = nullptr;
    ScriptPanel     *m_scriptPanel = nullptr;
    LogPanel        *m_logPanel   = nullptr;
};

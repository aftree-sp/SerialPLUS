#pragma once
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>

class MacroManager;
class AutoSender;

class MacroPanel : public QWidget
{
    Q_OBJECT
public:
    explicit MacroPanel(MacroManager *mgr, AutoSender *sender, QWidget *parent = nullptr);

signals:
    void sendRequested(const QByteArray &data);

private slots:
    void onAddMacro();
    void onRemoveMacro();
    void onEditMacro();
    void onSendMacro(int index);
    void refreshList();
    void onAutoSendToggle(bool on);

private:
    void setupUi();
    void rebuildButtons();

    MacroManager *m_mgr    = nullptr;
    AutoSender   *m_sender = nullptr;

    QListWidget  *m_listWidget  = nullptr;
    QPushButton  *m_addBtn      = nullptr;
    QPushButton  *m_removeBtn   = nullptr;
    QPushButton  *m_editBtn     = nullptr;

    // Auto sender controls
    QGroupBox    *m_autoGroup   = nullptr;
    QLineEdit    *m_autoData    = nullptr;
    QCheckBox    *m_autoHex     = nullptr;
    QSpinBox     *m_intervalSpin = nullptr;
    QSpinBox     *m_repeatSpin  = nullptr;
    QPushButton  *m_autoToggle  = nullptr;
    QLabel       *m_sentLabel   = nullptr;

    QWidget      *m_btnContainer = nullptr;
};

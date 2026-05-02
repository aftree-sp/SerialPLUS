#pragma once
#include <QWidget>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include "../core/ScriptEngine.h"

class ScriptPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptPanel(ScriptEngine *engine, QWidget *parent = nullptr);

    void appendOutput(const QString &text);
    void appendError(const QString &text);

private slots:
    void onRunClicked();
    void onStopClicked();
    void onClearClicked();
    void onLoadScript();
    void onSaveScript();

private:
    void setupUi();

    ScriptEngine    *m_engine = nullptr;
    QPlainTextEdit  *m_editor = nullptr;
    QPlainTextEdit  *m_output = nullptr;
    QComboBox       *m_langCombo = nullptr;
    QPushButton     *m_runBtn    = nullptr;
    QPushButton     *m_stopBtn   = nullptr;
    QPushButton     *m_clearBtn  = nullptr;
    QPushButton     *m_loadBtn   = nullptr;
    QPushButton     *m_saveBtn   = nullptr;
    QLabel          *m_statusLbl = nullptr;
};

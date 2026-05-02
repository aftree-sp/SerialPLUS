#include "ScriptPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QFont>
#include <QDateTime>

ScriptPanel::ScriptPanel(ScriptEngine *engine, QWidget *parent)
    : QWidget(parent), m_engine(engine)
{
    setupUi();
    connect(m_engine, &ScriptEngine::scriptFinished, this, [this]() {
        m_runBtn->setEnabled(true);
        m_stopBtn->setEnabled(false);
        m_statusLbl->setText("脚本执行完毕");
    });
}

void ScriptPanel::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    // Toolbar
    auto *toolbar = new QHBoxLayout();
    toolbar->addWidget(new QLabel("语言：", this));
    m_langCombo = new QComboBox(this);
    m_langCombo->addItem("Python", ScriptEngine::Python);
    m_langCombo->addItem("Lua",    ScriptEngine::Lua);
    toolbar->addWidget(m_langCombo);
    toolbar->addStretch();
    m_loadBtn = new QPushButton("📂 加载", this);
    m_saveBtn = new QPushButton("💾 保存", this);
    m_runBtn  = new QPushButton("▶ 运行",   this);
    m_stopBtn = new QPushButton("■ 停止",  this);
    m_runBtn->setObjectName("sendBtn");
    m_stopBtn->setEnabled(false);
    toolbar->addWidget(m_loadBtn);
    toolbar->addWidget(m_saveBtn);
    toolbar->addWidget(m_runBtn);
    toolbar->addWidget(m_stopBtn);
    layout->addLayout(toolbar);

    // Editor + output splitter
    auto *splitter = new QSplitter(Qt::Vertical, this);

    m_editor = new QPlainTextEdit(this);
    m_editor->setObjectName("scriptEditor");
    QFont f("Consolas", 10);
    f.setFixedPitch(true);
    m_editor->setFont(f);
    m_editor->setPlaceholderText(
        "# Python script example:\n"
        "# Use serial.send(b'Hello') to send data\n"
        "# Access received data via serial.data\n"
        "import time\n"
        "print('Script started')\n"
    );
    splitter->addWidget(m_editor);

    m_output = new QPlainTextEdit(this);
    m_output->setObjectName("scriptOutput");
    m_output->setReadOnly(true);
    m_output->setMaximumBlockCount(2000);
    m_output->setFont(f);
    m_output->setPlaceholderText("Script output will appear here...");
    splitter->addWidget(m_output);
    splitter->setSizes({200, 120});

    layout->addWidget(splitter, 1);

    // Status
    auto *statusRow = new QHBoxLayout();
    m_statusLbl = new QLabel("就绪", this);
    m_statusLbl->setObjectName("scriptStatus");
    m_clearBtn = new QPushButton("清除输出", this);
    m_clearBtn->setObjectName("clearBtn");
    statusRow->addWidget(m_statusLbl);
    statusRow->addStretch();
    statusRow->addWidget(m_clearBtn);
    layout->addLayout(statusRow);

    connect(m_runBtn,   &QPushButton::clicked, this, &ScriptPanel::onRunClicked);
    connect(m_stopBtn,  &QPushButton::clicked, this, &ScriptPanel::onStopClicked);
    connect(m_clearBtn, &QPushButton::clicked, this, &ScriptPanel::onClearClicked);
    connect(m_loadBtn,  &QPushButton::clicked, this, &ScriptPanel::onLoadScript);
    connect(m_saveBtn,  &QPushButton::clicked, this, &ScriptPanel::onSaveScript);
}

void ScriptPanel::appendOutput(const QString &text)
{
    m_output->appendHtml(
        QString("<span style='color:#A5D6A7'>%1</span>").arg(text.toHtmlEscaped()));
}

void ScriptPanel::appendError(const QString &text)
{
    m_output->appendHtml(
        QString("<span style='color:#EF5350'>%1</span>").arg(text.toHtmlEscaped()));
}

void ScriptPanel::onRunClicked()
{
    QString code = m_editor->toPlainText();
    if (code.trimmed().isEmpty()) return;
    ScriptEngine::Language lang = static_cast<ScriptEngine::Language>(
        m_langCombo->currentData().toInt());
    m_output->appendHtml(QString("<span style='color:#888'>--- 运行时间：%1 ---</span>")
                         .arg(QDateTime::currentDateTime().toString("HH:mm:ss")));
    if (m_engine->runScript(code, lang)) {
        m_runBtn->setEnabled(false);
        m_stopBtn->setEnabled(true);
        m_statusLbl->setText("运行中...");
    } else {
        m_statusLbl->setText("启动失败");
    }
}

void ScriptPanel::onStopClicked()
{
    m_engine->stopScript();
    m_runBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);
    m_statusLbl->setText("已停止");
}

void ScriptPanel::onClearClicked()
{
    m_output->clear();
}

void ScriptPanel::onLoadScript()
{
    QString path = QFileDialog::getOpenFileName(this, "加载脚本", "",
        "脚本文件 (*.py *.lua);;所有文件 (*)");
    if (path.isEmpty()) return;
    QFile f(path);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_editor->setPlainText(QString::fromUtf8(f.readAll()));
    }
}

void ScriptPanel::onSaveScript()
{
    QString path = QFileDialog::getSaveFileName(this, "保存脚本", "",
        "Python 脚本 (*.py);;Lua 脚本 (*.lua);;所有文件 (*)");
    if (path.isEmpty()) return;
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream ts(&f);
        ts << m_editor->toPlainText();
    }
}

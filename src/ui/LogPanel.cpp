#include "LogPanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QGroupBox>
#include <QFileDialog>
#include <QDateTime>
#include <QDir>

LogPanel::LogPanel(DataLogger *logger, QWidget *parent)
    : QWidget(parent), m_logger(logger)
{
    setupUi();
    connect(m_logger, &DataLogger::loggingStarted, this, [this](const QString &path) {
        m_statusLbl->setText("● 记录中：" + path);
        m_toggleBtn->setText("■ 停止记录");
    });
    connect(m_logger, &DataLogger::loggingStopped, this, [this]() {
        m_statusLbl->setText("● 已停止");
        m_toggleBtn->setText("▶ 开始记录");
    });
}

void LogPanel::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    auto *group = new QGroupBox("数据记录", this);
    auto *form  = new QFormLayout(group);

    auto *pathRow = new QHBoxLayout();
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setPlaceholderText("日志文件路径...");
    // Default path
    m_pathEdit->setText(QDir::homePath() + "/serial_log_" +
        QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".txt");
    m_browseBtn = new QPushButton("浏览", this);
    m_browseBtn->setObjectName("macroBtn");
    pathRow->addWidget(m_pathEdit, 1);
    pathRow->addWidget(m_browseBtn);
    form->addRow("文件：", pathRow);

    m_tsCheck = new QCheckBox("添加时间戳", this);
    m_tsCheck->setChecked(true);
    form->addRow("", m_tsCheck);

    m_hexCheck = new QCheckBox("以 HEX 格式记录", this);
    form->addRow("", m_hexCheck);

    m_toggleBtn = new QPushButton("▶ 开始记录", this);
    m_toggleBtn->setObjectName("sendBtn");
    form->addRow(m_toggleBtn);

    layout->addWidget(group);

    m_statusLbl = new QLabel("● 已停止", this);
    m_statusLbl->setObjectName("logStatus");
    layout->addWidget(m_statusLbl);
    layout->addStretch();

    connect(m_browseBtn, &QPushButton::clicked, this, &LogPanel::onBrowse);
    connect(m_toggleBtn, &QPushButton::clicked, this, &LogPanel::onStartStop);
    connect(m_tsCheck,  &QCheckBox::toggled, this, [this](bool on) {
        m_logger->setTimestamp(on);
    });
    connect(m_hexCheck, &QCheckBox::toggled, this, [this](bool on) {
        m_logger->setHexMode(on);
    });
}

void LogPanel::onStartStop()
{
    if (m_logger->isLogging()) {
        m_logger->stopLogging();
    } else {
        m_logger->setTimestamp(m_tsCheck->isChecked());
        m_logger->setHexMode(m_hexCheck->isChecked());
        m_logger->startLogging(m_pathEdit->text());
    }
}

void LogPanel::onBrowse()
{
    QString path = QFileDialog::getSaveFileName(this, "Log File", m_pathEdit->text(),
        "文本文件 (*.txt *.log);;所有文件 (*)");
    if (!path.isEmpty()) m_pathEdit->setText(path);
}

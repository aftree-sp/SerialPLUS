#pragma once
#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include "../core/DataLogger.h"

class LogPanel : public QWidget
{
    Q_OBJECT
public:
    explicit LogPanel(DataLogger *logger, QWidget *parent = nullptr);

private slots:
    void onStartStop();
    void onBrowse();

private:
    void setupUi();

    DataLogger   *m_logger    = nullptr;
    QLabel       *m_pathLabel = nullptr;
    QLineEdit    *m_pathEdit  = nullptr;
    QPushButton  *m_browseBtn = nullptr;
    QPushButton  *m_toggleBtn = nullptr;
    QCheckBox    *m_tsCheck   = nullptr;
    QCheckBox    *m_hexCheck  = nullptr;
    QLabel       *m_statusLbl = nullptr;
};

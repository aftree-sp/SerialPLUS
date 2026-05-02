#pragma once
#include <QWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QByteArray>
#include <QLabel>
#include "widgets/HexEditor.h"

class TerminalWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TerminalWidget(QWidget *parent = nullptr);

    void appendRx(const QByteArray &data);
    void appendTx(const QByteArray &data);
    void appendError(const QString &text);
    void clearDisplay();

signals:
    void sendRequested(const QByteArray &data);

private slots:
    void onSendClicked();
    void onModeChanged(int index);
    void onClearClicked();
    void onSendLineEnterPressed();

private:
    void setupUi();
    QString formatHex(const QByteArray &data) const;
    void scrollToBottom();

    QPlainTextEdit *m_display   = nullptr;
    QLineEdit      *m_inputLine = nullptr;
    HexEditor      *m_hexInput  = nullptr;
    QComboBox      *m_modeCombo = nullptr;
    QComboBox      *m_eolCombo  = nullptr;
    QPushButton    *m_sendBtn   = nullptr;
    QPushButton    *m_clearBtn  = nullptr;
    QCheckBox      *m_autoScroll = nullptr;
    QCheckBox      *m_showHex   = nullptr;
    QCheckBox      *m_timestamp = nullptr;
    QLabel         *m_rxLabel   = nullptr;
    QLabel         *m_txLabel   = nullptr;

    enum Mode { TextMode, HexMode };
    Mode m_mode = TextMode;
    bool m_showHexDisplay = false;
    bool m_showTimestamp  = true;
    qint64 m_rxCount = 0;
    qint64 m_txCount = 0;
};

#include "HexEditor.h"
#include <QKeyEvent>
#include <QRegularExpression>

HexEditor::HexEditor(QWidget *parent) : QLineEdit(parent)
{
    setFont(QFont("Consolas", 10));
    setPlaceholderText("HEX bytes: 01 02 03 ...");
    connect(this, &QLineEdit::textEdited, this, &HexEditor::formatInput);
}

QByteArray HexEditor::hexData() const
{
    QString clean = text().simplified().remove(' ');
    if (clean.length() % 2 != 0) clean.prepend('0');
    return QByteArray::fromHex(clean.toLatin1());
}

void HexEditor::setHexData(const QByteArray &data)
{
    QString hex;
    for (int i = 0; i < data.size(); i++) {
        if (i > 0) hex += ' ';
        hex += QString("%1").arg((quint8)data[i], 2, 16, QLatin1Char('0')).toUpper();
    }
    setText(hex);
}

void HexEditor::keyPressEvent(QKeyEvent *event)
{
    // Allow only hex chars, space, backspace, delete
    QString k = event->text().toUpper();
    static QRegularExpression hexChar("[0-9A-F]");
    if (!k.isEmpty() && !hexChar.match(k).hasMatch() &&
        event->key() != Qt::Key_Backspace &&
        event->key() != Qt::Key_Delete &&
        event->key() != Qt::Key_Left &&
        event->key() != Qt::Key_Right &&
        event->key() != Qt::Key_Home &&
        event->key() != Qt::Key_End &&
        event->key() != Qt::Key_Space)
    {
        return; // ignore non-hex
    }
    QLineEdit::keyPressEvent(event);
}

void HexEditor::formatInput()
{
    if (m_formatting) return;
    m_formatting = true;

    int cursorPos = cursorPosition();
    QString raw = text().toUpper();

    // Extract only hex chars
    QString hexOnly;
    for (QChar c : raw) {
        if (c.isLetterOrNumber() && QString("0123456789ABCDEF").contains(c))
            hexOnly += c;
    }

    // Format with spaces every 2 chars
    QString formatted;
    for (int i = 0; i < hexOnly.length(); i++) {
        if (i > 0 && i % 2 == 0) formatted += ' ';
        formatted += hexOnly[i];
    }

    if (formatted != text()) {
        setText(formatted);
        // Adjust cursor
        setCursorPosition(qMin(cursorPos, formatted.length()));
    }

    m_formatting = false;
}

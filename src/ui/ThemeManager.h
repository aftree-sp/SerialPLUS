#pragma once
#include <QObject>
#include <QString>

class QApplication;

class ThemeManager : public QObject
{
    Q_OBJECT
public:
    enum Theme { Dark, Light };

    static ThemeManager &instance();

    void applyTheme(Theme t);
    Theme currentTheme() const { return m_theme; }

signals:
    void themeChanged(Theme t);

private:
    ThemeManager() = default;
    Theme m_theme = Dark;

    QString darkStyleSheet() const;
    QString lightStyleSheet() const;
};

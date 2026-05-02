#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

struct MacroItem {
    QString name;
    QString data;
    bool    isHex = false;
    QString shortcut;
    QByteArray color; // button accent color
};

class MacroManager : public QObject
{
    Q_OBJECT
public:
    explicit MacroManager(QObject *parent = nullptr);

    void addMacro(const MacroItem &item);
    void removeMacro(int index);
    void updateMacro(int index, const MacroItem &item);
    void moveMacro(int from, int to);

    const QList<MacroItem> &macros() const { return m_macros; }
    int count() const { return m_macros.size(); }

    bool saveToFile(const QString &path);
    bool loadFromFile(const QString &path);

    QByteArray getMacroData(int index) const;

signals:
    void macrosChanged();

private:
    QList<MacroItem> m_macros;
};

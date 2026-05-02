#include "MacroManager.h"
#include <QJsonDocument>
#include <QFile>
#include <QDebug>

MacroManager::MacroManager(QObject *parent) : QObject(parent) {}

void MacroManager::addMacro(const MacroItem &item)
{
    m_macros.append(item);
    emit macrosChanged();
}

void MacroManager::removeMacro(int index)
{
    if (index >= 0 && index < m_macros.size()) {
        m_macros.removeAt(index);
        emit macrosChanged();
    }
}

void MacroManager::updateMacro(int index, const MacroItem &item)
{
    if (index >= 0 && index < m_macros.size()) {
        m_macros[index] = item;
        emit macrosChanged();
    }
}

void MacroManager::moveMacro(int from, int to)
{
    if (from < 0 || from >= m_macros.size()) return;
    if (to < 0 || to >= m_macros.size()) return;
    m_macros.move(from, to);
    emit macrosChanged();
}

bool MacroManager::saveToFile(const QString &path)
{
    QJsonArray arr;
    for (const auto &m : m_macros) {
        QJsonObject obj;
        obj["name"]     = m.name;
        obj["data"]     = m.data;
        obj["isHex"]    = m.isHex;
        obj["shortcut"] = m.shortcut;
        arr.append(obj);
    }
    QJsonDocument doc(arr);
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(doc.toJson());
    return true;
}

bool MacroManager::loadFromFile(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return false;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray()) return false;

    m_macros.clear();
    for (const auto &val : doc.array()) {
        QJsonObject obj = val.toObject();
        MacroItem item;
        item.name     = obj["name"].toString();
        item.data     = obj["data"].toString();
        item.isHex    = obj["isHex"].toBool(false);
        item.shortcut = obj["shortcut"].toString();
        m_macros.append(item);
    }
    emit macrosChanged();
    return true;
}

QByteArray MacroManager::getMacroData(int index) const
{
    if (index < 0 || index >= m_macros.size()) return {};
    const MacroItem &m = m_macros[index];
    if (m.isHex)
        return QByteArray::fromHex(m.data.simplified().remove(' ').toLatin1());
    return m.data.toUtf8();
}

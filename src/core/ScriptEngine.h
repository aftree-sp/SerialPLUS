#pragma once
#include <QObject>
#include <QString>
#include <QByteArray>
#include <functional>

class ScriptEngine : public QObject
{
    Q_OBJECT
public:
    explicit ScriptEngine(QObject *parent = nullptr);

    enum Language { Python, Lua };

    void setSendCallback(std::function<bool(const QByteArray&)> cb);
    bool runScript(const QString &code, Language lang = Python);
    void stopScript();
    bool isRunning() const { return m_running; }

    void feedData(const QByteArray &data); // feed received data to script

signals:
    void output(const QString &text);
    void error(const QString &text);
    void scriptFinished();

private:
    std::function<bool(const QByteArray&)> m_sendCb;
    bool m_running = false;
    QString m_pendingData;
};

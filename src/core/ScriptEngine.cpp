#include "ScriptEngine.h"
#include <QProcess>
#include <QTemporaryFile>
#include <QDir>
#include <QTextStream>

ScriptEngine::ScriptEngine(QObject *parent) : QObject(parent) {}

void ScriptEngine::setSendCallback(std::function<bool(const QByteArray&)> cb)
{
    m_sendCb = cb;
}

bool ScriptEngine::runScript(const QString &code, Language lang)
{
    if (m_running) return false;

    // Write script to temp file
    QString suffix = (lang == Python) ? ".py" : ".lua";
    QTemporaryFile tmpFile(QDir::tempPath() + "/ss_script_XXXXXX" + suffix);
    tmpFile.setAutoRemove(false);
    if (!tmpFile.open()) {
        emit error("Cannot create temp script file");
        return false;
    }
    QTextStream ts(&tmpFile);
    ts << code;
    tmpFile.close();
    QString tmpPath = tmpFile.fileName();

    // Run via QProcess
    QProcess *proc = new QProcess(this);
    QString interpreter = (lang == Python) ? "python" : "lua";

    m_running = true;
    connect(proc, &QProcess::readyReadStandardOutput, this, [this, proc]() {
        emit output(QString::fromUtf8(proc->readAllStandardOutput()));
    });
    connect(proc, &QProcess::readyReadStandardError, this, [this, proc]() {
        emit error(QString::fromUtf8(proc->readAllStandardError()));
    });
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, proc, tmpPath](int, QProcess::ExitStatus) {
        m_running = false;
        QFile::remove(tmpPath);
        proc->deleteLater();
        emit scriptFinished();
    });

    proc->start(interpreter, {tmpPath});
    if (!proc->waitForStarted(3000)) {
        emit error(QString("Cannot start interpreter '%1'. Make sure it is in PATH.").arg(interpreter));
        m_running = false;
        QFile::remove(tmpPath);
        proc->deleteLater();
        return false;
    }
    return true;
}

void ScriptEngine::stopScript()
{
    // Not easily stoppable without process ref; simplified
    m_running = false;
}

void ScriptEngine::feedData(const QByteArray &data)
{
    m_pendingData += QString::fromUtf8(data);
}

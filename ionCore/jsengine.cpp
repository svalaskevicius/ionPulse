#include "jsengine.h"
#include "shared.h"

#include <stdexcept>
#include <QSet>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QProcess>
#include <QCoreApplication>
#include <QKeyEvent>

namespace IonCore {

namespace Private {




bool loadFile(QString fileName, QScriptEngine *engine)
{
    // avoid loading files more than once
    static QSet<QString> loadedFiles;
    QFileInfo fileInfo(fileName);
    QString absoluteFileName = fileInfo.absoluteFilePath();
    QString absolutePath = fileInfo.absolutePath();
    QString canonicalFileName = fileInfo.canonicalFilePath();
    if (loadedFiles.contains(canonicalFileName)) {
        return true;
    }
    loadedFiles.insert(canonicalFileName);

    // load the file
    QFile file(fileName);
    if (file.open(QFile::ReadOnly)) {
        QTextStream stream(&file);
        QString contents = stream.readAll();
        file.close();

        int endlineIndex = contents.indexOf('\n');
        QString line = contents.left(endlineIndex);
        int lineNumber = 1;

        // strip off #!/usr/bin/env qscript line
        if (line.startsWith("#!")) {
            contents.remove(0, endlineIndex+1);
            ++lineNumber;
        }

        // set qt.script.absoluteFilePath
        QScriptValue script = engine->globalObject().property("qs").property("script");
        QScriptValue oldFilePathValue = script.property("absoluteFilePath");
        QScriptValue oldPathValue = script.property("absolutePath");
        script.setProperty("absoluteFilePath", engine->toScriptValue(absoluteFileName));
        script.setProperty("absolutePath", engine->toScriptValue(absolutePath));

        QScriptValue r = engine->evaluate(contents, fileName, lineNumber);
        if (engine->hasUncaughtException()) {
            QStringList backtrace = engine->uncaughtExceptionBacktrace();
            qDebug() << QString("    %1\n%2\n\n").arg(r.toString()).arg(backtrace.join("\n"));
            return true;
        }
        script.setProperty("absoluteFilePath", oldFilePathValue); // if we come from includeScript(), or whereever
        script.setProperty("absolutePath", oldPathValue); // if we come from includeScript(), or whereever
    } else {
        return false;
    }
    return true;
}

QScriptValue includeScript(QScriptContext *context, QScriptEngine *engine)
{
    QString importFile = context->argument(0).toString();
    QFileInfo importInfo(importFile);
    if (importInfo.isRelative()) {
        QString currentFileName = engine->globalObject().property("qs").property("script").property("absoluteFilePath").toString();
        QFileInfo currentFileInfo(currentFileName);
        QString path = currentFileInfo.path();
        importFile =  path + "/" + importInfo.filePath();
    }
    if (!loadFile(importFile, engine)) {
        return context->throwError(QString("Failed to resolve include: %1").arg(importFile));
    }
    return engine->toScriptValue(true);
}

QScriptValue importExtension(QScriptContext *context, QScriptEngine *engine)
{
    return engine->importExtension(context->argument(0).toString());
}

QScriptValue installAppShortcut(QScriptContext *context, QScriptEngine *engine)
{
    return engine->newQObject(
        new AppShortcut((Qt::Key)context->argument(0).toInt32(), (context->argumentCount()==2)?context->argument(1).toQObject():NULL),
        QScriptEngine::ScriptOwnership
    );
}

QScriptValue jsDebug(QScriptContext *context, QScriptEngine *engine)
{
    DEBUG_MSG(context->argument(0).toString());
    return engine->nullValue();
}


JsEngine::JsEngine()
{
    global = scriptEngine.globalObject();
    // add the qt object
    global.setProperty("qs", scriptEngine.newObject());
    // add a 'script' object
    script = scriptEngine.newObject();
    global.property("qs").setProperty("script", script);
    // add a 'system' object
    system = scriptEngine.newObject();
    global.property("qs").setProperty("system", system);

    global.setProperty("debug", scriptEngine.newFunction(IonCore::Private::jsDebug));

    initialiseJsFramework();
}

void JsEngine::loadFile(QString file)
{
    IonCore::Private::loadFile(file, &scriptEngine);
}


void JsEngine::initialiseJsFramework()
{
    // add environment variables to qt.system.env
    QMap<QString,QVariant> envMap;
    QStringList envList = QProcess::systemEnvironment();
    foreach (const QString &entry, envList) {
        QStringList keyVal = entry.split('=');
        if (keyVal.size() == 1)
            envMap.insert(keyVal.at(0), QString());
        else
            envMap.insert(keyVal.at(0), keyVal.at(1));
    }

    system.setProperty("installAppShortcut", scriptEngine.newFunction(IonCore::Private::installAppShortcut));
    system.setProperty("env", scriptEngine.toScriptValue(envMap));

    // add the include functionality to qt.script.include
    script.setProperty("include", scriptEngine.newFunction(IonCore::Private::includeScript));
    // add the importExtension functionality to qt.script.importExtension
    script.setProperty("importExtension", scriptEngine.newFunction(IonCore::Private::importExtension));
}


AppShortcut::AppShortcut(Qt::Key key, QObject *owner) : key(key), owner(owner)
{
    qApp->installEventFilter(this);
}

bool AppShortcut::eventFilter(QObject *obj, QEvent *event)
{
    if (owner && (owner != obj)) {
        return false;
    }
    if (QEvent::KeyPress == event->type()) {
        QKeyEvent *kev = static_cast<QKeyEvent*>(event);
        if (kev->key() == key) {
            emit callback();
            return true;
        }
    }
    return false;
}

}
}

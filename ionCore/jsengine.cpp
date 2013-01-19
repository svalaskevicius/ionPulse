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
#include <QTextEdit>


namespace IonCore {

namespace Private {




bool loadFile(QString fileName, QJSEngine *engine)
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
        QJSValue script = engine->globalObject().property("qs").property("script");
        QJSValue oldFilePathValue = script.property("absoluteFilePath");
        QJSValue oldPathValue = script.property("absolutePath");
        script.setProperty("absoluteFilePath", engine->toScriptValue(absoluteFileName));
        script.setProperty("absolutePath", engine->toScriptValue(absolutePath));

        QJSValue r = engine->evaluate(contents, fileName, lineNumber);
        if (r.isError()) {
            qDebug() << QString("    %1\n\n").arg(r.toString());
            return true;
        }
        script.setProperty("absoluteFilePath", oldFilePathValue); // if we come from includeScript(), or whereever
        script.setProperty("absolutePath", oldPathValue); // if we come from includeScript(), or whereever
    } else {
        return false;
    }
    return true;
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

    initialiseJsFramework();
}


bool JsEngine::includeScript(QString importFile)
{
    QFileInfo importInfo(importFile);
    if (importInfo.isRelative()) {
        QString currentFileName = scriptEngine.globalObject().property("qs").property("script").property("absoluteFilePath").toString();
        QFileInfo currentFileInfo(currentFileName);
        QString path = currentFileInfo.path();
        importFile =  path + "/" + importInfo.filePath();
    }
    return this->loadFile(importFile);
}
/*
QScriptValue importExtension(QScriptContext *context, QScriptEngine *engine)
{
    return engine->importExtension(context->argument(0).toString());
}

QScriptValue installAppShortcut(QScriptContext *context, QScriptEngine *engine)
{
    return engine->newQObject(
        new AppShortcut(
                (Qt::Key)context->argument(0).toUInt32(),
                (context->argumentCount()>=2)?(Qt::KeyboardModifier)context->argument(1).toUInt32():Qt::NoModifier,
                (context->argumentCount()>=3)?context->argument(2).toQObject():NULL
        ),
        QScriptEngine::ScriptOwnership
    );
}

QScriptValue jsDebug(QScriptContext *context, QScriptEngine *engine)
{
    DEBUG_MSG(context->argument(0).toString());
    return engine->nullValue();
}

QScriptValue getTextEditExtraSelectionCursor(QScriptContext *context, QScriptEngine *)
{
   QTextEdit::ExtraSelection *selection = qscriptvalue_cast<QTextEdit::ExtraSelection*>(context->argument(0));
   return qScriptValueFromValue(context->engine(), selection->cursor);
}
QScriptValue setTextEditExtraSelectionCursor(QScriptContext *context, QScriptEngine *engine)
{
   qscriptvalue_cast<QTextEdit::ExtraSelection*>(context->argument(0))->cursor = qscriptvalue_cast<QTextCursor>(context->argument(1));
   return engine->nullValue();
}

QScriptValue getTextEditExtraSelectionFormat(QScriptContext *context, QScriptEngine *)
{
   QTextEdit::ExtraSelection *selection = qscriptvalue_cast<QTextEdit::ExtraSelection*>(context->argument(0));
   return qScriptValueFromValue(context->engine(), selection->format);
}
QScriptValue setTextEditExtraSelectionFormat(QScriptContext *context, QScriptEngine *engine)
{
   qscriptvalue_cast<QTextEdit::ExtraSelection*>(context->argument(0))->format = qscriptvalue_cast<QTextCharFormat>(context->argument(1));
   return engine->nullValue();
}
*/
bool JsEngine::loadFile(QString file)
{
    return IonCore::Private::loadFile(file, &scriptEngine);
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
    system.setProperty("env", scriptEngine.toScriptValue(envMap));

    global.setProperty("engine", scriptEngine.newQObject(this));

    /*
    global.setProperty("debug", scriptEngine.newFunction(IonCore::Private::jsDebug));

    system.setProperty("installAppShortcut", scriptEngine.newFunction(IonCore::Private::installAppShortcut));

    global.setProperty("debug", scriptEngine.newFunction(IonCore::Private::jsDebug));
    // add the include functionality to qt.script.include
    script.setProperty("include", scriptEngine.newFunction(IonCore::Private::includeScript));
    // add the importExtension functionality to qt.script.importExtension
    script.setProperty("importExtension", scriptEngine.newFunction(IonCore::Private::importExtension));
    system.setProperty("getTextEditExtraSelectionCursor", scriptEngine.newFunction(IonCore::Private::getTextEditExtraSelectionCursor));
    system.setProperty("setTextEditExtraSelectionCursor", scriptEngine.newFunction(IonCore::Private::setTextEditExtraSelectionCursor));
    system.setProperty("getTextEditExtraSelectionFormat", scriptEngine.newFunction(IonCore::Private::getTextEditExtraSelectionFormat));
    system.setProperty("setTextEditExtraSelectionFormat", scriptEngine.newFunction(IonCore::Private::setTextEditExtraSelectionFormat));
    */
}


AppShortcut::AppShortcut(Qt::Key key, Qt::KeyboardModifier modifier, QObject *owner) : key(key), modifier(modifier), owner(owner)
{
    qApp->installEventFilter(this);
}

void AppShortcut::disable()
{
    qApp->removeEventFilter(this);
}

bool AppShortcut::eventFilter(QObject *obj, QEvent *event)
{
    if (owner && (owner != obj)) {
        return false;
    }
    if (QEvent::KeyPress == event->type()) {
        QKeyEvent *kev = static_cast<QKeyEvent*>(event);
        if ((kev->key() == key) && (!modifier || ((kev->modifiers() & modifier) == modifier))) {
            // TODO: change callback to a callable js function and pass event to it, returning its return value
            // add functionality for key = 0 to capture all keys
            emit callback();
            return true;
        }
    }
    return false;
}

}
}


Q_DECLARE_METATYPE(QTextCursor)
Q_DECLARE_METATYPE(QTextCharFormat)
Q_DECLARE_METATYPE(QTextEdit::ExtraSelection*)

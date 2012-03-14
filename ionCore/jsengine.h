#ifndef JSENGINE_H
#define JSENGINE_H

#include <QScriptEngine>
#include <QScriptEngineDebugger>

namespace IonCore {

namespace Private {

class JsEngine
{
protected:
    QScriptEngine scriptEngine;
    QScriptEngineDebugger debugger;
    QScriptValue global, script, system;
    void initialiseJsFramework();
public:
    JsEngine();
    void loadFile(QString);
    QScriptEngine &getScriptEngine() {return scriptEngine;}
};



class AppShortcut : public QObject {

    Q_OBJECT

private:
    Qt::Key key;
    QObject *owner;
public:
    AppShortcut(Qt::Key key, QObject *owner);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
signals:
    void callback();
};

}
}

#endif // JSENGINE_H

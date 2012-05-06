#ifndef JSENGINE_H
#define JSENGINE_H

#include <QScriptEngine>

namespace IonCore {

namespace Private {

class JsEngine
{
protected:
    QScriptEngine scriptEngine;
    QScriptValue global, script, system;
    void initialiseJsFramework();
public:
    JsEngine();
    void loadFile(QString);
    QScriptEngine &getScriptEngine() {return scriptEngine;}
};



class AppShortcut : public QObject
{
    Q_OBJECT

private:
    Qt::Key key;
    Qt::KeyboardModifier modifier;
    QObject *owner;
public:
    AppShortcut(Qt::Key key, Qt::KeyboardModifier modifier, QObject *owner);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
signals:
    void callback();
public slots:
    void disable();
};

}
}

#endif // JSENGINE_H

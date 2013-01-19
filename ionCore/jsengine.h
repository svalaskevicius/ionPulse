#ifndef JSENGINE_H
#define JSENGINE_H

#include <QJSEngine>

namespace IonCore {

namespace Private {

class JsEngine : public QObject
{
    Q_OBJECT
protected:
    QJSEngine scriptEngine;
    QJSValue global, script, system;
    void initialiseJsFramework();
public:
    JsEngine();
    bool loadFile(QString);
    QJSEngine &getScriptEngine() {return scriptEngine;}

    Q_INVOKABLE bool includeScript(QString importFile);
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

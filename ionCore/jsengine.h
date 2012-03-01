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
};

}
}

#endif // JSENGINE_H

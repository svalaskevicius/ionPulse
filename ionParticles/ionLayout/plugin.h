/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#pragma once

#include <QObject>
#include <ionCore/plugin.h>
#include "layoutapi.h"

namespace IonLayout {

namespace Private {

class Plugin : public QObject, public LayoutPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonCore::BasicPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    virtual ~Plugin() {
        if(layoutManager) {
            delete layoutManager;
        }
    }
    void preLoad();
    QString getName() {
        return IonLayout::LayoutPlugin::name();
    }
    virtual LayoutManager *getLayoutManager();
    virtual void registerJsApi(QScriptEngine & jsEngine);
private:
    LayoutManager *layoutManager;
signals:

public slots:
protected slots:

};

}
}






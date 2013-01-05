/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONDBXML_H
#define IONDBXML_H

#include <QObject>
#include <ionCore/plugin.h>

#include "dbxmlapi.h"

#include <ionCore/shared.h>

namespace IonDbXml {
namespace Private {

class Plugin : public QObject, public IonDbXml::DbXmlPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.ionPulse.ionDbXml")
    Q_INTERFACES(IonCore::BasicPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    virtual ~Plugin();
    QString getName() {
        return DbXmlPlugin::name();
    }
    void preLoad();
    Q_INVOKABLE virtual IonDbXml::DataStorage *getStorage() { return structureStorage; }

    void registerJsApi(QScriptEngine & jsEngine);
protected:
    DataStorage* structureStorage;
signals:

public slots:

};

}
}

Q_DECLARE_METATYPE(IonDbXml::DataValue*)
Q_DECLARE_METATYPE(IonDbXml::DataStorage*)
Q_DECLARE_METATYPE(IonDbXml::DataQueryResults*)
Q_DECLARE_METATYPE(IonDbXml::XmlNode*)


#endif // IONDBXML_H





/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "plugin.h"

#include <QtPlugin>
#include "ionCore/shared.h"

#include "datastorage.h"

namespace IonDbXml {
namespace Private {

Plugin::Plugin(QObject *parent) :
    QObject(parent)
{
    structureStorage = NULL;
}

Plugin::~Plugin()
{
    if (structureStorage) {
        delete structureStorage;
    }
}

void Plugin::preLoad()
{
    structureStorage = new DataStorageImpl();
    structureStorage->setParent(this);
}


void Plugin::registerJsApi(QJSEngine & jsEngine)
{
//    qScriptRegisterMetaType(&jsEngine, qObjectPtrToScriptValue<IonDbXml::DataValue>, qObjectPtrFromScriptValue<IonDbXml::DataValue>);
//    qScriptRegisterMetaType(&jsEngine, qObjectPtrToScriptValue<IonDbXml::DataQueryResults>, qObjectPtrFromScriptValue<IonDbXml::DataQueryResults>);
//    qScriptRegisterMetaType(&jsEngine, qObjectPtrToScriptValue<IonDbXml::DataStorage>, qObjectPtrFromScriptValue<IonDbXml::DataStorage>);
//    qScriptRegisterMetaType(&jsEngine, qObjectPtrToScriptValue<IonDbXml::XmlNode>, qObjectPtrFromScriptValue<IonDbXml::XmlNode>);
//    qScriptRegisterMetaType(&jsEngine, qObjectPtrToScriptValue<IonDbXml::XmlNodeIteratorJsAdapter>, qObjectPtrFromScriptValue<IonDbXml::XmlNodeIteratorJsAdapter>);

    jsEngine.globalObject().setProperty("dbxml", jsEngine.newQObject(this));
}

}

}



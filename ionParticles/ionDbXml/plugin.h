/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef IONEDITORPHP_H
#define IONEDITORPHP_H

#include <QObject>
#include <ionCore/plugin.h>

#include "dbxmlapi.h"

namespace IonDbXml {
namespace Private {


class Plugin : public QObject, public IonDbXml::DbXmlPlugin
{
    Q_OBJECT
    Q_INTERFACES(IonCore::BasicPlugin)
public:
    explicit Plugin(QObject *parent = 0);
    ~Plugin();
    QString getName() {
        return DbXmlPlugin::name();
    }
    void preLoad();
    virtual DataStorage *getStorage() { return structureStorage; }
protected:
    DataStorage* structureStorage;
signals:

public slots:

};

}
}

#endif // IONEDITORPHP_H





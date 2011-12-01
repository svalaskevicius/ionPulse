/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PHPTREEMODEL_H
#define PHPTREEMODEL_H

#include <ionParticles/ionProject/projectapi.h>
#include <QVector>
#include <QString>
#include "structurestorage.h"

namespace IonPhp {

class PhpTreeSource : public IonProject::TreeModelSource {
public:
    PhpTreeSource(StructureStorage &storage, QSharedPointer<IonProject::TreeItemFactory> treeItemFactory)
        : treeItemFactory(treeItemFactory), storage(storage) {
    }
    virtual IonProject::TreeBranch * setupData();
    QString getTitle() const {return "Class Browser";}
private:
    QSharedPointer<IonProject::TreeItemFactory> treeItemFactory;
    StructureStorage &storage;
};

}

#endif // PHPTREEMODEL_H

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
namespace Private {

class PhpTreeModelSourceDecorator : public IonProject::TreeModelSource {
public:
    PhpTreeModelSourceDecorator(QSharedPointer<IonProject::TreeModelSource> origTreeModelSource, StructureStorage &storage, QSharedPointer<IonProject::TreeItemFactory> treeItemFactory)
        : origTreeModelSource(origTreeModelSource), treeItemFactory(treeItemFactory), storage(storage) {
    }
    virtual IonProject::TreeItem * setupData();
    QString getTitle() const {return origTreeModelSource->getTitle();}
protected:
    void decorateNode(IonProject::TreeItem *node);
    void addPhpFileInfo(IonProject::TreeItem *node, QString path);
    int getStoredFile(QString path);
    int storeFile(QString path);
private:
    QSharedPointer<IonProject::TreeModelSource> origTreeModelSource;
    QSharedPointer<IonProject::TreeItemFactory> treeItemFactory;
    StructureStorage &storage;
};

}
}

#endif // PHPTREEMODEL_H

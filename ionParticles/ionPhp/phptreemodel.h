#ifndef PHPTREEMODEL_H
#define PHPTREEMODEL_H

#include <ionParticles/ionProject/projectapi.h>
#include <QVector>
#include <QString>
#include "structurestorage.h"

namespace IonPhp {

class PhpTreeSource : public IonProject::TreeModelSource {
public:
    PhpTreeSource(StructureStorage &storage, IonProject::TreeItemFactory &treeItemFactory)
        : treeItemFactory(treeItemFactory), storage(storage) {
    }
    virtual IonProject::TreeBranch * setupData();
    QString getTitle() const {return "Class Browser";}
private:
    IonProject::TreeItemFactory &treeItemFactory;
    StructureStorage &storage;
};

}

#endif // PHPTREEMODEL_H

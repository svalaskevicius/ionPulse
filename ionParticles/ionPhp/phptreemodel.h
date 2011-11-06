#ifndef PHPTREEMODEL_H
#define PHPTREEMODEL_H

#include <ionParticles/ionProject/projectapi.h>
#include <QVector>
#include <QString>


namespace IonPhp {

class PhpTreeSource : public IonProject::TreeModelSource {
public:
    PhpTreeSource(const IonProject::TreeModel &fileSource, IonProject::TreeItemFactory &treeItemFactory)
        : fileSource(fileSource), treeItemFactory(treeItemFactory) {
    }
    virtual IonProject::TreeBranch * setupData();
    QString getTitle() const {return "Class browser";}

protected:
    QVector<QString> getPhpFileList();
private:
    const IonProject::TreeModel &fileSource;
    IonProject::TreeItemFactory &treeItemFactory;
};

}

#endif // PHPTREEMODEL_H

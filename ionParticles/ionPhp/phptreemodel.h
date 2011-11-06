#ifndef PHPTREEMODEL_H
#define PHPTREEMODEL_H

#include <ionParticles/ionProject/projectapi.h>
#include <QVector>
#include <QString>
#include "phpparser.h"

namespace IonPhp {

class PhpTreeSource : public IonProject::TreeModelSource {
public:
    PhpTreeSource(const IonProject::TreeModel &fileSource, IonProject::TreeItemFactory &treeItemFactory)
        : fileSource(fileSource), treeItemFactory(treeItemFactory), errors() {
    }
    virtual IonProject::TreeBranch * setupData();
    QString getTitle() const {return "Class Browser";}
    const QList<QString> &getErrors() const {return errors;}
protected:
    QVector<QString> getPhpFileList();
    void addFileToTree(QString path, IonProject::TreeBranch* root);
private:
    const IonProject::TreeModel &fileSource;
    IonProject::TreeItemFactory &treeItemFactory;
    QList<QString> errors;
};

}

#endif // PHPTREEMODEL_H

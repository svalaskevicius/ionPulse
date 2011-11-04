#ifndef PHPTREEMODEL_H
#define PHPTREEMODEL_H

#include <ionParticles/ionProject/projectapi.h>

class PhpTreeSource : public IonProject::TreeModelSource {
public:
    PhpTreeSource(const IonProject::TreeModel &fileSource)
        : fileSource(fileSource) {
    }
    virtual IonProject::TreeBranch * setupData();
    QString getTitle() const {return "Class browser";}

private:
    const IonProject::TreeModel &fileSource;
};


#endif // PHPTREEMODEL_H

#include "phptreemodel.h"
#include <QFile>
#include "phpparser.h"

namespace IonPhp {

IonProject::TreeBranch *PhpTreeSource::setupData()
{
    int cnt=0, err=0;
    foreach (QString path, getPhpFileList()) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString text = file.readAll();
            phpParser parser;
            ASTRoot root = parser.parse(text);
            if (!root) {
                printf("error: %s\n", path.toAscii().constData());
                err++;
            }
//            printf("textwas \n\n===\n%s\n===\n\n", text.toAscii().constData());
        }
        cnt++;
    }
    printf("summary: %i of %i failed\n", err, cnt);
    return fileSource.getRoot();
}

QVector<QString> PhpTreeSource::getPhpFileList()
{
    QVector<IonProject::TreeItem*> parents;
    QVector<QString> phpFiles;
    parents.push_back(fileSource.getRoot());
    while (!parents.empty()) {
        IonProject::TreeItem *parent = parents.back();
        parents.pop_back();
        foreach (IonProject::TreeItem *child, parent->getChildren()) {
            parents.push_back(child);
            QString path = child->getPath();
            if (path.toLower().endsWith(".php")) {
                phpFiles.push_back(path);
            }
        }
    }
    return phpFiles;
}

}

#include "phptreemodel.h"
#include <QFile>
#include "phpparser.h"
#include <iostream>

namespace IonPhp {

IonProject::TreeBranch *PhpTreeSource::setupData()
{
    IonProject::TreeBranch* root = treeItemFactory.createTreeBranch("Name", "", NULL);
    int cnt=0, err=0;
    foreach (QString path, getPhpFileList()) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString text = file.readAll();
            phpParser parser;
            ASTRoot astRoot = parser.parse(text);
            if (!astRoot) {
                printf("error: %s\n", path.toAscii().constData());
                err++;
            } else {
                foreach(pASTNode classDecl, astRoot->findChildren("class_declaration")) {
                    try {
                        IonProject::TreeBranch* classNode = treeItemFactory.createTreeBranch(classDecl->getChild(1)->getStrData("text"), path, root);
                        root->appendChild(classNode);
                        foreach(pASTNode classStatementList, classDecl->findChildren("class_statement_list")) {
                            foreach(pASTNode methodDecl, classStatementList->findChildren("METHOD")) {
                                classNode->appendChild(treeItemFactory.createTreeItem(methodDecl->getChild(2)->getStrData("text"), path, classNode));
                            }
                        }
                    } catch (std::exception &e) {
                        std::cerr << e.what() << std::endl;
                    }
                }
            }
        }
        cnt++;
    }
    printf("summary: %i of %i failed\n", err, cnt);
    return root;
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

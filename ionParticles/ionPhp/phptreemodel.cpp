#include "phptreemodel.h"
#include <iostream>
#include <QFile>

namespace IonPhp {

IonProject::TreeBranch *PhpTreeSource::setupData()
{
    IonProject::TreeBranch* root = treeItemFactory.createTreeBranch("Name", "", NULL);
    foreach (QString path, getPhpFileList()) {
        try {
            addFileToTree(path, root);
        } catch (std::runtime_error &err) {
            errors << err.what();
        }
    }
    return root;
}

void PhpTreeSource::addFileToTree(QString path, IonProject::TreeBranch* root)
{
    ASTRoot astRoot = phpParser().parseFile(path);
    foreach(pASTNode classDecl, astRoot->findChildren("class_declaration")) {
        try {
            IonProject::TreeBranch* classNode = treeItemFactory.createTreeBranch(classDecl->getChild(1)->getStrData("text"), path, root);
            root->appendChild(classNode);
            foreach(pASTNode classStatementList, classDecl->findChildren("class_statement_list")) {
                foreach(pASTNode methodDecl, classStatementList->findChildren("METHOD")) {
                    classNode->appendChild(treeItemFactory.createTreeItem(methodDecl->getChild(2)->getStrData("text"), path, classNode));
                }
            }
        } catch (std::exception &err) {
            errors << err.what();
        }
    }
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

#include "phptreemodel.h"
#include <iostream>
#include <QFile>

namespace IonPhp {

IonProject::TreeBranch *PhpTreeSource::setupData()
{
    IonProject::TreeBranch* root = treeItemFactory.createTreeBranch("Name", "", -1, NULL);
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
            IonPhp::pASTNode classDeclLabel = classDecl->getChild(1);
            IonProject::TreeBranch* classNode = treeItemFactory.createTreeBranch(classDeclLabel->getStrData("text"), path, classDeclLabel->getLine(), root);
            root->appendChild(classNode);
            foreach(pASTNode classStatementList, classDecl->findChildren("class_statement_list")) {
                foreach(pASTNode methodDecl, classStatementList->findChildren("METHOD")) {
                    IonPhp::pASTNode methodDeclLabel = methodDecl->getChild(2);
                    classNode->appendChild(treeItemFactory.createTreeItem(methodDeclLabel->getStrData("text"), path, methodDeclLabel->getLine(), classNode));
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

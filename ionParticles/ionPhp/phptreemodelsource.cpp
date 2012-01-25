/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "phptreemodelsource.h"
#include <iostream>
#include <QFile>
#include <QSqlQuery>
#include <ionHeart/shared.h>

#include "phpparser.h"

namespace IonPhp {
namespace Private {

IonProject::TreeItem *PhpTreeModelSourceDecorator::setupData()
{
    IonProject::TreeItem* root = origTreeModelSource->setupData();

    QVector<IonProject::TreeItem*> parents;
    parents.push_back(root);
    while (!parents.empty()) {
        IonProject::TreeItem *parent = parents.back();
        parents.pop_back();
        foreach (IonProject::TreeItem *child, parent->getChildren()) {
            parents.push_back(child);
        }
        decorateNode(parent);
    }
    return root;
}

void PhpTreeModelSourceDecorator::decorateNode(IonProject::TreeItem *node)
{
    QString path = node->getPath();
    if (path.toLower().endsWith(".php")) {
        addPhpFileInfo(node, path);
    }
}

void PhpTreeModelSourceDecorator::addPhpFileInfo(IonProject::TreeItem *node, QString path)
{
    QSharedPointer<ASTRoot> fileInfo;
    try {
        DEBUG_MSG("adding "<<path.toStdString());
        fileInfo = phpParser().parseFile(path);
    } catch (std::exception &err) {
        DEBUG_MSG(err.what());
        //TODO: mark node red
        return;
    }


    try {
        storage.beginTransaction();
        int fileId = storage.addFile(path, *fileInfo);
        QSharedPointer<QSqlQuery> q = storage.getFileClasses(fileId);
        while (q->next()) {
            QString className = q->value(1).toString();
            IonProject::TreeItem* classNode = treeItemFactory->createTreeItem(className, className, q->value(2).toString(), q->value(3).toInt(), node);
            node->appendChild(classNode);

            QSharedPointer<QSqlQuery> qm = storage.getClassMethods(q->value(0).toInt());
            while (qm->next()) {
                QString methodName = qm->value(1).toString();
                classNode->appendChild(treeItemFactory->createTreeItem(methodName, className+"::"+methodName, qm->value(2).toString(), qm->value(3).toInt(), classNode));
            }
        }
        storage.commitTransaction();
    } catch (std::exception &err) {
        DEBUG_MSG(err.what());
        storage.rollbackTransaction();
        //TODO: mark node red
    }
}




}
}

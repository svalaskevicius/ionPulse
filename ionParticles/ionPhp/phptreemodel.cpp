/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "phptreemodel.h"
#include <iostream>
#include <QFile>
#include <QSqlQuery>
#include <QDebug>

namespace IonPhp {

IonProject::TreeBranch *PhpTreeSource::setupData()
{
    IonProject::TreeBranch* root = treeItemFactory->createTreeBranch("Name", "", -1, NULL);
    QSharedPointer<QSqlQuery> q = storage.getClasses();
    qDebug() << "setp1 ";
    while (q->next()) {
        QString className = q->value(1).toString();
        IonProject::TreeBranch* classNode = treeItemFactory->createTreeBranch(className, q->value(2).toString(), q->value(3).toInt(), root);
        root->appendChild(classNode);

        QSharedPointer<QSqlQuery> qm = storage.getMethods(q->value(0).toInt());
        while (qm->next()) {
            QString methodName = qm->value(1).toString();
            classNode->appendChild(treeItemFactory->createTreeItem(methodName, className+"::"+methodName, qm->value(2).toString(), qm->value(3).toInt(), classNode));
        }

    }
    return root;
}


}

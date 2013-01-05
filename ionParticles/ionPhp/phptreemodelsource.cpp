/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include "phptreemodelsource.h"
#include <iostream>
#include <QFile>
#include <ionCore/shared.h>

#include "phpparser.h"

#include <QFileInfo>
#include <QDateTime>

namespace IonPhp {
namespace Private {

IonProject::TreeItem *PhpTreeModelSourceDecorator::setupData(QString pathFilter)
{
    IonProject::TreeItem* root = origTreeModelSource->setupData(pathFilter);

    QVector<IonProject::TreeItem*> parents;
    parents.push_back(root);
    while (!parents.empty()) {
        IonProject::TreeItem *node = parents.back();
        parents.pop_back();
        if (node->getItemClass() == TREESOURCE_CLASS_DIR) {
            foreach (IonProject::TreeItem *child, node->getChildren()) {
                if ((pathFilter == "")
                        || child->getPath().startsWith(pathFilter)
                        || pathFilter.startsWith(child->getPath())
                    ) {
                    parents.push_back(child);
                }
            }
        } else if (node->getItemClass() == TREESOURCE_CLASS_FILE) {
            if ((pathFilter == "") || node->getPath().startsWith(pathFilter)) {
                decorateNode(node);
            }
        }
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
    try {
        if (node->getChildren().count()) {
            node->clearChildren();
        }

        updateFile(path);

        QScopedPointer<IonDbXml::DataQueryResults> q(storage.getFileClasses(path));
        while (q->next()) {
            QString className = q->value()->getFirstChild()->getFirstChild()->toString();
            IonProject::TreeItem* classNode = treeItemFactory->createTreeItem("phpclass", className, className, path, q->value()->getLastChild()->getFirstChild()->toNumber(), node);
            node->appendChild(classNode);

            QScopedPointer<IonDbXml::DataQueryResults> qm(storage.getClassMethods(path, className));
            while (qm->next()) {
                QString methodName = qm->value()->getFirstChild()->getFirstChild()->toString();
                classNode->appendChild(treeItemFactory->createTreeItem("phpmethod", methodName, className+"::"+methodName, path, qm->value()->getLastChild()->getFirstChild()->toNumber(), classNode));
            }
        }
    } catch (std::exception &err) {
        DEBUG_MSG(err.what());
        //TODO: mark node red
    } catch (...) {
        DEBUG_MSG("cought unknown exception.");
    }
}

void PhpTreeModelSourceDecorator::updateFile(QString path)
{
    uint mtimeStored = storage.getTimeStamp(path);
    QFileInfo fileInfo(path);
    if (fileInfo.lastModified().toTime_t() > mtimeStored) {
        if (mtimeStored) {
            storage.removeFile(path);
        }
        QSharedPointer<ParserResult> parseInfo = QSharedPointer<ParserResult>(PhpParser().parseFile(path));
        if (!parseInfo->success) {
            throw std::runtime_error(parseInfo->error.message.toStdString());
        }
        storage.addFile(path, fileInfo.lastModified().toTime_t(), parseInfo->getRoot());
    }
}





}
}

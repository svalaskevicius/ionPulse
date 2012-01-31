/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef ProjectTreeTest_H
#define ProjectTreeTest_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <iostream>

#include <ionParticles/ionProject/projectapi.h>
#include <ionParticles/ionProject/treeitem.h>
#include <ionParticles/ionProject/treemodeladapter.h>
#include <ionParticles/ionProject/directorytreesource.h>

using namespace IonProject;
using namespace IonProject::Private;

namespace IonTests {

class MockTreeSource : public TreeModelSource {
public:
    TreeItem* root;
    MockTreeSource() {
        root = new TreeItemImpl("test", "name", "", "path", -1, NULL);

        TreeItem* level1 = new TreeItemImpl("test", "dir1", "path1", "path1", -1, root);
        root->appendChild(level1);

        level1->appendChild(new TreeItemImpl("test", "fileName1_1", "path11/fileName11", "path1/fileName11", -1, level1));
        level1->appendChild(new TreeItemImpl("test", "fileName1_2", "path12/fileName12", "path1/fileName12", -1, level1));

        root->appendChild(new TreeItemImpl("test", "fileName2", "fileName2", "fileName2", -1, root));
    }
    virtual QString getTitle() const {return "test";}
    virtual TreeItem * setupData() {
        return root;
    }
};


class ProjectDirectoryTreeSourceTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_if_getTitleIsForDirectoryTree() {
        QString path;
        DirectoryTreeSource ds(path);

        QCOMPARE(ds.getTitle(), QString("Project Browser"));
    }
};


class ProjectTreeItemTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_if_treeItem_getChildren_returnsEmptyList() {
        TreeItemFactoryImpl factory;
        TreeItem* item = factory.createTreeItem("test", "", "", "", -1, NULL);
        QList<TreeItem*> list = item->getChildren();
        delete item;

        QCOMPARE(list.size(), 0);
    }
    void test_if_TreeItem_getChildren_returnsEmptyListIfThereAreNoChildren() {
        TreeItemFactoryImpl factory;
        TreeItem* item = factory.createTreeItem("test", "", "", "", -1, NULL);
        QList<TreeItem*> list = item->getChildren();
        delete item;

        QCOMPARE(list.size(), 0);
    }
    void test_if_TreeItem_getChildren_returnsEmptyListIfThereAreSomeChildren() {
        TreeItemFactoryImpl factory;
        TreeItem* item = factory.createTreeItem("test", "", "", "", -1, NULL);
        TreeItem* item2 = factory.createTreeItem("test", "", "", "", -1, item);
        item->appendChild(item2);
        QList<TreeItem*> list = item->getChildren();
        delete item;

        QCOMPARE(list.size(), 1);
        QCOMPARE(list.front(), item2); // note, item2 is already deleted, compare only the address
    }
    void test_if_treeItem_is_filtered_by_filterBy_param() {
        TreeItemFactoryImpl factory;
        QScopedPointer<TreeItem> item(factory.createTreeItem("test", "", "aaa", "", -1, NULL));
        item->filter("baa");
        QCOMPARE(item->isVisible(), 0);
        item->filter("aaa");
        QCOMPARE(item->isVisible(), 1);
    }
};


class ProjectTreeModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_dataDimensions() {
        QSharedPointer<TreeModelSource> source(new MockTreeSource());
        TreeModelAdapter model(source);

        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.rowCount(model.index(0, 0)), 2);
        QCOMPARE(model.columnCount(), 1);
    }

    void test_structureBetweenParentAndChild() {
        QSharedPointer<TreeModelSource> source(new MockTreeSource());
        TreeModelAdapter model(source);

        QCOMPARE(model.parent(model.index(0, 0, model.index(0, 0))), model.index(0, 0));
    }

    void test_dataContainsCorrectFixtures() {
        QSharedPointer<TreeModelSource> source(new MockTreeSource());
        TreeModelAdapter model(source);

        QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toString(), QString("dir1"));
        QCOMPARE(model.data(model.index(1, 0), Qt::DisplayRole).toString(), QString("fileName2"));
        QCOMPARE(model.data(model.index(0, 0, model.index(0, 0)), Qt::DisplayRole).toString(), QString("fileName1_1"));
    }

    void test_if_dataIsReducedByFilter() {
        QSharedPointer<TreeModelSource> source(new MockTreeSource());
        TreeModelAdapter model(source);

        model.filter("2");

        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.rowCount(model.index(1, 0)), 0);
        QCOMPARE(model.columnCount(), 1);

        model.filter("1");

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.rowCount(model.index(0, 0)), 2);
        QCOMPARE(model.columnCount(), 1);
    }

    void test_if_filterWorksForPathBasis() {
        QSharedPointer<TreeModelSource> source(new MockTreeSource());
        TreeModelAdapter model(source);

        model.filter("11/f");

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.rowCount(model.index(0, 0)), 0);

        QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toString(), QString("fileName1_1"));
    }

    void test_if_filterChecksFilterByFirst() {
        QSharedPointer<MockTreeSource> source(new MockTreeSource());

        TreeItem* level1 = new TreeItemImpl("test", "dir3", "path3", "path1", -1, source->root);
        source->root->appendChild(level1);
        level1->appendChild(new TreeItemImpl("test", "fileName3", "fileName1", "fileName1", -1, level1));

        TreeModelAdapter model(source);

        model.filter("path3");

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.rowCount(model.index(0, 0)), 0);
        QCOMPARE(model.columnCount(), 1);

        QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toString(), QString("fileName3"));
    }

    void test_if_filterChecksChildItems() {
        QSharedPointer<MockTreeSource> source(new MockTreeSource());

        TreeItem* level1 = new TreeItemImpl("test", "dir3", "path3", "path1", -1, source->root);
        source->root->appendChild(level1);
        level1->appendChild(new TreeItemImpl("test", "fileName3", "fileName3", "fileName1", -1, level1));

        TreeModelAdapter model(source);

        model.filter("fileName3");

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.rowCount(model.index(0, 0)), 0);
        QCOMPARE(model.columnCount(), 1);

        QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toString(), QString("fileName3"));
    }

    void test_getPath() {
        QSharedPointer<MockTreeSource> source(new MockTreeSource());
        TreeModelAdapter model(source);

        model.filter("11/f");

        QCOMPARE(model.getItem(model.index(0, 0))->getPath(), QString("path1/fileName11"));
    }

    void test_if_getTitleReturnsSourceTitle() {
        QSharedPointer<MockTreeSource> source(new MockTreeSource());
        TreeModelAdapter model(source);

        QCOMPARE(model.getTitle(), QString("test"));
    }

    void test_if_getRootReturnsSourceCreatedRoot() {
        QSharedPointer<MockTreeSource> source(new MockTreeSource());
        TreeModelAdapter model(source);

        TreeItem *root = model.getRoot();
        QCOMPARE(root->data(0).toString(), QString("name"));
    }

    void test_if_getRangeItems_returnsVectorOfSkippedTreeItems()
    {
        QSharedPointer<MockTreeSource> source(new MockTreeSource());
        TreeModelAdapter model(source);

        model.filter("11/f");

        QVector<TreeItem*> ret= model.getRangeItems(model.index(0, 0));
        QCOMPARE(ret.size(), 2);

        QCOMPARE(ret[0]->data(0).toString(), QString("dir1"));
        QCOMPARE(ret[1]->data(0).toString(), QString("fileName1_1"));
    }
};

}

#endif // ProjectTreeTest_H

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

#include <boost/fusion/algorithm/iteration/accumulate.hpp>
#include <boost/fusion/include/accumulate.hpp>

#include "gmock/gmock.h"

using namespace IonProject;
using namespace IonProject::Private;

namespace IonTests {

class MockedDirectoryTreeSourceDirInfo : public IonProject::Private::DirectoryTreeSource::DirectoryInfo {
public:
    MockedDirectoryTreeSourceDirInfo() : IonProject::Private::DirectoryTreeSource::DirectoryInfo("") {}
    MOCK_METHOD0(dirnames, QStringList ());
    MOCK_METHOD0(filenames, QStringList ());
    MOCK_METHOD0(absolutePath, QString ());
};
class MockedDirectoryTreeSource : public IonProject::Private::DirectoryTreeSource {
public:
    MockedDirectoryTreeSource(QString path) : IonProject::Private::DirectoryTreeSource(path) {}
    MOCK_METHOD1(_getDir, QSharedPointer<IonProject::Private::DirectoryTreeSource::DirectoryInfo> (const QString));
};

}
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
    virtual TreeItem * setupData(QString filter = "") {
        return root;
    }
};

struct TreeItemPrinter {
    QString operator()(TreeItem *item, int indent=0) {
        QString ret = "";
        for (int i = indent; i>0; i--) {
            ret += "    ";
        }
        ret += item->data(0).toString();
        ret += "\n";
        foreach (TreeItem *child, item->getChildren()) {
            ret += (*this)(child, indent+1);
        }
        return ret;
    }
};
struct TreeItemAddressPrinter {
    QString operator()(TreeItem *item, int indent=0) {
        QString ret = "";
        for (int i = indent; i>0; i--) {
            ret += "    ";
        }
        ret += QString("0x%1").arg((unsigned long)item);
        ret += "\n";
        foreach (TreeItem *child, item->getChildren()) {
            ret += (*this)(child, indent+1);
        }
        return ret;
    }
};
struct TreeItemAddressCollector {
    QSet<TreeItem *> operator()(TreeItem *item) {
        QSet<TreeItem *> ret;
        return this->operator ()(item, ret);
    }
    QSet<TreeItem *> operator()(TreeItem *item, QSet<TreeItem *> &ret) {
        ret +=(TreeItem *)item;
        foreach (TreeItem *child, item->getChildren()) {
            (*this)(child, ret);
        }
        return ret;
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
    void test_if_setupData_createsDirTree() {
        MockedDirectoryTreeSourceDirInfo *pRoot = new MockedDirectoryTreeSourceDirInfo();
        EXPECT_CALL(*pRoot, dirnames()).WillOnce(testing::Return(QStringList("l1dir1")));
        EXPECT_CALL(*pRoot, filenames()).WillOnce(testing::Return(QStringList() << "l1file1" << "l1file2"));
        EXPECT_CALL(*pRoot, absolutePath()).Times(testing::AnyNumber());
        ON_CALL(*pRoot, absolutePath()).WillByDefault(testing::Return("root"));
        QSharedPointer<IonProject::Private::DirectoryTreeSource::DirectoryInfo> root(pRoot);

        QSharedPointer<IonProject::Private::DirectoryTreeSource::DirectoryInfo> l1(new MockedDirectoryTreeSourceDirInfo());
        EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), dirnames()).WillOnce(testing::Return(QStringList()));
        EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), filenames()).WillOnce(testing::Return(QStringList() << "l2file1" << "l2file2"));
        EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), absolutePath()).Times(testing::AnyNumber());
        ON_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), absolutePath()).WillByDefault(testing::Return("root/l1dir1"));

        MockedDirectoryTreeSource source("root/");
        EXPECT_CALL(source, _getDir(testing::Eq("root/"))).WillOnce(testing::Return(root));
        EXPECT_CALL(source, _getDir(testing::Eq("root/l1dir1/"))).WillOnce(testing::Return(l1));

        QSharedPointer<TreeItem> rootTreeItem(source.setupData());

        TreeItemPrinter printer;

        QCOMPARE(
            printer(rootTreeItem.data()),
            QString(
                "Name\n"
                "    l1dir1\n"
                "        l2file1\n"
                "        l2file2\n"
                "    l1file1\n"
                "    l1file2\n"
            )
        );
    }

    void test_if_setupData_updatesDirTreeAndKeepsSameNodes() {
        MockedDirectoryTreeSourceDirInfo *pRoot = new MockedDirectoryTreeSourceDirInfo();
        EXPECT_CALL(*pRoot, dirnames()).WillRepeatedly(testing::Return(QStringList("l1dir1")));
        EXPECT_CALL(*pRoot, filenames()).WillRepeatedly(testing::Return(QStringList() << "l1file1" << "l1file2"));
        EXPECT_CALL(*pRoot, absolutePath()).Times(testing::AnyNumber());
        ON_CALL(*pRoot, absolutePath()).WillByDefault(testing::Return("root"));
        QSharedPointer<IonProject::Private::DirectoryTreeSource::DirectoryInfo> root(pRoot);

        QSharedPointer<IonProject::Private::DirectoryTreeSource::DirectoryInfo> l1(new MockedDirectoryTreeSourceDirInfo());
        EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), dirnames()).WillRepeatedly(testing::Return(QStringList()));
        EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), filenames()).WillRepeatedly(testing::Return(QStringList() << "l2file1" << "l2file2"));
        EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), absolutePath()).Times(testing::AnyNumber());
        ON_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), absolutePath()).WillByDefault(testing::Return("root/l1dir1"));

        MockedDirectoryTreeSource source("root/");
        EXPECT_CALL(source, _getDir(testing::Eq("root/"))).WillRepeatedly(testing::Return(root));
        EXPECT_CALL(source, _getDir(testing::Eq("root/l1dir1/"))).WillRepeatedly(testing::Return(l1));

        QSharedPointer<TreeItem> rootTreeItem(source.setupData());

        TreeItemAddressPrinter printer;
        QString addresses = printer(rootTreeItem.data());

        QCOMPARE(printer(source.setupData()), addresses);
    }

    void test_if_setupData_updatesDirTreeAndAddsNewNodes() {
        testing::ExpectationSet init1;

        MockedDirectoryTreeSourceDirInfo *pRoot = new MockedDirectoryTreeSourceDirInfo();
        EXPECT_CALL(*pRoot, dirnames()).WillRepeatedly(testing::Return(QStringList("l1dir1")));
        init1 += EXPECT_CALL(*pRoot, filenames()).WillOnce(testing::Return(QStringList() << "l1file1" << "l1file2"));
        EXPECT_CALL(*pRoot, absolutePath()).Times(testing::AnyNumber());
        ON_CALL(*pRoot, absolutePath()).WillByDefault(testing::Return("root"));
        QSharedPointer<IonProject::Private::DirectoryTreeSource::DirectoryInfo> root(pRoot);

        QSharedPointer<IonProject::Private::DirectoryTreeSource::DirectoryInfo> l1(new MockedDirectoryTreeSourceDirInfo());
        EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), dirnames()).WillRepeatedly(testing::Return(QStringList()));
        init1 += EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), filenames()).WillOnce(testing::Return(QStringList() << "l2file1" << "l2file2"));
        EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), absolutePath()).Times(testing::AnyNumber());
        ON_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), absolutePath()).WillByDefault(testing::Return("root/l1dir1"));

        EXPECT_CALL(*pRoot, filenames()).After(init1).WillOnce(testing::Return(QStringList() << "l1file1" << "l1file2" << "l1file3"));
        EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), filenames()).After(init1).WillOnce(testing::Return(QStringList() << "l2file1" << "l2file2" << "l2file3"));

        MockedDirectoryTreeSource source("root/");
        EXPECT_CALL(source, _getDir(testing::Eq("root/"))).WillRepeatedly(testing::Return(root));
        EXPECT_CALL(source, _getDir(testing::Eq("root/l1dir1/"))).WillRepeatedly(testing::Return(l1));

        QSharedPointer<TreeItem> rootTreeItem(source.setupData());

        TreeItemAddressCollector collector;
        QSet<TreeItem *> addresses1 = collector(rootTreeItem.data());
        QSet<TreeItem *> addresses2 = collector(source.setupData());

        QSet<TreeItem *> diff = addresses2 - addresses1;
        QCOMPARE(diff.size(), 2);
        QSet<QString> paths;
        foreach (TreeItem *item, diff) {
            paths += item->getPath();
        }
        QVERIFY(paths.contains("root/l1file3"));
        QVERIFY(paths.contains("root/l1dir1/l2file3"));
    }

    void test_if_setupData_updatesDirTreeAndRemovesGoneNodes() {
        testing::ExpectationSet init1;

        MockedDirectoryTreeSourceDirInfo *pRoot = new MockedDirectoryTreeSourceDirInfo();
        EXPECT_CALL(*pRoot, dirnames()).WillRepeatedly(testing::Return(QStringList("l1dir1")));
        init1 += EXPECT_CALL(*pRoot, filenames()).WillOnce(testing::Return(QStringList() << "l1file1" << "l1file2"));
        EXPECT_CALL(*pRoot, absolutePath()).Times(testing::AnyNumber());
        ON_CALL(*pRoot, absolutePath()).WillByDefault(testing::Return("root"));
        QSharedPointer<IonProject::Private::DirectoryTreeSource::DirectoryInfo> root(pRoot);

        QSharedPointer<IonProject::Private::DirectoryTreeSource::DirectoryInfo> l1(new MockedDirectoryTreeSourceDirInfo());
        EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), dirnames()).WillRepeatedly(testing::Return(QStringList()));
        init1 += EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), filenames()).WillOnce(testing::Return(QStringList() << "l2file1" << "l2file2"));
        EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), absolutePath()).Times(testing::AnyNumber());
        ON_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), absolutePath()).WillByDefault(testing::Return("root/l1dir1"));

        EXPECT_CALL(*pRoot, filenames()).After(init1).WillOnce(testing::Return(QStringList() << "l1file1"));
        EXPECT_CALL((MockedDirectoryTreeSourceDirInfo &)(*l1), filenames()).After(init1).WillOnce(testing::Return(QStringList() << "l2file1"));

        MockedDirectoryTreeSource source("root/");
        EXPECT_CALL(source, _getDir(testing::Eq("root/"))).WillRepeatedly(testing::Return(root));
        EXPECT_CALL(source, _getDir(testing::Eq("root/l1dir1/"))).WillRepeatedly(testing::Return(l1));

        QSharedPointer<TreeItem> rootTreeItem(source.setupData());

        TreeItemAddressCollector collector;
        QSet<TreeItem *> addresses1 = collector(rootTreeItem.data());
        QMap<TreeItem *, QString> pathMap;
        foreach (TreeItem *item, addresses1) {
            pathMap[item] = item->getPath();
        }
        QSet<TreeItem *> addresses2 = collector(source.setupData());

        QSet<TreeItem *> diff = addresses1 - addresses2;
        QCOMPARE(diff.size(), 2);
        QSet<QString> paths;
        foreach (TreeItem *item, diff) {
            paths += pathMap[item];
        }
        QVERIFY(paths.contains("root/l1file2"));
        QVERIFY(paths.contains("root/l1dir1/l2file2"));
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

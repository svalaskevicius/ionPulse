#ifndef PHPCLASSBROWSERTEST_H
#define PHPCLASSBROWSERTEST_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <iostream>

#include <ionParticles/ionPhp/phptreemodel.h>

#include "gmock/gmock.h"

namespace IonTests {

using namespace IonPhp;

class MockedTreeModel : public IonProject::TreeModel {
public:
    MOCK_METHOD1(filter, void (QString filter));
    MOCK_CONST_METHOD1(getItem, IonProject::TreeItem * (const QModelIndex &index));
    MOCK_CONST_METHOD0(getTitle, QString ());
    MOCK_CONST_METHOD0(getRoot, IonProject::TreeBranch * ());

    MOCK_CONST_METHOD3(index, QModelIndex (int, int, const QModelIndex &));
    MOCK_CONST_METHOD1(parent, QModelIndex (const QModelIndex &));
    MOCK_CONST_METHOD1(rowCount, int (const QModelIndex &));
    MOCK_CONST_METHOD1(columnCount, int (const QModelIndex &));
    MOCK_CONST_METHOD2(data, QVariant (const QModelIndex &, int));
};
class MockedTreeItemFactory : public IonProject::TreeItemFactory {
public:
    MOCK_METHOD4(createTreeItem, IonProject::TreeItem * (QString, QString, int const line, IonProject::TreeBranch*));
    MOCK_METHOD4(createTreeBranch, IonProject::TreeBranch * (QString, QString, int const line, IonProject::TreeBranch*));
};
class MockedTreeItem : public IonProject::TreeItem {
public:
    MOCK_CONST_METHOD1(data, QVariant (int column));
    MOCK_METHOD0(getRowNr, int ());
    MOCK_METHOD0(parent, IonProject::TreeBranch * ());
    MOCK_CONST_METHOD0(childrenCount, int ());
    MOCK_CONST_METHOD0(getChildren, QList<IonProject::TreeItem*> ());
    MOCK_CONST_METHOD0(isVisible, bool ());
    MOCK_CONST_METHOD0(getPath, QString ());
    MOCK_CONST_METHOD0(getLine, int ());
    MOCK_METHOD1(filter, void(QString));
};
class MockedTreeBranch : public IonProject::TreeBranch {
public:
    MOCK_CONST_METHOD1(data, QVariant (int column));
    MOCK_METHOD0(getRowNr, int ());
    MOCK_METHOD0(parent, IonProject::TreeBranch * ());
    MOCK_CONST_METHOD0(childrenCount, int ());
    MOCK_CONST_METHOD0(getChildren, QList<IonProject::TreeItem*> ());
    MOCK_CONST_METHOD0(isVisible, bool ());
    MOCK_CONST_METHOD0(getPath, QString ());
    MOCK_CONST_METHOD0(getLine, int ());
    MOCK_METHOD1(filter, void(QString));

    MOCK_METHOD1(appendChild, void(IonProject::TreeItem*));
    MOCK_METHOD1(getChild, IonProject::TreeItem*(int));
    MOCK_METHOD1(getChildRowNr, int(IonProject::TreeItem*));
};


class PhpTreeModelSourceTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_if_getTitleIsForClassTree()
    {
        MockedTreeModel fileSource;
        MockedTreeItemFactory treeItemFactory;
        PhpTreeSource ps(fileSource, treeItemFactory);
        QCOMPARE(ps.getTitle(), QString("Class Browser"));
    }
    void test_if_setupDataFiltersPhpFiles()
    {
        MockedTreeModel fileSource;
        MockedTreeItemFactory treeItemFactory;
        MockedTreeBranch rootMock;

        MockedTreeBranch fileMock;
        MockedTreeItem phpFileMock1;
        MockedTreeItem phpFileMock2;

        EXPECT_CALL(treeItemFactory, createTreeBranch(::testing::_, ::testing::_, ::testing::_, ::testing::_))
            .WillOnce(::testing::Return((&rootMock)));
        EXPECT_CALL(fileSource, getRoot()).WillOnce(::testing::Return((&fileMock)));
        EXPECT_CALL(fileMock, getChildren()).WillOnce(::testing::Return((QList<IonProject::TreeItem*>() << &phpFileMock1 << &phpFileMock2)));
        EXPECT_CALL(phpFileMock1, getChildren()).WillOnce(::testing::Return((QList<IonProject::TreeItem*>())));
        EXPECT_CALL(phpFileMock1, getPath()).WillOnce(::testing::Return(QString("ad.php")));
        EXPECT_CALL(phpFileMock2, getChildren()).WillOnce(::testing::Return((QList<IonProject::TreeItem*>())));
        EXPECT_CALL(phpFileMock2, getPath()).WillOnce(::testing::Return(QString("adnotphp")));

        PhpTreeSource ps(fileSource, treeItemFactory);

        IonProject::TreeBranch *root(ps.setupData());

        QCOMPARE(ps.getErrors().size(), 1);
        QCOMPARE(ps.getErrors().front(), QString("parsing of the file failed: file not found: ad.php"));

        QCOMPARE(root, &rootMock);

    }
};

}

#endif // PHPCLASSBROWSERTEST_H

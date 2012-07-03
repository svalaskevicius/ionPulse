#ifndef PHPCLASSBROWSERTEST_H
#define PHPCLASSBROWSERTEST_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <iostream>

#include <ionParticles/ionPhp/phptreemodelsource.h>
#include <ionParticles/ionProject/treemodeladapter.h>

#include "gmock/gmock.h"

namespace IonTests {

class MockedTreeModel : public IonProject::Private::TreeModelAdapter {
public:
    MOCK_METHOD1(filter, void (QString filter));
    MOCK_CONST_METHOD1(getItem, IonProject::TreeItem * (const QModelIndex &index));
    MOCK_CONST_METHOD0(getTitle, QString ());
    MOCK_CONST_METHOD0(getRoot, IonProject::TreeItem * ());

    MOCK_CONST_METHOD3(index, QModelIndex (int, int, const QModelIndex &));
    MOCK_CONST_METHOD1(parent, QModelIndex (const QModelIndex &));
    MOCK_CONST_METHOD1(rowCount, int (const QModelIndex &));
    MOCK_CONST_METHOD1(columnCount, int (const QModelIndex &));
    MOCK_CONST_METHOD2(data, QVariant (const QModelIndex &, int));
};

}
namespace IonTests {

class MockedTreeItemFactory : public IonProject::TreeItemFactory {
public:
    MOCK_METHOD5(createTreeItem, IonProject::TreeItem * (QString, QString, QString, int const line, IonProject::TreeItem*));
    MOCK_METHOD4(createTreeItem, IonProject::TreeItem * (QString, QString, int const line, IonProject::TreeItem*));
};

}

namespace IonTests {

class MockedTreeItem : public IonProject::TreeItem {
public:
    MOCK_CONST_METHOD1(data, QVariant (int column));
    MOCK_METHOD0(getRowNr, int ());
    MOCK_METHOD0(parent, IonProject::TreeItem * ());
    MOCK_CONST_METHOD0(visibleChildrenCount, int ());
    MOCK_CONST_METHOD0(getChildren, const QList<IonProject::TreeItem*> &());
    MOCK_CONST_METHOD0(isVisible, bool ());
    MOCK_CONST_METHOD0(getPath, QString ());
    MOCK_CONST_METHOD0(getLine, int ());
    MOCK_METHOD1(filter, void(QString));

    MOCK_METHOD1(appendChild, void(IonProject::TreeItem*));
    MOCK_METHOD1(getVisibleChildByNr, IonProject::TreeItem*(int));
    MOCK_METHOD1(getChildRowNr, int(IonProject::TreeItem*));
};

}

#endif // PHPCLASSBROWSERTEST_H

#ifndef ProjectTreeTest_H
#define ProjectTreeTest_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <iostream>

#include <ionParticles/ionProject/treeitem.h>
#include <ionParticles/ionProject/treemodel.h>

using namespace IonProject::Private;

class MockTreeSource : public TreeModelSource {
public:
    virtual TreeBranch * setupData() {
        TreeBranch* parent = new TreeBranch("name", "path", NULL);

        TreeBranch* level1 = new TreeBranch("dir1", "path1", parent);
        parent->appendChild(level1);

        level1->appendChild(new TreeItem("fileName1", "path1/fileName1", level1));

        parent->appendChild(new TreeItem("fileName2", "fileName2", parent));

        return parent;
    }
};


class ProjectTreeTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_treeModel_dataDimensions() {
        MockTreeSource source;
        TreeModel model(&source, NULL);

        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.rowCount(model.index(0, 0)), 1);
        QCOMPARE(model.columnCount(), 1);
    }

    void test_treeModel_structureBetweenParentAndChild() {
        MockTreeSource source;
        TreeModel model(&source, NULL);

        QCOMPARE(model.parent(model.index(0, 0, model.index(0, 0))), model.index(0, 0));
    }

    void test_treeModel_dataContainsCorrectFixtures() {
        MockTreeSource source;
        TreeModel model(&source, NULL);

        QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toString(), QString("dir1"));
        QCOMPARE(model.data(model.index(1, 0), Qt::DisplayRole).toString(), QString("fileName2"));
        QCOMPARE(model.data(model.index(0, 0, model.index(0, 0)), Qt::DisplayRole).toString(), QString("fileName1"));
    }

    void test_treeModel_dataIsReducedByFilter() {
        MockTreeSource source;
        TreeModel model(&source, NULL);

        model.filter("2");

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.rowCount(model.index(0, 0)), 0);
        QCOMPARE(model.columnCount(), 1);

        model.filter("1");

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.rowCount(model.index(0, 0)), 1);
        QCOMPARE(model.columnCount(), 1);
    }

    void test_treeModel_filterWorksForPathBasis() {
        MockTreeSource source;
        TreeModel model(&source, NULL);

        model.filter("1/f");

        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.rowCount(model.index(0, 0)), 1);
        QCOMPARE(model.columnCount(), 1);

        QCOMPARE(model.data(model.index(0, 0), Qt::DisplayRole).toString(), QString("dir1"));
        QCOMPARE(model.data(model.index(0, 0, model.index(0, 0)), Qt::DisplayRole).toString(), QString("fileName1"));
    }

    void test_treeModel_getPath() {
        MockTreeSource source;
        TreeModel model(&source, NULL);

        model.filter("1/f");

        QCOMPARE(model.getPath(model.index(0, 0, model.index(0, 0))), QString("path1/fileName1"));
    }
};

#endif // ProjectTreeTest_H

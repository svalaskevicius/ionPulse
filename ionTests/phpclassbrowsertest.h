#ifndef PHPCLASSBROWSERTEST_H
#define PHPCLASSBROWSERTEST_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <iostream>

#include <ionParticles/ionPhp/phptreemodel.h>
#include <mockcpp/mockcpp.hpp>

USING_MOCKCPP_NS

namespace IonTests {

using namespace IonPhp;

class PhpTreeModelSourceTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_if_getTitleIsForClassTree()
    {
        MockObject<IonProject::TreeModel> fileSource;
        MockObject<IonProject::TreeItemFactory> treeItemFactory;
        PhpTreeSource ps(*fileSource, *treeItemFactory);
        QCOMPARE(ps.getTitle(), QString("Class Browser"));
    }
};

}

#endif // PHPCLASSBROWSERTEST_H

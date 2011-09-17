/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#pragma once

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <iostream>

#include <ionParticles/ionLayout/layoutmanager.h>
#include <mockcpp/MockObject.h>
#include <mockcpp/ChainingMockHelper.h>

USING_MOCKCPP_NS
using namespace IonLayout;
using namespace IonLayout::Private;


class LayoutManagerTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_zones_getZoneNameReturnsNameFromZoneDef() {
        ZoneDefinition def;
        def.name = "name of the zone";
        ZoneNodeBranch root;

        ZoneNodeBranch branch(&root, def);
        ZoneNodeLeaf leaf(&root, def);

        QCOMPARE(branch.getZoneName(), QString("name of the zone"));
        QCOMPARE(leaf.getZoneName(), QString("name of the zone"));
    }

    void test_zones_findSubZoneReturnsNullIfNotFound() {
        ZoneNodeBranch root;
        QStringList empty;
        QCOMPARE((ulong)root.findSubZone(empty), (ulong)NULL);
    }

    void test_zones_findSubZoneReturnsDeepestPath() {
        ZoneDefinition def;
        ZoneNodeBranch root;

        def.name = "name br";
        ZoneNodeBranch branch(&root, def);
        def.name = "name lf";
        ZoneNodeLeaf leaf(&root, def);

        root.addSubZone(&branch);
        root.addSubZone(&leaf);
        branch.addSubZone(&leaf);

        QStringList searchPath;
        searchPath << "name br" << "name lf";
        QCOMPARE(root.findSubZone(searchPath), &leaf);

        searchPath.clear();
        searchPath << "name lf";
        QCOMPARE(root.findSubZone(searchPath), &leaf);
    }

    void test_zones_findSubZoneForBranchesLooksForSameNameIfSearchListIsEmpty() {
        ZoneDefinition def;
        ZoneNodeBranch root;

        def.name = "name br";
        ZoneNodeBranch branch(&root, def);
        ZoneNodeLeaf leaf(&root, def);

        root.addSubZone(&branch);
        branch.addSubZone(&leaf);

        QStringList searchPath;
        searchPath << "name br";
        QCOMPARE(root.findSubZone(searchPath), &leaf);
    }

    void test_zones_getSubZoneReturnsFindSubZoneIfSuccess() {
        ZoneDefinition def;
        ZoneNodeBranch root;

        def.name = "name lf";
        ZoneNodeLeaf leaf(&root, def);

        root.addSubZone(&leaf);

        QStringList searchPath;
        searchPath << "name lf";
        QCOMPARE(root.getSubZone(searchPath), &leaf);
    }

    void test_zones_getSubZoneThrowsExceptionOnError() {
        ZoneNodeBranch root;

        QStringList searchPath;
        searchPath << "name lf";
        try {
            root.getSubZone(searchPath);
        } catch (std::runtime_error &e) {
            QCOMPARE(e.what(), "zone 'name lf' not found");
            return;
        }
        QFAIL("exception is not thrown");
    }

    void test_zones_getZoneReturnsDeepestPaths() {
        ZoneDefinition def;
        ZoneNodeBranch root;

        def.name = "zone0";
        ZoneNodeBranch branch(&root, def);
        ZoneNodeLeaf leaf0(&branch, def);
        def.name = "zone1";
        ZoneNodeLeaf leaf1(&branch, def);

        root.addSubZone(&branch);
        branch.addSubZone(&leaf0);
        branch.addSubZone(&leaf1);

        QCOMPARE(root.getZone(""), &root);
        QCOMPARE(root.getZone("zone0"), &leaf0);
        QCOMPARE(root.getZone("zone0/zone0"), &leaf0);
        QCOMPARE(root.getZone("zone0/zone1"), &leaf1);
    }

    void test_zones_getZoneLeafReturnsDeepestLeaf() {
        ZoneDefinition def;
        ZoneNodeBranch root;

        def.name = "zone0";
        ZoneNodeBranch branch(&root, def);
        ZoneNodeLeaf leaf0(&branch, def);
        def.name = "zone1";
        ZoneNodeLeaf leaf1(&branch, def);

        root.addSubZone(&branch);
        branch.addSubZone(&leaf0);
        branch.addSubZone(&leaf1);

        QCOMPARE(branch.getZoneLeaf(), &leaf0);
        QCOMPARE(leaf0.getZoneLeaf(), &leaf0);
        QCOMPARE(leaf1.getZoneLeaf(), &leaf1);
    }

    void test_zones_getZoneAsBranchConvertsLeavesToBranchesIfNecessary() {
        ZoneDefinition def;
        def.parentPath = "parentpath";
        def.sizeWeight = 23;
        def.orientation = Qt::Vertical;
        def.hideIfEmpty = 1;
        ZoneNodeBranch root;

        def.name = "zone0";
        ZoneNodeBranch branch(&root, def);
        ZoneNodeLeaf leaf0(&branch, def);
        def.name = "zone1";
        ZoneNodeLeaf leaf1(&branch, def);

        root.addSubZone(&branch);
        branch.addSubZone(&leaf0);
        branch.addSubZone(&leaf1);

        QCOMPARE(branch.getZoneAsBranch(), &branch);
        ZoneNodeBranch *newBranch = leaf1.getZoneAsBranch();
        const ZoneDefinition &newBrDef = newBranch->getDefinition();
        QCOMPARE(newBrDef.parentPath, QString("parentpath"));
        QCOMPARE(newBrDef.name, QString("zone1"));
        QCOMPARE(newBrDef.orientation, Qt::Vertical);
        QCOMPARE(newBrDef.sizeWeight, 23);

        QCOMPARE(root.getZone("zone0"), &leaf0);
        QCOMPARE(root.getZone("zone0/zone0"), &leaf0);
        QCOMPARE(root.getZone("zone0/zone1"), &leaf1);
        QCOMPARE(root.getZone("zone0/zone1/zone1"), &leaf1);
    }

    void test_zones_showPropagatesToParents() {
        ZoneDefinition def;

        ZoneNodeBranch branch;
        ZoneNodeLeaf leaf(&branch, def);
        branch.getWidget()->hide();

        leaf.show();

        QCOMPARE(branch.getWidget()->isVisible(), 1);
    }

};


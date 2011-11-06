/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <iostream>

#include "phpparsertest.h"
#include "projecttreetest.h"
#include "layouttest.h"
#include "phpclassbrowsertest.h"
#include <gtest/gtest.h>

#define ADD_TEST_CLASS(className) { className tc; ret |= QTest::qExec(&tc, argc, argv); }

using namespace IonTests;

class GoogleTestEventListener : public ::testing::EmptyTestEventListener {
   virtual void OnTestStart(const ::testing::TestInfo& test_info) {
   }

   virtual void OnTestPartResult(const ::testing::TestPartResult& test_part_result) {
        if (test_part_result.failed()) {
            QFAIL(
                QString("mock objects failed with '%1' at %2:%3")
                    .arg(QString(test_part_result.summary()))
                    .arg(test_part_result.file_name())
                    .arg(test_part_result.line_number())
                    .toAscii().constData()
            );
        }
   }

   // Called after a test ends.
   virtual void OnTestEnd(const ::testing::TestInfo& test_info) {
   }
 };

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTEST_DISABLE_KEYPAD_NAVIGATION

    ::testing::InitGoogleTest(&argc, argv);
      // Gets hold of the event listener list.
      ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
      delete listeners.Release(listeners.default_result_printer());
      // Adds a listener to the end.  Google Test takes the ownership.
      listeners.Append(new GoogleTestEventListener);

    int ret = 0;
    ADD_TEST_CLASS(PhpParserTest);
    ADD_TEST_CLASS(PhpTreeModelSourceTest);

    ADD_TEST_CLASS(LayoutManagerTest);

    ADD_TEST_CLASS(ProjectTreeModelTest);
    ADD_TEST_CLASS(ProjectTreeItemTest);
    ADD_TEST_CLASS(ProjectDirectoryTreeSourceTest);


    if (!ret) {
        std::cout << "all tests PASSED\n";
    } else {
        std::cout << "some tests FAILED\n";
    }
    return ret;
}



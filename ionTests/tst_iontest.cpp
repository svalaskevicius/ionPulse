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

#define ADD_TEST_CLASS(className) { className tc; ret |= QTest::qExec(&tc, argc, argv); }

int main(int argc, char *argv[])
{
    int ret = 0;
    ADD_TEST_CLASS(PhpParserTest)
    ADD_TEST_CLASS(ProjectTreeTest)
    return ret;
}

//#include "tst_iontest.moc"

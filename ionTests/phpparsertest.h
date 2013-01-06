/*
  (C) 2011 Sarunas Valaskevicius <rakatan@gmail.com>

  This program is released under the terms of
  GNU Lesser General Public License version 3.0
  available at http://www.gnu.org/licenses/lgpl-3.0.txt
*/

#ifndef PHPPARSERTEST_H
#define PHPPARSERTEST_H

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <iostream>

#include <ionParticles/ionPhp/phpparser.h>

namespace IonTests {

using namespace IonPhp;

#define QCOMPARE_3(actual, expected, actions) \
do {\
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__)) { \
        actions; \
    } \
    return; \
} while (0)

#define PRINT_FOR_CODE(QSTR) std::cout << "        \"" << QSTR.trimmed().replace("\\", "\\\\").replace("\"", "\\\"").replace("\n    ", "\"\n        \"").toStdString() << "\"" << std::endl;
#define PRINT_NEW_TEST(CODE, QSTR) \
    std::cout << "-----------------" << std::endl \
    << "    void "<<__FUNCTION__<<"() { TEST_PHP_PARSER("<<std::endl \
    << "        \""<<QString(CODE).replace("\\", "\\\\").replace("\r", "\\r").replace("\n", "\\n").replace("\"", "\\\"").toStdString()<<"\","<<std::endl \
    << "        \"" << QSTR.trimmed().replace("\\", "\\\\").replace("\"", "\\\"").replace("\n    ", "\"\n        \"").toStdString() << "\"" << std::endl \
    << "    );}" << std::endl \
    << "-----------------" << std::endl;

#define TEST_PHP_PARSER(CODE, ASTSTR) { \
    QSharedPointer<IonPhp::Private::ParserResult> ret = \
        QSharedPointer<IonPhp::Private::ParserResult>( \
            IonPhp::Private::PhpParser().parseString(CODE) \
        ); \
    if (!ret->success || ret->errors.length()) { \
        PRINT_FOR_CODE(ret->getRoot()->toString()); \
        QFAIL(ret->errors.front().message.toLatin1()); \
    } \
    QString actual = ret->getRoot()->toString().replace(" ", "").replace("\n", "").replace("\r", "").replace("<?xmlversion=\"1.0\"?>", ""); \
    QString expected = QString(ASTSTR).replace(" ", "").replace("\n", "").replace("\r", ""); \
    QCOMPARE_3(actual, expected, PRINT_NEW_TEST(CODE, ret->getRoot()->toString())); \
}


class PhpParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_failsOnPhpError() {
        QSharedPointer<IonPhp::Private::ParserResult> ret =
                QSharedPointer<IonPhp::Private::ParserResult>(
                    IonPhp::Private::PhpParser().parseString("<?php asd")
                );
        QCOMPARE(ret->success, false);
    }

    void test_openCloseNoEnd() { TEST_PHP_PARSER(
        "<?php ?><?php ?><?php ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "  <empty statement columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  <empty statement columnNr=\"14\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\"/>"
        "</top_statement_list>"
    );}
    void test_openCloseEnd() { TEST_PHP_PARSER(
        "<?php ?><?php ?><?php ?>",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "  <empty statement columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  <empty statement columnNr=\"14\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  <empty statement columnNr=\"22\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\"/>"
        "</top_statement_list>"
    );}
    void test_inlineHtml() { TEST_PHP_PARSER(
        "<?php ?>asd1<?php ?>asd2",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "  <empty statement columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  <inline_html columnNr=\"8\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">asd1</inline_html>"
        "  <empty statement columnNr=\"18\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  <inline_html columnNr=\"20\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">asd2</inline_html>"
        "</top_statement_list>"
    );}
    void test_scriptOpenTag() { TEST_PHP_PARSER(
        " <?<%<?php ?>asd?%<script language=\"php\">echo $d</script> asd",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"61\" endLineNr=\"1\" lineNr=\"1\">"
        "  <inline_html columnNr=\"0\" endColumnNr=\"5\" endLineNr=\"1\" lineNr=\"1\"> &lt;?&lt;%</inline_html>"
        "  <empty statement columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  <inline_html columnNr=\"13\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">asd?%</inline_html>"
        "  <echo columnNr=\"46\" endColumnNr=\"57\" endLineNr=\"1\" lineNr=\"1\">"
        "    <echo_expr_list columnNr=\"46\" endColumnNr=\"57\" endLineNr=\"1\" lineNr=\"1\">"
        "      <variable columnNr=\"46\" endColumnNr=\"48\" endLineNr=\"1\" lineNr=\"1\">$d</variable>"
        "    </echo_expr_list>"
        "  </echo>"
        "  <inline_html columnNr=\"57\" endColumnNr=\"61\" endLineNr=\"1\" lineNr=\"1\"> asd</inline_html>"
        "</top_statement_list>"
    );}
    void test_scriptOpenTagWOQuotes() { TEST_PHP_PARSER(
        "<?php \n?>asd<script \nlanguage=php>\necho $d\n</script>\n asd",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"4\" endLineNr=\"6\" lineNr=\"1\">"
        "  <empty statement columnNr=\"0\" endColumnNr=\"2\" endLineNr=\"2\" lineNr=\"2\"/>"
        "  <inline_html columnNr=\"2\" endColumnNr=\"5\" endLineNr=\"2\" lineNr=\"2\">asd</inline_html>"
        "  <echo columnNr=\"5\" endColumnNr=\"0\" endLineNr=\"6\" lineNr=\"4\">"
        "    <echo_expr_list columnNr=\"5\" endColumnNr=\"0\" endLineNr=\"6\" lineNr=\"4\">"
        "      <variable columnNr=\"5\" endColumnNr=\"7\" endLineNr=\"4\" lineNr=\"4\">$d</variable>"
        "    </echo_expr_list>"
        "  </echo>"
        "  <inline_html columnNr=\"0\" endColumnNr=\"4\" endLineNr=\"6\" lineNr=\"6\"> asd</inline_html>"
        "</top_statement_list>"
    );}
    void test_scriptOpenTagWrong() { TEST_PHP_PARSER(
        "<?php ?>asd<script language=notphp>echo $d</script> asd",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">"
        "  <empty statement columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  <inline_html columnNr=\"8\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">asd&lt;script language=notphp&gt;echo $d&lt;/script&gt; asd</inline_html>"
        "</top_statement_list>"
    );}
    void test_variableAssignmentFromFncCall() { TEST_PHP_PARSER(
        "<?php $a = moo();",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <function_call columnNr=\"11\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "      <namespace_name columnNr=\"11\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "        <string columnNr=\"11\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">moo</string>"
        "      </namespace_name>"
        "      <function_call_parameter_list columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    </function_call>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_variableAssignmentFromFncCallWithParams() { TEST_PHP_PARSER(
        "<?php $a = moo(1, $s);",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <function_call columnNr=\"11\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "      <namespace_name columnNr=\"11\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "        <string columnNr=\"11\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">moo</string>"
        "      </namespace_name>"
        "      <function_call_parameter_list columnNr=\"15\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "        <lnumber columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "        <variable columnNr=\"18\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">$s</variable>"
        "      </function_call_parameter_list>"
        "    </function_call>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_functionDefinition() { TEST_PHP_PARSER(
        "<?php function myfnc() {}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "  <function_declaration columnNr=\"15\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "    <is_reference columnNr=\"15\" endColumnNr=\"20\" endLineNr=\"1\" is_reference=\"0\" lineNr=\"1\"/>"
        "    <string columnNr=\"15\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">myfnc</string>"
        "    <parameter_list columnNr=\"21\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <inner_statement_list columnNr=\"23\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </function_declaration>"
        "</top_statement_list>"
    );}
    void test_listDefinition() { TEST_PHP_PARSER(
        "<?php list($a, $b) = $c;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"11\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "    <assignment_list columnNr=\"11\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">"
        "      <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "      <variable columnNr=\"15\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "    </assignment_list>"
        "    <variable columnNr=\"21\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">$c</variable>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_assignDefinition() { TEST_PHP_PARSER(
        "<?php $a = $c;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$c</variable>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_assignRefDefinition() { TEST_PHP_PARSER(
        "<?php $a = &$c;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"15\" endLineNr=\"1\" is_reference=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$c</variable>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_assignRefNewClassDefinition() { TEST_PHP_PARSER(
        "<?php $a = & new asd;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"21\" endLineNr=\"1\" is_reference=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <new columnNr=\"17\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "      <namespace_name columnNr=\"17\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "        <string columnNr=\"17\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">asd</string>"
        "      </namespace_name>"
        "      <VOID columnNr=\"20\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    </new>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_assignRefNewClassWithParamsDefinition() { TEST_PHP_PARSER(
        "<?php $a = & new asd(1);",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"23\" endLineNr=\"1\" is_reference=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <new columnNr=\"17\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">"
        "      <namespace_name columnNr=\"17\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "        <string columnNr=\"17\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">asd</string>"
        "      </namespace_name>"
        "      <function_call_parameter_list columnNr=\"21\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">"
        "        <lnumber columnNr=\"21\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "      </function_call_parameter_list>"
        "    </new>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_newClassDefinition() { TEST_PHP_PARSER(
        "<?php new asd(1, '2');",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "  <new columnNr=\"10\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "    <namespace_name columnNr=\"10\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "      <string columnNr=\"10\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">asd</string>"
        "    </namespace_name>"
        "    <function_call_parameter_list columnNr=\"14\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">"
        "      <lnumber columnNr=\"14\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "      <constant_encapsed_string columnNr=\"17\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">2</constant_encapsed_string>"
        "    </function_call_parameter_list>"
        "  </new>"
        "</top_statement_list>"
    );}
    void test_cloneDefinition() { TEST_PHP_PARSER(
        "<?php clone $a;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <clone columnNr=\"12\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </clone>"
        "</top_statement_list>"
    );}
    void test_assignPlusDefinition() { TEST_PHP_PARSER(
        "<?php $a+=$b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "  <plus_equal columnNr=\"6\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </plus_equal>"
        "</top_statement_list>"
    );}
    void test_assignMinusDefinition() { TEST_PHP_PARSER(
        "<?php $a-=$b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "  <minus_equal columnNr=\"6\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </minus_equal>"
        "</top_statement_list>"
    );}
    void test_assignMulDefinition() { TEST_PHP_PARSER(
        "<?php $a*=$b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "  <mul_equal columnNr=\"6\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </mul_equal>"
        "</top_statement_list>"
    );}
    void test_assignDivDefinition() { TEST_PHP_PARSER(
        "<?php $a/=$b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "  <div_equal columnNr=\"6\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </div_equal>"
        "</top_statement_list>"
    );}
    void test_assignConcatDefinition() { TEST_PHP_PARSER(
        "<?php $a.=$b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "  <concat_equal columnNr=\"6\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </concat_equal>"
        "</top_statement_list>"
    );}
    void test_assignModDefinition() { TEST_PHP_PARSER(
        "<?php $a%=$b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "  <mod_equal columnNr=\"6\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </mod_equal>"
        "</top_statement_list>"
    );}
    void test_assignAndDefinition() { TEST_PHP_PARSER(
        "<?php $a&=$b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "  <and_equal columnNr=\"6\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </and_equal>"
        "</top_statement_list>"
    );}
    void test_assignOrDefinition() { TEST_PHP_PARSER(
        "<?php $a|=$b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "  <or_equal columnNr=\"6\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </or_equal>"
        "</top_statement_list>"
    );}
    void test_assignXorDefinition() { TEST_PHP_PARSER(
        "<?php $a^=$b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "  <xor_equal columnNr=\"6\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </xor_equal>"
        "</top_statement_list>"
    );}
    void test_assignShLeftDefinition() { TEST_PHP_PARSER(
        "<?php $a<<=$b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <sl_equal columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </sl_equal>"
        "</top_statement_list>"
    );}
    void test_assignShRightDefinition() { TEST_PHP_PARSER(
        "<?php $a>>=$b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <sr_equal columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </sr_equal>"
        "</top_statement_list>"
    );}
    void test_postIncDefinition() { TEST_PHP_PARSER(
        "<?php $a++;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">"
        "  <postinc columnNr=\"6\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </postinc>"
        "</top_statement_list>"
    );}
    void testPhpParserp_preIncDefinition() { TEST_PHP_PARSER(
        "<?php ++$b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">"
        "  <preinc columnNr=\"8\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"8\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </preinc>"
        "</top_statement_list>"
    );}
    void test_postDecDefinition() { TEST_PHP_PARSER(
        "<?php $a--;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">"
        "  <postdec columnNr=\"6\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </postdec>"
        "</top_statement_list>"
    );}
    void test_preDecDefinition() { TEST_PHP_PARSER(
        "<?php --$a;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">"
        "  <predec columnNr=\"8\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"8\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </predec>"
        "</top_statement_list>"
    );}
    void test_boolOrDefinition() { TEST_PHP_PARSER(
        "<?php $a || $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <boolean_or columnNr=\"6\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </boolean_or>"
        "</top_statement_list>"
    );}
    void test_boolAndDefinition() { TEST_PHP_PARSER(
        "<?php $a && $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <boolean_and columnNr=\"6\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </boolean_and>"
        "</top_statement_list>"
    );}
    void test_logicalOrDefinition() { TEST_PHP_PARSER(
        "<?php $a or $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <logical_or columnNr=\"6\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </logical_or>"
        "</top_statement_list>"
    );}
    void test_logicalAndDefinition() { TEST_PHP_PARSER(
        "<?php $a and $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "  <logical_and columnNr=\"6\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"13\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </logical_and>"
        "</top_statement_list>"
    );}
    void test_logicalXorDefinition() { TEST_PHP_PARSER(
        "<?php $a xor $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "  <logical_xor columnNr=\"6\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"13\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </logical_xor>"
        "</top_statement_list>"
    );}
    void test_orDefinition() { TEST_PHP_PARSER(
        "<?php $a | $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <binary_or columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </binary_or>"
        "</top_statement_list>"
    );}
    void test_andDefinition() { TEST_PHP_PARSER(
        "<?php $a & $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <binary_and columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </binary_and>"
        "</top_statement_list>"
    );}
    void test_xorDefinition() { TEST_PHP_PARSER(
        "<?php $a ^ $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <binary_xor columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </binary_xor>"
        "</top_statement_list>"
    );}
    void tesconcatDefinition() { TEST_PHP_PARSER(
        "<?php $a . $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <concat columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </concat>"
        "</top_statement_list>"
    );}
    void tesplusDefinition() { TEST_PHP_PARSER(
        "<?php $a + $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <plus columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </plus>"
        "</top_statement_list>"
    );}
    void test_minusDefinition() { TEST_PHP_PARSER(
        "<?php $a - $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <minus columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </minus>"
        "</top_statement_list>"
    );}
    void test_mulDefinition() { TEST_PHP_PARSER(
        "<?php $a * $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <mul columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </mul>"
        "</top_statement_list>"
    );}
    void test_divDefinition() { TEST_PHP_PARSER(
        "<?php $a / $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <div columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </div>"
        "</top_statement_list>"
    );}
    void test_modDefinition() { TEST_PHP_PARSER(
        "<?php $a % $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <mod columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </mod>"
        "</top_statement_list>"
    );}
    void test_shLeftDefinition() { TEST_PHP_PARSER(
        "<?php $a << $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <shift_left columnNr=\"6\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </shift_left>"
        "</top_statement_list>"
    );}
    void test_shRightDefinition() { TEST_PHP_PARSER(
        "<?php $a >> $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <shift_right columnNr=\"6\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </shift_right>"
        "</top_statement_list>"
    );}
    void test_negateDefinition() { TEST_PHP_PARSER(
        "<?php !$a;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">"
        "  <logical_negate columnNr=\"7\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"7\" endColumnNr=\"9\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </logical_negate>"
        "</top_statement_list>"
    );}
    void test_inverseDefinition() { TEST_PHP_PARSER(
        "<?php ~$a;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">"
        "  <binary_inverse columnNr=\"7\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"7\" endColumnNr=\"9\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </binary_inverse>"
        "</top_statement_list>"
    );}
    void test_isIdenticalDefinition() { TEST_PHP_PARSER(
        "<?php $a === $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "  <is_identical columnNr=\"6\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"13\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </is_identical>"
        "</top_statement_list>"
    );}
    void test_isNotIdenticalDefinition() { TEST_PHP_PARSER(
        "<?php $a !== $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "  <is_not_identical columnNr=\"6\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"13\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </is_not_identical>"
        "</top_statement_list>"
    );}
    void test_isEqualDefinition() { TEST_PHP_PARSER(
        "<?php $a == $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <is_equal columnNr=\"6\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </is_equal>"
        "</top_statement_list>"
    );}
    void test_isNotEqualDefinition() { TEST_PHP_PARSER(
        "<?php $a != $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <is_not_equal columnNr=\"6\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </is_not_equal>"
        "</top_statement_list>"
    );}
    void test_lessDefinition() { TEST_PHP_PARSER(
        "<?php $a < $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <lessthan columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </lessthan>"
        "</top_statement_list>"
    );}
    void test_lessEqDefinition() { TEST_PHP_PARSER(
        "<?php $a <= $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <lessthan_equal columnNr=\"6\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </lessthan_equal>"
        "</top_statement_list>"
    );}
    void test_moreDefinition() { TEST_PHP_PARSER(
        "<?php $a > $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <greatherthan columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </greatherthan>"
        "</top_statement_list>"
    );}
    void test_moreEqDefinition() { TEST_PHP_PARSER(
        "<?php $a >= $b;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <greatherthan_equal columnNr=\"6\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "  </greatherthan_equal>"
        "</top_statement_list>"
    );}
    void test_instanceOfDefinition() { TEST_PHP_PARSER(
        "<?php $a instanceof B;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "  <instanceof columnNr=\"6\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <namespace_name columnNr=\"20\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "      <string columnNr=\"20\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">B</string>"
        "    </namespace_name>"
        "  </instanceof>"
        "</top_statement_list>"
    );}
    void test_parenthesesDefinition() { TEST_PHP_PARSER(
        "<?php ($a);",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"9\" endLineNr=\"1\" lineNr=\"1\">"
        "  <variable columnNr=\"7\" endColumnNr=\"9\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "</top_statement_list>"
    );}
    void test_ternaryDefinition() { TEST_PHP_PARSER(
        "<?php $a ? $b : $c ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "  <ternaryop columnNr=\"6\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <variable columnNr=\"11\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "    <variable columnNr=\"16\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">$c</variable>"
        "  </ternaryop>"
        "</top_statement_list>"
    );}
    void test_ternaryPartialDefinition() { TEST_PHP_PARSER(
        "<?php $a ? : $c ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">"
        "  <ternaryop columnNr=\"6\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <VOID columnNr=\"16\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <variable columnNr=\"13\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">$c</variable>"
        "  </ternaryop>"
        "</top_statement_list>"
    );}
    void test_castIntDefinition() { TEST_PHP_PARSER(
        "<?php (int) $a ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "  <int_cast columnNr=\"12\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </int_cast>"
        "</top_statement_list>"
    );}
    void test_castDoubleDefinition() { TEST_PHP_PARSER(
        "<?php (double) $a ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "  <double_cast columnNr=\"15\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"15\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </double_cast>"
        "</top_statement_list>"
    );}
    void test_castStringDefinition() { TEST_PHP_PARSER(
        "<?php (string) $a ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "  <string_cast columnNr=\"15\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"15\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </string_cast>"
        "</top_statement_list>"
    );}
    void test_castArrayDefinition() { TEST_PHP_PARSER(
        "<?php (array) $a ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">"
        "  <array_cast columnNr=\"14\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"14\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </array_cast>"
        "</top_statement_list>"
    );}
    void test_castObjectDefinition() { TEST_PHP_PARSER(
        "<?php (object) $a ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "  <object_cast columnNr=\"15\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"15\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </object_cast>"
        "</top_statement_list>"
    );}
    void test_castBoolDefinition() { TEST_PHP_PARSER(
        "<?php (bool) $a ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">"
        "  <bool_cast columnNr=\"13\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"13\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </bool_cast>"
        "</top_statement_list>"
    );}
    void test_castUnsetDefinition() { TEST_PHP_PARSER(
        "<?php (unset) $a ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">"
        "  <unset_cast columnNr=\"14\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"14\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </unset_cast>"
        "</top_statement_list>"
    );}
    void test_exitDefinition() { TEST_PHP_PARSER(
        "<?php exit (1) ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "  <exit columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "    <lnumber columnNr=\"12\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "  </exit>"
        "</top_statement_list>"
    );}
    void test_silenceDefinition() { TEST_PHP_PARSER(
        "<?php @$a ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">"
        "  <silence columnNr=\"7\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"7\" endColumnNr=\"9\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "  </silence>"
        "</top_statement_list>"
    );}
    void test_arrayDefinition() { TEST_PHP_PARSER(
        "<?php array(1=>$a, &$b, 'c'=>3+2,) ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "  <array columnNr=\"12\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "    <array_pair_list columnNr=\"12\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "      <array_pair columnNr=\"12\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">"
        "        <array_key columnNr=\"12\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">"
        "          <lnumber columnNr=\"12\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "        </array_key>"
        "        <array_value columnNr=\"15\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">"
        "          <variable columnNr=\"15\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "        </array_value>"
        "      </array_pair>"
        "      <array_pair columnNr=\"20\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">"
        "        <array_key columnNr=\"22\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\"/>"
        "        <array_value columnNr=\"20\" endColumnNr=\"23\" endLineNr=\"1\" is_reference=\"1\" lineNr=\"1\">"
        "          <variable columnNr=\"20\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "        </array_value>"
        "      </array_pair>"
        "      <array_pair columnNr=\"24\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "        <array_key columnNr=\"24\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "          <constant_encapsed_string columnNr=\"24\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">c</constant_encapsed_string>"
        "        </array_key>"
        "        <array_value columnNr=\"29\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "          <plus columnNr=\"29\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "            <lnumber columnNr=\"29\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">3</lnumber>"
        "            <lnumber columnNr=\"31\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">2</lnumber>"
        "          </plus>"
        "        </array_value>"
        "      </array_pair>"
        "    </array_pair_list>"
        "  </array>"
        "</top_statement_list>"
    );}
    void test_arrayDefinitionRef() { TEST_PHP_PARSER(
        "<?php array(&$a, &$b) ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "  <array columnNr=\"13\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "    <array_pair_list columnNr=\"13\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "      <array_pair columnNr=\"13\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "        <array_key columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\"/>"
        "        <array_value columnNr=\"13\" endColumnNr=\"16\" endLineNr=\"1\" is_reference=\"1\" lineNr=\"1\">"
        "          <variable columnNr=\"13\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "        </array_value>"
        "      </array_pair>"
        "      <array_pair columnNr=\"18\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "        <array_key columnNr=\"20\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\"/>"
        "        <array_value columnNr=\"18\" endColumnNr=\"21\" endLineNr=\"1\" is_reference=\"1\" lineNr=\"1\">"
        "          <variable columnNr=\"18\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "        </array_value>"
        "      </array_pair>"
        "    </array_pair_list>"
        "  </array>"
        "</top_statement_list>"
    );}
    void test_backticksDefinition() { TEST_PHP_PARSER(
        "<?php `$a boo $2 moo` ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "  <backticks columnNr=\"7\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "    <encaps_list columnNr=\"7\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "      <variable columnNr=\"7\" endColumnNr=\"9\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "      <encapsed_and_whitespace columnNr=\"9\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\"> boo $2 moo</encapsed_and_whitespace>"
        "    </encaps_list>"
        "  </backticks>"
        "</top_statement_list>"
    );}
    void test_backticksStartsWithBackslash() { TEST_PHP_PARSER(
        "<?php `\\$a boo moo` ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "  <backticks columnNr=\"7\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "    <encapsed_and_whitespace columnNr=\"7\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">\\$a boo moo</encapsed_and_whitespace>"
        "  </backticks>"
        "</top_statement_list>"
    );}
    void test_backticksContainsBackslash() { TEST_PHP_PARSER(
        "<?php `asd\\$a boo {}moo` ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "  <backticks columnNr=\"7\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "    <encapsed_and_whitespace columnNr=\"7\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">asd\\$a boo {}moo</encapsed_and_whitespace>"
        "  </backticks>"
        "</top_statement_list>"
    );}
    void test_backticksWithTwoVariables() { TEST_PHP_PARSER(
        "<?php `asd$a $boo moo` ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "  <backticks columnNr=\"7\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "    <encaps_list columnNr=\"7\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "      <encapsed_and_whitespace columnNr=\"7\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">asd</encapsed_and_whitespace>"
        "      <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "      <encapsed_and_whitespace columnNr=\"12\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\"> </encapsed_and_whitespace>"
        "      <variable columnNr=\"13\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">$boo</variable>"
        "      <encapsed_and_whitespace columnNr=\"17\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\"> moo</encapsed_and_whitespace>"
        "    </encaps_list>"
        "  </backticks>"
        "</top_statement_list>"
    );}
    void test_backticksWithTwoVariablesAndSecondIsBracketed() { TEST_PHP_PARSER(
        "<?php `asd$a {$boo} moo` ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "  <backticks columnNr=\"7\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "    <encaps_list columnNr=\"7\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "      <encapsed_and_whitespace columnNr=\"7\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">asd</encapsed_and_whitespace>"
        "      <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "      <encapsed_and_whitespace columnNr=\"12\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\"> </encapsed_and_whitespace>"
        "      <variable columnNr=\"15\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">$boo</variable>"
        "      <encapsed_and_whitespace columnNr=\"20\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\"> moo</encapsed_and_whitespace>"
        "    </encaps_list>"
        "  </backticks>"
        "</top_statement_list>"
    );}
    void test_printDefinition() { TEST_PHP_PARSER(
        "<?php print $x ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "  <print columnNr=\"12\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"12\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">$x</variable>"
        "  </print>"
        "</top_statement_list>"
    );}
    void test_lambdaFncDefinition() { TEST_PHP_PARSER(
        "<?php function ($a) { return $c; } ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "  <LAMBDA_FUNCTION columnNr=\"15\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "    <is_reference columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" is_reference=\"0\" lineNr=\"1\"/>"
        "    <parameter_list columnNr=\"16\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "      <parameter columnNr=\"16\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "        <__ANY_CLASS_TYPE__ columnNr=\"16\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\"/>"
        "        <variable columnNr=\"16\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "      </parameter>"
        "    </parameter_list>"
        "    <use columnNr=\"20\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <inner_statement_list columnNr=\"20\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">"
        "      <return columnNr=\"29\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">"
        "        <variable columnNr=\"29\" endColumnNr=\"31\" endLineNr=\"1\" lineNr=\"1\">$c</variable>"
        "      </return>"
        "    </inner_statement_list>"
        "  </LAMBDA_FUNCTION>"
        "</top_statement_list>"
    );}
    void test_lambdaFncUseDefinition() { TEST_PHP_PARSER(
        "<?php function ($a) use ($b) { return $c; } ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"43\" endLineNr=\"1\" lineNr=\"1\">"
        "  <LAMBDA_FUNCTION columnNr=\"15\" endColumnNr=\"43\" endLineNr=\"1\" lineNr=\"1\">"
        "    <is_reference columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" is_reference=\"0\" lineNr=\"1\"/>"
        "    <parameter_list columnNr=\"16\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "      <parameter columnNr=\"16\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "        <__ANY_CLASS_TYPE__ columnNr=\"16\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\"/>"
        "        <variable columnNr=\"16\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "      </parameter>"
        "    </parameter_list>"
        "    <use columnNr=\"25\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">"
        "      <lexical_var_list columnNr=\"25\" endColumnNr=\"27\" endLineNr=\"1\" lineNr=\"1\">"
        "        <variable columnNr=\"25\" endColumnNr=\"27\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "      </lexical_var_list>"
        "    </use>"
        "    <inner_statement_list columnNr=\"29\" endColumnNr=\"41\" endLineNr=\"1\" lineNr=\"1\">"
        "      <return columnNr=\"38\" endColumnNr=\"41\" endLineNr=\"1\" lineNr=\"1\">"
        "        <variable columnNr=\"38\" endColumnNr=\"40\" endLineNr=\"1\" lineNr=\"1\">$c</variable>"
        "      </return>"
        "    </inner_statement_list>"
        "  </LAMBDA_FUNCTION>"
        "</top_statement_list>"
    );}
    void test_lambdaFncRefDefinition() { TEST_PHP_PARSER(
        "<?php function & ($a) use ($b, &$c) { return $c; } ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"50\" endLineNr=\"1\" lineNr=\"1\">"
        "  <LAMBDA_FUNCTION columnNr=\"15\" endColumnNr=\"50\" endLineNr=\"1\" lineNr=\"1\">"
        "    <is_reference columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" is_reference=\"1\" lineNr=\"1\"/>"
        "    <parameter_list columnNr=\"18\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "      <parameter columnNr=\"18\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "        <__ANY_CLASS_TYPE__ columnNr=\"18\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\"/>"
        "        <variable columnNr=\"18\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "      </parameter>"
        "    </parameter_list>"
        "    <use columnNr=\"27\" endColumnNr=\"35\" endLineNr=\"1\" lineNr=\"1\">"
        "      <lexical_var_list columnNr=\"27\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "        <variable columnNr=\"27\" endColumnNr=\"29\" endLineNr=\"1\" lineNr=\"1\">$b</variable>"
        "        <variable columnNr=\"32\" endColumnNr=\"34\" endLineNr=\"1\" is_reference=\"1\" lineNr=\"1\">$c</variable>"
        "      </lexical_var_list>"
        "    </use>"
        "    <inner_statement_list columnNr=\"36\" endColumnNr=\"48\" endLineNr=\"1\" lineNr=\"1\">"
        "      <return columnNr=\"45\" endColumnNr=\"48\" endLineNr=\"1\" lineNr=\"1\">"
        "        <variable columnNr=\"45\" endColumnNr=\"47\" endLineNr=\"1\" lineNr=\"1\">$c</variable>"
        "      </return>"
        "    </inner_statement_list>"
        "  </LAMBDA_FUNCTION>"
        "</top_statement_list>"
    );}
    void test_doubleQuotesConst() { TEST_PHP_PARSER(
        "<?php $a = \"\\rtest $ {} string\\n\" ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"35\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"35\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <constant_encapsed_string columnNr=\"11\" endColumnNr=\"31\" endLineNr=\"1\" lineNr=\"1\">\\rtest $ {} string\\n</constant_encapsed_string>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_doubleQuotesWithVars() { TEST_PHP_PARSER(
        "<?php $a = \"\\ntest $moo more text\n\\n {$boo}${buka}s${aa[2]} {$o->prop}trin $ {} g\" ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"52\" endLineNr=\"2\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"52\" endLineNr=\"2\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <doubleQuotes columnNr=\"12\" endColumnNr=\"50\" endLineNr=\"2\" lineNr=\"1\">"
        "      <encaps_list columnNr=\"12\" endColumnNr=\"49\" endLineNr=\"2\" lineNr=\"1\">"
        "        <encapsed_and_whitespace columnNr=\"12\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">\\ntest </encapsed_and_whitespace>"
        "        <variable columnNr=\"19\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">$moo</variable>"
        "        <encapsed_and_whitespace columnNr=\"23\" endColumnNr=\"4\" endLineNr=\"2\" lineNr=\"1\"> more text&#xA;\\n </encapsed_and_whitespace>"
        "        <variable columnNr=\"5\" endColumnNr=\"9\" endLineNr=\"2\" lineNr=\"2\">$boo</variable>"
        "        <string_varname columnNr=\"12\" endColumnNr=\"16\" endLineNr=\"2\" lineNr=\"2\">buka</string_varname>"
        "        <encapsed_and_whitespace columnNr=\"17\" endColumnNr=\"18\" endLineNr=\"2\" lineNr=\"2\">s</encapsed_and_whitespace>"
        "        <offset columnNr=\"20\" endColumnNr=\"26\" endLineNr=\"2\" lineNr=\"2\">"
        "          <string_varname columnNr=\"20\" endColumnNr=\"22\" endLineNr=\"2\" lineNr=\"2\">aa</string_varname>"
        "          <lnumber columnNr=\"23\" endColumnNr=\"24\" endLineNr=\"2\" lineNr=\"2\">2</lnumber>"
        "        </offset>"
        "        <encapsed_and_whitespace columnNr=\"26\" endColumnNr=\"27\" endLineNr=\"2\" lineNr=\"2\"> </encapsed_and_whitespace>"
        "        <object_operator columnNr=\"29\" endColumnNr=\"38\" endLineNr=\"2\" lineNr=\"2\">"
        "          <variable columnNr=\"29\" endColumnNr=\"31\" endLineNr=\"2\" lineNr=\"2\">$o</variable>"
        "          <string columnNr=\"33\" endColumnNr=\"37\" endLineNr=\"2\" lineNr=\"2\">prop</string>"
        "          <method_or_not columnNr=\"37\" endColumnNr=\"38\" endLineNr=\"2\" lineNr=\"2\"/>"
        "          <variable_properties columnNr=\"37\" endColumnNr=\"38\" endLineNr=\"2\" lineNr=\"2\"/>"
        "        </object_operator>"
        "        <encapsed_and_whitespace columnNr=\"38\" endColumnNr=\"49\" endLineNr=\"2\" lineNr=\"2\">trin $ {} g</encapsed_and_whitespace>"
        "      </encaps_list>"
        "    </doubleQuotes>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_doubleQuotesWithVarsStartsBracketed() { TEST_PHP_PARSER(
        "<?php $a = \"\\ntest {$moo} more text\n\\n {$boo}${buka}s${aa[2]} {$o->prop}trin $ {} g\" ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"52\" endLineNr=\"2\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"52\" endLineNr=\"2\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <doubleQuotes columnNr=\"12\" endColumnNr=\"50\" endLineNr=\"2\" lineNr=\"1\">"
        "      <encaps_list columnNr=\"12\" endColumnNr=\"49\" endLineNr=\"2\" lineNr=\"1\">"
        "        <encapsed_and_whitespace columnNr=\"12\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">\\ntest </encapsed_and_whitespace>"
        "        <variable columnNr=\"21\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">$moo</variable>"
        "        <encapsed_and_whitespace columnNr=\"26\" endColumnNr=\"4\" endLineNr=\"2\" lineNr=\"1\"> more text&#xA;\\n </encapsed_and_whitespace>"
        "        <variable columnNr=\"5\" endColumnNr=\"9\" endLineNr=\"2\" lineNr=\"2\">$boo</variable>"
        "        <string_varname columnNr=\"12\" endColumnNr=\"16\" endLineNr=\"2\" lineNr=\"2\">buka</string_varname>"
        "        <encapsed_and_whitespace columnNr=\"17\" endColumnNr=\"18\" endLineNr=\"2\" lineNr=\"2\">s</encapsed_and_whitespace>"
        "        <offset columnNr=\"20\" endColumnNr=\"26\" endLineNr=\"2\" lineNr=\"2\">"
        "          <string_varname columnNr=\"20\" endColumnNr=\"22\" endLineNr=\"2\" lineNr=\"2\">aa</string_varname>"
        "          <lnumber columnNr=\"23\" endColumnNr=\"24\" endLineNr=\"2\" lineNr=\"2\">2</lnumber>"
        "        </offset>"
        "        <encapsed_and_whitespace columnNr=\"26\" endColumnNr=\"27\" endLineNr=\"2\" lineNr=\"2\"> </encapsed_and_whitespace>"
        "        <object_operator columnNr=\"29\" endColumnNr=\"38\" endLineNr=\"2\" lineNr=\"2\">"
        "          <variable columnNr=\"29\" endColumnNr=\"31\" endLineNr=\"2\" lineNr=\"2\">$o</variable>"
        "          <string columnNr=\"33\" endColumnNr=\"37\" endLineNr=\"2\" lineNr=\"2\">prop</string>"
        "          <method_or_not columnNr=\"37\" endColumnNr=\"38\" endLineNr=\"2\" lineNr=\"2\"/>"
        "          <variable_properties columnNr=\"37\" endColumnNr=\"38\" endLineNr=\"2\" lineNr=\"2\"/>"
        "        </object_operator>"
        "        <encapsed_and_whitespace columnNr=\"38\" endColumnNr=\"49\" endLineNr=\"2\" lineNr=\"2\">trin $ {} g</encapsed_and_whitespace>"
        "      </encaps_list>"
        "    </doubleQuotes>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_doubleQuotesWithNonVars() { TEST_PHP_PARSER(
        "<?php $a = \"test $12 more text\" ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <constant_encapsed_string columnNr=\"11\" endColumnNr=\"29\" endLineNr=\"1\" lineNr=\"1\">test $12 more text</constant_encapsed_string>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_singleQuotesEscaping() { TEST_PHP_PARSER(
        "<?php $a = 'test\\' $a more text' ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <constant_encapsed_string columnNr=\"11\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">test\\' $a more text</constant_encapsed_string>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_hereDoc() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\ntex{t\nMYDOC; ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <encapsed_and_whitespace columnNr=\"0\" endColumnNr=\"5\" endLineNr=\"2\" lineNr=\"2\">tex{t</encapsed_and_whitespace>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_hereDocWithWindowsEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\ntext\nMYDOC; ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <encapsed_and_whitespace columnNr=\"0\" endColumnNr=\"4\" endLineNr=\"2\" lineNr=\"2\">text</encapsed_and_whitespace>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_hereDocWithMacEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\rtext\rMYDOC; ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"31\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"31\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <encapsed_and_whitespace columnNr=\"20\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">text</encapsed_and_whitespace>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_hereDocMultiline() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\rtext\na\rs\r\nd\n\rMYDOC; ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"7\" endLineNr=\"4\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"7\" endLineNr=\"4\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <encapsed_and_whitespace columnNr=\"20\" endColumnNr=\"1\" endLineNr=\"4\" lineNr=\"1\">text&#xA;a&#xD;s&#xD;&#xA;d&#xA;</encapsed_and_whitespace>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_hereDocWithVars() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\ntex$var1 {$var2} {$var3[$i]} {$var4->prop} ${var5[$i]}$var6[$i]$var7->prop t\nMYDOC; ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <hereDoc columnNr=\"0\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"2\">"
        "      <encaps_list columnNr=\"0\" endColumnNr=\"83\" endLineNr=\"2\" lineNr=\"2\">"
        "        <encapsed_and_whitespace columnNr=\"0\" endColumnNr=\"3\" endLineNr=\"2\" lineNr=\"2\">tex</encapsed_and_whitespace>"
        "        <variable columnNr=\"3\" endColumnNr=\"8\" endLineNr=\"2\" lineNr=\"2\">$var1</variable>"
        "        <encapsed_and_whitespace columnNr=\"8\" endColumnNr=\"9\" endLineNr=\"2\" lineNr=\"2\"> </encapsed_and_whitespace>"
        "        <variable columnNr=\"11\" endColumnNr=\"16\" endLineNr=\"2\" lineNr=\"2\">$var2</variable>"
        "        <encapsed_and_whitespace columnNr=\"17\" endColumnNr=\"18\" endLineNr=\"2\" lineNr=\"2\"> </encapsed_and_whitespace>"
        "        <offset columnNr=\"20\" endColumnNr=\"29\" endLineNr=\"2\" lineNr=\"2\">"
        "          <variable columnNr=\"20\" endColumnNr=\"25\" endLineNr=\"2\" lineNr=\"2\">$var3</variable>"
        "          <variable columnNr=\"26\" endColumnNr=\"28\" endLineNr=\"2\" lineNr=\"2\">$i</variable>"
        "        </offset>"
        "        <encapsed_and_whitespace columnNr=\"30\" endColumnNr=\"31\" endLineNr=\"2\" lineNr=\"2\"> </encapsed_and_whitespace>"
        "        <object_operator columnNr=\"33\" endColumnNr=\"45\" endLineNr=\"2\" lineNr=\"2\">"
        "          <variable columnNr=\"33\" endColumnNr=\"38\" endLineNr=\"2\" lineNr=\"2\">$var4</variable>"
        "          <string columnNr=\"40\" endColumnNr=\"44\" endLineNr=\"2\" lineNr=\"2\">prop</string>"
        "          <method_or_not columnNr=\"44\" endColumnNr=\"45\" endLineNr=\"2\" lineNr=\"2\"/>"
        "          <variable_properties columnNr=\"44\" endColumnNr=\"45\" endLineNr=\"2\" lineNr=\"2\"/>"
        "        </object_operator>"
        "        <encapsed_and_whitespace columnNr=\"45\" endColumnNr=\"46\" endLineNr=\"2\" lineNr=\"2\"> </encapsed_and_whitespace>"
        "        <offset columnNr=\"48\" endColumnNr=\"57\" endLineNr=\"2\" lineNr=\"2\">"
        "          <string_varname columnNr=\"48\" endColumnNr=\"52\" endLineNr=\"2\" lineNr=\"2\">var5</string_varname>"
        "          <variable columnNr=\"53\" endColumnNr=\"55\" endLineNr=\"2\" lineNr=\"2\">$i</variable>"
        "        </offset>"
        "        <offset columnNr=\"57\" endColumnNr=\"67\" endLineNr=\"2\" lineNr=\"2\">"
        "          <variable columnNr=\"57\" endColumnNr=\"62\" endLineNr=\"2\" lineNr=\"2\">$var6</variable>"
        "          <variable columnNr=\"64\" endColumnNr=\"66\" endLineNr=\"2\" lineNr=\"2\">$i</variable>"
        "        </offset>"
        "        <object_operator columnNr=\"67\" endColumnNr=\"81\" endLineNr=\"2\" lineNr=\"2\">"
        "          <variable columnNr=\"67\" endColumnNr=\"72\" endLineNr=\"2\" lineNr=\"2\">$var7</variable>"
        "          <string columnNr=\"77\" endColumnNr=\"81\" endLineNr=\"2\" lineNr=\"2\">prop</string>"
        "        </object_operator>"
        "        <encapsed_and_whitespace columnNr=\"81\" endColumnNr=\"83\" endLineNr=\"2\" lineNr=\"2\"> t</encapsed_and_whitespace>"
        "      </encaps_list>"
        "    </hereDoc>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_hereDocWithQuoteLabels() { TEST_PHP_PARSER(
        "<?php $a = <<<\"MYDOC\"\ntext\nMYDOC; ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <encapsed_and_whitespace columnNr=\"0\" endColumnNr=\"4\" endLineNr=\"2\" lineNr=\"2\">text</encapsed_and_whitespace>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_hereDocWithNonVars() { TEST_PHP_PARSER(
        "<?php $a = <<<\"MYDOC\"\ntex $123 t\nMYDOC; ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <encapsed_and_whitespace columnNr=\"0\" endColumnNr=\"10\" endLineNr=\"2\" lineNr=\"2\">tex $123 t</encapsed_and_whitespace>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_nowDoc() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\ntext$nonvar w\nMYDOC; ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <encapsed_and_whitespace columnNr=\"0\" endColumnNr=\"13\" endLineNr=\"2\" lineNr=\"2\">text$nonvar w</encapsed_and_whitespace>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_nowDocWithWindowsEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\r\ntext\r\nMYDOC; ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <encapsed_and_whitespace columnNr=\"0\" endColumnNr=\"4\" endLineNr=\"2\" lineNr=\"2\">text</encapsed_and_whitespace>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_nowDocWithMacEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\rtext\rMYDOC; ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <encapsed_and_whitespace columnNr=\"22\" endColumnNr=\"26\" endLineNr=\"1\" lineNr=\"1\">text</encapsed_and_whitespace>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_nowDocMultiline() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\rtext\na\rs\r\nd\n\rMYDOC; ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"7\" endLineNr=\"4\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"7\" endLineNr=\"4\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <encapsed_and_whitespace columnNr=\"22\" endColumnNr=\"1\" endLineNr=\"4\" lineNr=\"1\">text&#xA;a&#xD;s&#xD;&#xA;d&#xA;</encapsed_and_whitespace>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_singleLineComment() { TEST_PHP_PARSER(
        "<?php //$a =? <<<'MYDOC'\r\n$s=1; // asd \r\n // //..  ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"5\" endLineNr=\"2\" lineNr=\"1\">"
        "  <assignment columnNr=\"0\" endColumnNr=\"5\" endLineNr=\"2\" lineNr=\"2\">"
        "    <variable columnNr=\"0\" endColumnNr=\"2\" endLineNr=\"2\" lineNr=\"2\">$s</variable>"
        "    <lnumber columnNr=\"3\" endColumnNr=\"4\" endLineNr=\"2\" lineNr=\"2\">1</lnumber>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_singleLineCommentWindowsEndline() { TEST_PHP_PARSER(
        "<?php //$a = <<<'MYDOC'\r\n$s=1; // asd \r\n // //..  ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"5\" endLineNr=\"2\" lineNr=\"1\">"
        "  <assignment columnNr=\"0\" endColumnNr=\"5\" endLineNr=\"2\" lineNr=\"2\">"
        "    <variable columnNr=\"0\" endColumnNr=\"2\" endLineNr=\"2\" lineNr=\"2\">$s</variable>"
        "    <lnumber columnNr=\"3\" endColumnNr=\"4\" endLineNr=\"2\" lineNr=\"2\">1</lnumber>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_singleLineCommentMacEndline() { TEST_PHP_PARSER(
        "<?php //$a = <<<'MYDOC'\r$s=1; \r// asd  // //..  ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"29\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"24\" endColumnNr=\"29\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"24\" endColumnNr=\"26\" endLineNr=\"1\" lineNr=\"1\">$s</variable>"
        "    <lnumber columnNr=\"27\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_singleLineCommentScriptStop() { TEST_PHP_PARSER(
        "<?php //$a =? <<<'MYDOC'?>$s=1; // asd \n // //..  ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"11\" endLineNr=\"2\" lineNr=\"1\">"
        "  <empty statement columnNr=\"24\" endColumnNr=\"26\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  <inline_html columnNr=\"26\" endColumnNr=\"11\" endLineNr=\"2\" lineNr=\"1\">$s=1; // asd &#xA; // //..  </inline_html>"
        "</top_statement_list>"
    );}
    void test_singleLineHashComment() { TEST_PHP_PARSER(
        "<?php #$a = <<<'MYDOC'\n$s=1; # asd \n # /..  ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"5\" endLineNr=\"2\" lineNr=\"1\">"
        "  <assignment columnNr=\"0\" endColumnNr=\"5\" endLineNr=\"2\" lineNr=\"2\">"
        "    <variable columnNr=\"0\" endColumnNr=\"2\" endLineNr=\"2\" lineNr=\"2\">$s</variable>"
        "    <lnumber columnNr=\"3\" endColumnNr=\"4\" endLineNr=\"2\" lineNr=\"2\">1</lnumber>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_multiLineComment() { TEST_PHP_PARSER(
        "<?php /* $a = *<<<'MYDOC'\n$s=1; # asd \n # /.. */ $b=2; ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"3\" lineNr=\"1\">"
        "  <assignment columnNr=\"10\" endColumnNr=\"15\" endLineNr=\"3\" lineNr=\"3\">"
        "    <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"3\" lineNr=\"3\">$b</variable>"
        "    <lnumber columnNr=\"13\" endColumnNr=\"14\" endLineNr=\"3\" lineNr=\"3\">2</lnumber>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_docComment() { TEST_PHP_PARSER(
        "<?php /** lala */ ",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"0\" endLineNr=\"1\" lineNr=\"1\"/>"
    );}
    void test_numbers() { TEST_PHP_PARSER(
        "<?php $a=12+12.2+.2+0x02+0123+2e1;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <plus columnNr=\"9\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "      <plus columnNr=\"9\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">"
        "        <plus columnNr=\"9\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "          <plus columnNr=\"9\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "            <plus columnNr=\"9\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">"
        "              <lnumber columnNr=\"9\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">12</lnumber>"
        "              <dnumber columnNr=\"12\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">12.2</dnumber>"
        "            </plus>"
        "            <dnumber columnNr=\"17\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">.2</dnumber>"
        "          </plus>"
        "          <lnumber columnNr=\"20\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">0x02</lnumber>"
        "        </plus>"
        "        <lnumber columnNr=\"25\" endColumnNr=\"29\" endLineNr=\"1\" lineNr=\"1\">0123</lnumber>"
        "      </plus>"
        "      <dnumber columnNr=\"30\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">2e1</dnumber>"
        "    </plus>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_phpConstants() { TEST_PHP_PARSER(
        "<?php $a=__CLASS__.__FUNCTION__.__METHOD__.__LINE__.__FILE__.__DIR__.__NAMESPACE__;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"83\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"83\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <concat columnNr=\"9\" endColumnNr=\"83\" endLineNr=\"1\" lineNr=\"1\">"
        "      <concat columnNr=\"9\" endColumnNr=\"69\" endLineNr=\"1\" lineNr=\"1\">"
        "        <concat columnNr=\"9\" endColumnNr=\"61\" endLineNr=\"1\" lineNr=\"1\">"
        "          <concat columnNr=\"9\" endColumnNr=\"52\" endLineNr=\"1\" lineNr=\"1\">"
        "            <concat columnNr=\"9\" endColumnNr=\"43\" endLineNr=\"1\" lineNr=\"1\">"
        "              <concat columnNr=\"9\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">"
        "                <__CLASS__ columnNr=\"9\" endColumnNr=\"9\" endLineNr=\"1\" lineNr=\"1\"/>"
        "                <__FUNCTION__ columnNr=\"19\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\"/>"
        "              </concat>"
        "              <__METHOD__ columnNr=\"32\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\"/>"
        "            </concat>"
        "            <__LINE__ columnNr=\"43\" endColumnNr=\"43\" endLineNr=\"1\" lineNr=\"1\"/>"
        "          </concat>"
        "          <__FILE__ columnNr=\"52\" endColumnNr=\"52\" endLineNr=\"1\" lineNr=\"1\"/>"
        "        </concat>"
        "        <__DIR__ columnNr=\"61\" endColumnNr=\"61\" endLineNr=\"1\" lineNr=\"1\"/>"
        "      </concat>"
        "      <__NAMESPACE__ columnNr=\"69\" endColumnNr=\"69\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    </concat>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_classDefinition() { TEST_PHP_PARSER(
        "<?php class A { }",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">"
        "  <class_declaration columnNr=\"6\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">"
        "    <class columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <string columnNr=\"12\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">A</string>"
        "    <extends columnNr=\"14\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <implements columnNr=\"14\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <class_statement_list columnNr=\"14\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </class_declaration>"
        "</top_statement_list>"
    );}
    void test_classExtendingOtherDefinition() { TEST_PHP_PARSER(
        "<?php class A extends B { }",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"27\" endLineNr=\"1\" lineNr=\"1\">"
        "  <class_declaration columnNr=\"6\" endColumnNr=\"27\" endLineNr=\"1\" lineNr=\"1\">"
        "    <class columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <string columnNr=\"12\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">A</string>"
        "    <extends columnNr=\"22\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "      <namespace_name columnNr=\"22\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">"
        "        <string columnNr=\"22\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">B</string>"
        "      </namespace_name>"
        "    </extends>"
        "    <implements columnNr=\"24\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <class_statement_list columnNr=\"24\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </class_declaration>"
        "</top_statement_list>"
    );}
    void test_classImplementingInterfaceDefinition() { TEST_PHP_PARSER(
        "<?php class A implements I { }",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">"
        "  <class_declaration columnNr=\"6\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">"
        "    <class columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <string columnNr=\"12\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">A</string>"
        "    <extends columnNr=\"14\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <implements columnNr=\"25\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">"
        "      <interfaceList columnNr=\"25\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">"
        "        <namespace_name columnNr=\"25\" endColumnNr=\"26\" endLineNr=\"1\" lineNr=\"1\">"
        "          <string columnNr=\"25\" endColumnNr=\"26\" endLineNr=\"1\" lineNr=\"1\">I</string>"
        "        </namespace_name>"
        "      </interfaceList>"
        "    </implements>"
        "    <class_statement_list columnNr=\"27\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </class_declaration>"
        "</top_statement_list>"
    );}
    void test_abstractClassDefinition() { TEST_PHP_PARSER(
        "<?php abstract class A { }",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"26\" endLineNr=\"1\" lineNr=\"1\">"
        "  <class_declaration columnNr=\"15\" endColumnNr=\"26\" endLineNr=\"1\" lineNr=\"1\">"
        "    <class columnNr=\"15\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\" type=\"abstract\"/>"
        "    <string columnNr=\"21\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">A</string>"
        "    <extends columnNr=\"23\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <implements columnNr=\"23\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <class_statement_list columnNr=\"23\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </class_declaration>"
        "</top_statement_list>"
    );}
    void test_finalClassDefinition() { TEST_PHP_PARSER(
        "<?php final class A { }",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">"
        "  <class_declaration columnNr=\"12\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">"
        "    <class columnNr=\"12\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\" type=\"final\"/>"
        "    <string columnNr=\"18\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">A</string>"
        "    <extends columnNr=\"20\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <implements columnNr=\"20\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <class_statement_list columnNr=\"20\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </class_declaration>"
        "</top_statement_list>"
    );}
    void test_interfaceDefinition() { TEST_PHP_PARSER(
        "<?php interface A { }",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "  <interface_declaration columnNr=\"6\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">"
        "    <interface columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <string columnNr=\"16\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">A</string>"
        "    <extends columnNr=\"18\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <class_statement_list columnNr=\"18\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </interface_declaration>"
        "</top_statement_list>"
    );}
    void test_interfaceInheritanceDefinition() { TEST_PHP_PARSER(
        "<?php interface A extends B, C { }",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "  <interface_declaration columnNr=\"6\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "    <interface columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <string columnNr=\"16\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">A</string>"
        "    <extends columnNr=\"26\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">"
        "      <interfaceList columnNr=\"26\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">"
        "        <namespace_name columnNr=\"26\" endColumnNr=\"27\" endLineNr=\"1\" lineNr=\"1\">"
        "          <string columnNr=\"26\" endColumnNr=\"27\" endLineNr=\"1\" lineNr=\"1\">B</string>"
        "        </namespace_name>"
        "        <namespace_name columnNr=\"29\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">"
        "          <string columnNr=\"29\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">C</string>"
        "        </namespace_name>"
        "      </interfaceList>"
        "    </extends>"
        "    <class_statement_list columnNr=\"31\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </interface_declaration>"
        "</top_statement_list>"
    );}
    void test_statementIf() { TEST_PHP_PARSER(
        "<?php if(1) {echo 'x';} elseif (2) { echo 'y'; } else echo 'z';",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"63\" endLineNr=\"1\" lineNr=\"1\">"
        "  <if columnNr=\"9\" endColumnNr=\"63\" endLineNr=\"1\" lineNr=\"1\">"
        "    <lnumber columnNr=\"9\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "    <inner_statement_list columnNr=\"12\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "      <echo columnNr=\"18\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "        <echo_expr_list columnNr=\"18\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "          <constant_encapsed_string columnNr=\"18\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">x</constant_encapsed_string>"
        "        </echo_expr_list>"
        "      </echo>"
        "    </inner_statement_list>"
        "    <elseif_list columnNr=\"32\" endColumnNr=\"48\" endLineNr=\"1\" lineNr=\"1\">"
        "      <elseif columnNr=\"32\" endColumnNr=\"48\" endLineNr=\"1\" lineNr=\"1\">"
        "        <lnumber columnNr=\"32\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">2</lnumber>"
        "        <inner_statement_list columnNr=\"35\" endColumnNr=\"46\" endLineNr=\"1\" lineNr=\"1\">"
        "          <echo columnNr=\"42\" endColumnNr=\"46\" endLineNr=\"1\" lineNr=\"1\">"
        "            <echo_expr_list columnNr=\"42\" endColumnNr=\"46\" endLineNr=\"1\" lineNr=\"1\">"
        "              <constant_encapsed_string columnNr=\"42\" endColumnNr=\"43\" endLineNr=\"1\" lineNr=\"1\">y</constant_encapsed_string>"
        "            </echo_expr_list>"
        "          </echo>"
        "        </inner_statement_list>"
        "      </elseif>"
        "    </elseif_list>"
        "    <else columnNr=\"59\" endColumnNr=\"63\" endLineNr=\"1\" lineNr=\"1\">"
        "      <echo columnNr=\"59\" endColumnNr=\"63\" endLineNr=\"1\" lineNr=\"1\">"
        "        <echo_expr_list columnNr=\"59\" endColumnNr=\"63\" endLineNr=\"1\" lineNr=\"1\">"
        "          <constant_encapsed_string columnNr=\"59\" endColumnNr=\"60\" endLineNr=\"1\" lineNr=\"1\">z</constant_encapsed_string>"
        "        </echo_expr_list>"
        "      </echo>"
        "    </else>"
        "  </if>"
        "</top_statement_list>"
    );}
    void test_statementWhile() { TEST_PHP_PARSER(
        "<?php while(1) {echo 'x';}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"26\" endLineNr=\"1\" lineNr=\"1\">"
        "  <while columnNr=\"12\" endColumnNr=\"26\" endLineNr=\"1\" lineNr=\"1\">"
        "    <lnumber columnNr=\"12\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "    <inner_statement_list columnNr=\"15\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "      <echo columnNr=\"21\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "        <echo_expr_list columnNr=\"21\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "          <constant_encapsed_string columnNr=\"21\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">x</constant_encapsed_string>"
        "        </echo_expr_list>"
        "      </echo>"
        "    </inner_statement_list>"
        "  </while>"
        "</top_statement_list>"
    );}
    void test_statementDoWhile() { TEST_PHP_PARSER(
        "<?php do {echo 'x';} while(1);",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">"
        "  <dowhile columnNr=\"9\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">"
        "    <inner_statement_list columnNr=\"9\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "      <echo columnNr=\"15\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "        <echo_expr_list columnNr=\"15\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "          <constant_encapsed_string columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">x</constant_encapsed_string>"
        "        </echo_expr_list>"
        "      </echo>"
        "    </inner_statement_list>"
        "    <lnumber columnNr=\"27\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "  </dowhile>"
        "</top_statement_list>"
    );}
    void test_statementFor() { TEST_PHP_PARSER(
        "<?php for($i=0;$i<1;$i++) {echo 'x';}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"37\" endLineNr=\"1\" lineNr=\"1\">"
        "  <for columnNr=\"10\" endColumnNr=\"37\" endLineNr=\"1\" lineNr=\"1\">"
        "    <assignment columnNr=\"10\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "      <variable columnNr=\"10\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$i</variable>"
        "      <lnumber columnNr=\"13\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">0</lnumber>"
        "    </assignment>"
        "    <lessthan columnNr=\"15\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "      <variable columnNr=\"15\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">$i</variable>"
        "      <lnumber columnNr=\"18\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "    </lessthan>"
        "    <postinc columnNr=\"20\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "      <variable columnNr=\"20\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">$i</variable>"
        "    </postinc>"
        "    <inner_statement_list columnNr=\"26\" endColumnNr=\"36\" endLineNr=\"1\" lineNr=\"1\">"
        "      <echo columnNr=\"32\" endColumnNr=\"36\" endLineNr=\"1\" lineNr=\"1\">"
        "        <echo_expr_list columnNr=\"32\" endColumnNr=\"36\" endLineNr=\"1\" lineNr=\"1\">"
        "          <constant_encapsed_string columnNr=\"32\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">x</constant_encapsed_string>"
        "        </echo_expr_list>"
        "      </echo>"
        "    </inner_statement_list>"
        "  </for>"
        "</top_statement_list>"
    );}
    void test_statementSwitch() { TEST_PHP_PARSER(
        "<?php switch($i) {case 'x': echo 'y'; default: echo 'z';}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"57\" endLineNr=\"1\" lineNr=\"1\">"
        "  <switch columnNr=\"13\" endColumnNr=\"57\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"13\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">$i</variable>"
        "    <case_list columnNr=\"18\" endColumnNr=\"57\" endLineNr=\"1\" lineNr=\"1\">"
        "      <case columnNr=\"23\" endColumnNr=\"45\" endLineNr=\"1\" lineNr=\"1\">"
        "        <constant_encapsed_string columnNr=\"23\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">x</constant_encapsed_string>"
        "        <inner_statement_list columnNr=\"26\" endColumnNr=\"37\" endLineNr=\"1\" lineNr=\"1\">"
        "          <echo columnNr=\"33\" endColumnNr=\"37\" endLineNr=\"1\" lineNr=\"1\">"
        "            <echo_expr_list columnNr=\"33\" endColumnNr=\"37\" endLineNr=\"1\" lineNr=\"1\">"
        "              <constant_encapsed_string columnNr=\"33\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">y</constant_encapsed_string>"
        "            </echo_expr_list>"
        "          </echo>"
        "        </inner_statement_list>"
        "      </case>"
        "      <default_case columnNr=\"45\" endColumnNr=\"57\" endLineNr=\"1\" lineNr=\"1\">"
        "        <inner_statement_list columnNr=\"45\" endColumnNr=\"56\" endLineNr=\"1\" lineNr=\"1\">"
        "          <echo columnNr=\"52\" endColumnNr=\"56\" endLineNr=\"1\" lineNr=\"1\">"
        "            <echo_expr_list columnNr=\"52\" endColumnNr=\"56\" endLineNr=\"1\" lineNr=\"1\">"
        "              <constant_encapsed_string columnNr=\"52\" endColumnNr=\"53\" endLineNr=\"1\" lineNr=\"1\">z</constant_encapsed_string>"
        "            </echo_expr_list>"
        "          </echo>"
        "        </inner_statement_list>"
        "      </default_case>"
        "    </case_list>"
        "  </switch>"
        "</top_statement_list>"
    );}
    void test_statementBreak() { TEST_PHP_PARSER(
        "<?php break;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">"
        "  <break columnNr=\"11\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\"/>"
        "</top_statement_list>"
    );}
    void test_statementContinue() { TEST_PHP_PARSER(
        "<?php continue;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <continue columnNr=\"14\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\"/>"
        "</top_statement_list>"
    );}
    void test_statementReturn() { TEST_PHP_PARSER(
        "<?php function x(){return 2;}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"29\" endLineNr=\"1\" lineNr=\"1\">"
        "  <function_declaration columnNr=\"15\" endColumnNr=\"29\" endLineNr=\"1\" lineNr=\"1\">"
        "    <is_reference columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" is_reference=\"0\" lineNr=\"1\"/>"
        "    <string columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">x</string>"
        "    <parameter_list columnNr=\"17\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <inner_statement_list columnNr=\"18\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">"
        "      <return columnNr=\"26\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">"
        "        <lnumber columnNr=\"26\" endColumnNr=\"27\" endLineNr=\"1\" lineNr=\"1\">2</lnumber>"
        "      </return>"
        "    </inner_statement_list>"
        "  </function_declaration>"
        "</top_statement_list>"
    );}
    void test_statementStaticVars() { TEST_PHP_PARSER(
        "<?php function x(){static $a = 2; return $a;}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"45\" endLineNr=\"1\" lineNr=\"1\">"
        "  <function_declaration columnNr=\"15\" endColumnNr=\"45\" endLineNr=\"1\" lineNr=\"1\">"
        "    <is_reference columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" is_reference=\"0\" lineNr=\"1\"/>"
        "    <string columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">x</string>"
        "    <parameter_list columnNr=\"17\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <inner_statement_list columnNr=\"18\" endColumnNr=\"44\" endLineNr=\"1\" lineNr=\"1\">"
        "      <static columnNr=\"26\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "        <static_var_list columnNr=\"26\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">"
        "          <variable columnNr=\"26\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">$a                <lnumber columnNr=\"31\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">2</lnumber>"
        "          </variable>"
        "        </static_var_list>"
        "      </static>"
        "      <return columnNr=\"41\" endColumnNr=\"44\" endLineNr=\"1\" lineNr=\"1\">"
        "        <variable columnNr=\"41\" endColumnNr=\"43\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "      </return>"
        "    </inner_statement_list>"
        "  </function_declaration>"
        "</top_statement_list>"
    );}
    void test_statementForeach() { TEST_PHP_PARSER(
        "<?php\n foreach($ar as $a=>$v) {\necho 'x';\n}\n",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"4\" lineNr=\"1\">"
        "  <foreach columnNr=\"9\" endColumnNr=\"1\" endLineNr=\"4\" lineNr=\"2\">"
        "    <variable columnNr=\"9\" endColumnNr=\"12\" endLineNr=\"2\" lineNr=\"2\">$ar</variable>"
        "    <variable columnNr=\"16\" endColumnNr=\"18\" endLineNr=\"2\" lineNr=\"2\">$a</variable>"
        "    <foreach_optional_arg columnNr=\"20\" endColumnNr=\"23\" endLineNr=\"2\" lineNr=\"2\">"
        "      <variable columnNr=\"20\" endColumnNr=\"22\" endLineNr=\"2\" lineNr=\"2\">$v</variable>"
        "    </foreach_optional_arg>"
        "    <inner_statement_list columnNr=\"24\" endColumnNr=\"9\" endLineNr=\"3\" lineNr=\"2\">"
        "      <echo columnNr=\"5\" endColumnNr=\"9\" endLineNr=\"3\" lineNr=\"3\">"
        "        <echo_expr_list columnNr=\"5\" endColumnNr=\"9\" endLineNr=\"3\" lineNr=\"3\">"
        "          <constant_encapsed_string columnNr=\"5\" endColumnNr=\"6\" endLineNr=\"3\" lineNr=\"3\">x</constant_encapsed_string>"
        "        </echo_expr_list>"
        "      </echo>"
        "    </inner_statement_list>"
        "  </foreach>"
        "</top_statement_list>"
    );}
    void test_statementForeachRef() { TEST_PHP_PARSER(
        "<?php foreach($ar as $a=>&$v) {echo 'x';}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"41\" endLineNr=\"1\" lineNr=\"1\">"
        "  <foreach columnNr=\"14\" endColumnNr=\"41\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"14\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">$ar</variable>"
        "    <variable columnNr=\"21\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
        "    <foreach_optional_arg columnNr=\"26\" endColumnNr=\"29\" endLineNr=\"1\" lineNr=\"1\">"
        "      <variable columnNr=\"26\" endColumnNr=\"28\" endLineNr=\"1\" is_reference=\"1\" lineNr=\"1\">$v</variable>"
        "    </foreach_optional_arg>"
        "    <inner_statement_list columnNr=\"30\" endColumnNr=\"40\" endLineNr=\"1\" lineNr=\"1\">"
        "      <echo columnNr=\"36\" endColumnNr=\"40\" endLineNr=\"1\" lineNr=\"1\">"
        "        <echo_expr_list columnNr=\"36\" endColumnNr=\"40\" endLineNr=\"1\" lineNr=\"1\">"
        "          <constant_encapsed_string columnNr=\"36\" endColumnNr=\"37\" endLineNr=\"1\" lineNr=\"1\">x</constant_encapsed_string>"
        "        </echo_expr_list>"
        "      </echo>"
        "    </inner_statement_list>"
        "  </foreach>"
        "</top_statement_list>"
    );}
    void test_statementThrow() { TEST_PHP_PARSER(
        "<?php\nthrow new Exception();",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"22\" endLineNr=\"2\" lineNr=\"1\">"
        "  <throw columnNr=\"10\" endColumnNr=\"22\" endLineNr=\"2\" lineNr=\"2\">"
        "    <new columnNr=\"10\" endColumnNr=\"21\" endLineNr=\"2\" lineNr=\"2\">"
        "      <namespace_name columnNr=\"10\" endColumnNr=\"19\" endLineNr=\"2\" lineNr=\"2\">"
        "        <string columnNr=\"10\" endColumnNr=\"19\" endLineNr=\"2\" lineNr=\"2\">Exception</string>"
        "      </namespace_name>"
        "      <function_call_parameter_list columnNr=\"20\" endColumnNr=\"21\" endLineNr=\"2\" lineNr=\"2\"/>"
        "    </new>"
        "  </throw>"
        "</top_statement_list>"
    );}
    void test_statementThyCatchCatch() { TEST_PHP_PARSER(
        "<?php try {echo 'x';} catch (MyException $e) {} catch (Exception $e) {}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"71\" endLineNr=\"1\" lineNr=\"1\">"
        "  <try columnNr=\"10\" endColumnNr=\"71\" endLineNr=\"1\" lineNr=\"1\">"
        "    <inner_statement_list columnNr=\"10\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "      <echo columnNr=\"16\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "        <echo_expr_list columnNr=\"16\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "          <constant_encapsed_string columnNr=\"16\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">x</constant_encapsed_string>"
        "        </echo_expr_list>"
        "      </echo>"
        "    </inner_statement_list>"
        "    <catch columnNr=\"29\" endColumnNr=\"71\" endLineNr=\"1\" lineNr=\"1\">"
        "      <namespace_name columnNr=\"29\" endColumnNr=\"40\" endLineNr=\"1\" lineNr=\"1\">"
        "        <string columnNr=\"29\" endColumnNr=\"40\" endLineNr=\"1\" lineNr=\"1\">MyException</string>"
        "      </namespace_name>"
        "      <variable columnNr=\"41\" endColumnNr=\"43\" endLineNr=\"1\" lineNr=\"1\">$e</variable>"
        "      <inner_statement_list columnNr=\"45\" endColumnNr=\"46\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    </catch>"
        "    <additional_catches columnNr=\"55\" endColumnNr=\"71\" endLineNr=\"1\" lineNr=\"1\">"
        "      <catch columnNr=\"55\" endColumnNr=\"71\" endLineNr=\"1\" lineNr=\"1\">"
        "        <namespace_name columnNr=\"55\" endColumnNr=\"64\" endLineNr=\"1\" lineNr=\"1\">"
        "          <string columnNr=\"55\" endColumnNr=\"64\" endLineNr=\"1\" lineNr=\"1\">Exception</string>"
        "        </namespace_name>"
        "        <variable columnNr=\"65\" endColumnNr=\"67\" endLineNr=\"1\" lineNr=\"1\">$e</variable>"
        "        <inner_statement_list columnNr=\"69\" endColumnNr=\"70\" endLineNr=\"1\" lineNr=\"1\"/>"
        "      </catch>"
        "    </additional_catches>"
        "  </try>"
        "</top_statement_list>"
    );}
    void test_parsingRequire() { TEST_PHP_PARSER(
        "<?php require 'file.php';\n",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "  <require columnNr=\"14\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "    <constant_encapsed_string columnNr=\"14\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">file.php</constant_encapsed_string>"
        "  </require>"
        "</top_statement_list>"
    );}
    void test_classMethod() { TEST_PHP_PARSER(
        "<?php\nclass my_class\n{\n    public function my_method()\n    {\n    }\n}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"7\" lineNr=\"1\">"
        "  <class_declaration columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"7\" lineNr=\"2\">"
        "    <class columnNr=\"0\" endColumnNr=\"0\" endLineNr=\"2\" lineNr=\"2\"/>"
        "    <string columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"2\" lineNr=\"2\">my_class</string>"
        "    <extends columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"3\" lineNr=\"3\"/>"
        "    <implements columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"3\" lineNr=\"3\"/>"
        "    <class_statement_list columnNr=\"0\" endColumnNr=\"5\" endLineNr=\"6\" lineNr=\"3\">"
        "      <METHOD columnNr=\"4\" endColumnNr=\"5\" endLineNr=\"6\" lineNr=\"4\">"
        "        <MODIFIERS columnNr=\"4\" endColumnNr=\"10\" endLineNr=\"4\" lineNr=\"4\">"
        "          <public columnNr=\"4\" endColumnNr=\"10\" endLineNr=\"4\" lineNr=\"4\"/>"
        "        </MODIFIERS>"
        "        <is_reference columnNr=\"20\" endColumnNr=\"29\" endLineNr=\"4\" is_reference=\"0\" lineNr=\"4\"/>"
        "        <string columnNr=\"20\" endColumnNr=\"29\" endLineNr=\"4\" lineNr=\"4\">my_method</string>"
        "        <parameter_list columnNr=\"30\" endColumnNr=\"31\" endLineNr=\"4\" lineNr=\"4\"/>"
        "        <METHOD_BODY columnNr=\"4\" endColumnNr=\"5\" endLineNr=\"6\" lineNr=\"5\">"
        "          <inner_statement_list columnNr=\"4\" endColumnNr=\"5\" endLineNr=\"5\" lineNr=\"5\"/>"
        "        </METHOD_BODY>"
        "      </METHOD>"
        "    </class_statement_list>"
        "  </class_declaration>"
        "</top_statement_list>"
    );}
    void test_classProperty() { TEST_PHP_PARSER(
        "<?php\nclass my_class\n{\n    public $prop = 1;\n}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"5\" lineNr=\"1\">"
        "  <class_declaration columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"5\" lineNr=\"2\">"
        "    <class columnNr=\"0\" endColumnNr=\"0\" endLineNr=\"2\" lineNr=\"2\"/>"
        "    <string columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"2\" lineNr=\"2\">my_class</string>"
        "    <extends columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"3\" lineNr=\"3\"/>"
        "    <implements columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"3\" lineNr=\"3\"/>"
        "    <class_statement_list columnNr=\"0\" endColumnNr=\"21\" endLineNr=\"4\" lineNr=\"3\">"
        "      <PROPERTY columnNr=\"4\" endColumnNr=\"21\" endLineNr=\"4\" lineNr=\"4\">"
        "        <MODIFIERS columnNr=\"4\" endColumnNr=\"10\" endLineNr=\"4\" lineNr=\"4\">"
        "          <public columnNr=\"4\" endColumnNr=\"10\" endLineNr=\"4\" lineNr=\"4\"/>"
        "        </MODIFIERS>"
        "        <class_properties columnNr=\"11\" endColumnNr=\"20\" endLineNr=\"4\" lineNr=\"4\">"
        "          <variable columnNr=\"11\" endColumnNr=\"20\" endLineNr=\"4\" lineNr=\"4\">$prop                <lnumber columnNr=\"19\" endColumnNr=\"20\" endLineNr=\"4\" lineNr=\"4\">1</lnumber>"
        "          </variable>"
        "        </class_properties>"
        "      </PROPERTY>"
        "    </class_statement_list>"
        "  </class_declaration>"
        "</top_statement_list>"
    );}
    void test_classPropertyList() { TEST_PHP_PARSER(
        "<?php\nclass my_class\n{\n    private final $prop = 1, $x, $y = array('c'=>3,);\n}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"5\" lineNr=\"1\">"
        "  <class_declaration columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"5\" lineNr=\"2\">"
        "    <class columnNr=\"0\" endColumnNr=\"0\" endLineNr=\"2\" lineNr=\"2\"/>"
        "    <string columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"2\" lineNr=\"2\">my_class</string>"
        "    <extends columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"3\" lineNr=\"3\"/>"
        "    <implements columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"3\" lineNr=\"3\"/>"
        "    <class_statement_list columnNr=\"0\" endColumnNr=\"53\" endLineNr=\"4\" lineNr=\"3\">"
        "      <PROPERTY columnNr=\"4\" endColumnNr=\"53\" endLineNr=\"4\" lineNr=\"4\">"
        "        <MODIFIERS columnNr=\"4\" endColumnNr=\"17\" endLineNr=\"4\" lineNr=\"4\">"
        "          <private columnNr=\"4\" endColumnNr=\"11\" endLineNr=\"4\" lineNr=\"4\"/>"
        "          <final columnNr=\"12\" endColumnNr=\"17\" endLineNr=\"4\" lineNr=\"4\"/>"
        "        </MODIFIERS>"
        "        <class_properties columnNr=\"18\" endColumnNr=\"52\" endLineNr=\"4\" lineNr=\"4\">"
        "          <variable columnNr=\"18\" endColumnNr=\"27\" endLineNr=\"4\" lineNr=\"4\">$prop                <lnumber columnNr=\"26\" endColumnNr=\"27\" endLineNr=\"4\" lineNr=\"4\">1</lnumber>"
        "          </variable>"
        "          <variable columnNr=\"29\" endColumnNr=\"31\" endLineNr=\"4\" lineNr=\"4\">$x</variable>"
        "          <variable columnNr=\"33\" endColumnNr=\"52\" endLineNr=\"4\" lineNr=\"4\">$y                <array columnNr=\"44\" endColumnNr=\"52\" endLineNr=\"4\" lineNr=\"4\">"
        "              <array_pair_list columnNr=\"44\" endColumnNr=\"51\" endLineNr=\"4\" lineNr=\"4\">"
        "                <array_pair columnNr=\"44\" endColumnNr=\"51\" endLineNr=\"4\" lineNr=\"4\">"
        "                  <array_key columnNr=\"44\" endColumnNr=\"51\" endLineNr=\"4\" lineNr=\"4\">"
        "                    <constant_encapsed_string columnNr=\"44\" endColumnNr=\"45\" endLineNr=\"4\" lineNr=\"4\">c</constant_encapsed_string>"
        "                  </array_key>"
        "                  <array_value columnNr=\"49\" endColumnNr=\"51\" endLineNr=\"4\" lineNr=\"4\">"
        "                    <lnumber columnNr=\"49\" endColumnNr=\"50\" endLineNr=\"4\" lineNr=\"4\">3</lnumber>"
        "                  </array_value>"
        "                </array_pair>"
        "              </array_pair_list>"
        "            </array>"
        "          </variable>"
        "        </class_properties>"
        "      </PROPERTY>"
        "    </class_statement_list>"
        "  </class_declaration>"
        "</top_statement_list>"
    );}
    void test_classConstant() { TEST_PHP_PARSER(
        "<?php\nclass my_class\n{\n    const MYCONST = 1, OTHER=2;\n}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"5\" lineNr=\"1\">"
        "  <class_declaration columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"5\" lineNr=\"2\">"
        "    <class columnNr=\"0\" endColumnNr=\"0\" endLineNr=\"2\" lineNr=\"2\"/>"
        "    <string columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"2\" lineNr=\"2\">my_class</string>"
        "    <extends columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"3\" lineNr=\"3\"/>"
        "    <implements columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"3\" lineNr=\"3\"/>"
        "    <class_statement_list columnNr=\"0\" endColumnNr=\"30\" endLineNr=\"4\" lineNr=\"3\">"
        "      <class_constants columnNr=\"10\" endColumnNr=\"30\" endLineNr=\"4\" lineNr=\"4\">"
        "        <string columnNr=\"10\" endColumnNr=\"21\" endLineNr=\"4\" lineNr=\"4\">MYCONST              <lnumber columnNr=\"20\" endColumnNr=\"21\" endLineNr=\"4\" lineNr=\"4\">1</lnumber>"
        "        </string>"
        "        <string columnNr=\"23\" endColumnNr=\"30\" endLineNr=\"4\" lineNr=\"4\">OTHER              <lnumber columnNr=\"29\" endColumnNr=\"30\" endLineNr=\"4\" lineNr=\"4\">2</lnumber>"
        "        </string>"
        "      </class_constants>"
        "    </class_statement_list>"
        "  </class_declaration>"
        "</top_statement_list>"
    );}
    void test_methodCallWithNegativeNumber() { TEST_PHP_PARSER(
        "<?php $object->method(-2);",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"26\" endLineNr=\"1\" lineNr=\"1\">"
        "  <object_operator columnNr=\"6\" endColumnNr=\"26\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$object</variable>"
        "    <string columnNr=\"15\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">method</string>"
        "    <method_or_not columnNr=\"23\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "      <function_call_parameter_list columnNr=\"23\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\">"
        "        <negative columnNr=\"23\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "          <lnumber columnNr=\"23\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">2</lnumber>"
        "        </negative>"
        "      </function_call_parameter_list>"
        "    </method_or_not>"
        "    <variable_properties columnNr=\"24\" endColumnNr=\"25\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </object_operator>"
        "</top_statement_list>"
    );}
    void test_negativeNrAsOperation() { TEST_PHP_PARSER(
        "<?php $x-1;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">"
        "  <minus columnNr=\"6\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$x</variable>"
        "    <lnumber columnNr=\"9\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "  </minus>"
        "</top_statement_list>"
    );}
    void test_addToArray() { TEST_PHP_PARSER(
        "<?php $array[] = $x;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"6\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "    <offset columnNr=\"6\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\">"
        "      <variable columnNr=\"6\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\">$array</variable>"
        "      <empty_offset columnNr=\"13\" endColumnNr=\"14\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    </offset>"
        "    <variable columnNr=\"17\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">$x</variable>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_exitWithoutParameters() { TEST_PHP_PARSER(
        "<?php exit;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">"
        "  <exit columnNr=\"10\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">"
        "    <VOID columnNr=\"10\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </exit>"
        "</top_statement_list>"
    );}
    void test_functionParametersDefinitionAllowsDefaults() { TEST_PHP_PARSER(
        "<?php function method($parameter, $type = null) {}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"50\" endLineNr=\"1\" lineNr=\"1\">"
        "  <function_declaration columnNr=\"15\" endColumnNr=\"50\" endLineNr=\"1\" lineNr=\"1\">"
        "    <is_reference columnNr=\"15\" endColumnNr=\"21\" endLineNr=\"1\" is_reference=\"0\" lineNr=\"1\"/>"
        "    <string columnNr=\"15\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">method</string>"
        "    <parameter_list columnNr=\"22\" endColumnNr=\"47\" endLineNr=\"1\" lineNr=\"1\">"
        "      <parameter columnNr=\"22\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "        <__ANY_CLASS_TYPE__ columnNr=\"22\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\"/>"
        "        <variable columnNr=\"22\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">$parameter</variable>"
        "      </parameter>"
        "      <parameter columnNr=\"34\" endColumnNr=\"47\" endLineNr=\"1\" lineNr=\"1\">"
        "        <__ANY_CLASS_TYPE__ columnNr=\"34\" endColumnNr=\"39\" endLineNr=\"1\" lineNr=\"1\"/>"
        "        <variable columnNr=\"34\" endColumnNr=\"39\" endLineNr=\"1\" lineNr=\"1\">$type</variable>"
        "        <namespace_name columnNr=\"42\" endColumnNr=\"46\" endLineNr=\"1\" lineNr=\"1\">"
        "          <string columnNr=\"42\" endColumnNr=\"46\" endLineNr=\"1\" lineNr=\"1\">null</string>"
        "        </namespace_name>"
        "      </parameter>"
        "    </parameter_list>"
        "    <inner_statement_list columnNr=\"48\" endColumnNr=\"49\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </function_declaration>"
        "</top_statement_list>"
    );}
    void test_functionParametersDefinitionAllowsReference() { TEST_PHP_PARSER(
        "<?php function method($parameter, &$variable) {}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"48\" endLineNr=\"1\" lineNr=\"1\">"
        "  <function_declaration columnNr=\"15\" endColumnNr=\"48\" endLineNr=\"1\" lineNr=\"1\">"
        "    <is_reference columnNr=\"15\" endColumnNr=\"21\" endLineNr=\"1\" is_reference=\"0\" lineNr=\"1\"/>"
        "    <string columnNr=\"15\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">method</string>"
        "    <parameter_list columnNr=\"22\" endColumnNr=\"45\" endLineNr=\"1\" lineNr=\"1\">"
        "      <parameter columnNr=\"22\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "        <__ANY_CLASS_TYPE__ columnNr=\"22\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\"/>"
        "        <variable columnNr=\"22\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">$parameter</variable>"
        "      </parameter>"
        "      <parameter columnNr=\"34\" endColumnNr=\"45\" endLineNr=\"1\" is_reference=\"1\" lineNr=\"1\">"
        "        <__ANY_CLASS_TYPE__ columnNr=\"34\" endColumnNr=\"35\" endLineNr=\"1\" lineNr=\"1\"/>"
        "        <variable columnNr=\"35\" endColumnNr=\"44\" endLineNr=\"1\" lineNr=\"1\">$variable</variable>"
        "      </parameter>"
        "    </parameter_list>"
        "    <inner_statement_list columnNr=\"46\" endColumnNr=\"47\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </function_declaration>"
        "</top_statement_list>"
    );}
    void test_instantiateObjectWithoutParameters() { TEST_PHP_PARSER(
        "<?php new classname;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "  <new columnNr=\"10\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\">"
        "    <namespace_name columnNr=\"10\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">"
        "      <string columnNr=\"10\" endColumnNr=\"19\" endLineNr=\"1\" lineNr=\"1\">classname</string>"
        "    </namespace_name>"
        "    <VOID columnNr=\"19\" endColumnNr=\"20\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </new>"
        "</top_statement_list>"
    );}
    void test_arrayDefinitionInsideForeach() { TEST_PHP_PARSER(
        "<?php foreach(array('x') as $x);",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">"
        "  <foreach columnNr=\"20\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">"
        "    <array columnNr=\"20\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "      <array_pair_list columnNr=\"20\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "        <array_pair columnNr=\"20\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "          <array_key columnNr=\"23\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\"/>"
        "          <array_value columnNr=\"20\" endColumnNr=\"24\" endLineNr=\"1\" lineNr=\"1\">"
        "            <constant_encapsed_string columnNr=\"20\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">x</constant_encapsed_string>"
        "          </array_value>"
        "        </array_pair>"
        "      </array_pair_list>"
        "    </array>"
        "    <variable columnNr=\"28\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">$x</variable>"
        "    <foreach_optional_arg columnNr=\"30\" endColumnNr=\"31\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <empty statement columnNr=\"31\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </foreach>"
        "</top_statement_list>"
    );}
    void test_negativeConstant() { TEST_PHP_PARSER(
        "<?php class classname { const constname = -1; }",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"47\" endLineNr=\"1\" lineNr=\"1\">"
        "  <class_declaration columnNr=\"6\" endColumnNr=\"47\" endLineNr=\"1\" lineNr=\"1\">"
        "    <class columnNr=\"6\" endColumnNr=\"6\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <string columnNr=\"12\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">classname</string>"
        "    <extends columnNr=\"22\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <implements columnNr=\"22\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <class_statement_list columnNr=\"22\" endColumnNr=\"44\" endLineNr=\"1\" lineNr=\"1\">"
        "      <class_constants columnNr=\"30\" endColumnNr=\"44\" endLineNr=\"1\" lineNr=\"1\">"
        "        <string columnNr=\"30\" endColumnNr=\"44\" endLineNr=\"1\" lineNr=\"1\">constname              <negative columnNr=\"43\" endColumnNr=\"44\" endLineNr=\"1\" lineNr=\"1\">"
        "            <lnumber columnNr=\"43\" endColumnNr=\"44\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "          </negative>"
        "        </string>"
        "      </class_constants>"
        "    </class_statement_list>"
        "  </class_declaration>"
        "</top_statement_list>"
    );}
    void test_callMethodFromString() { TEST_PHP_PARSER(
        "<?php $object->$methodName();",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">"
        "  <object_operator columnNr=\"6\" endColumnNr=\"30\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$object</variable>"
        "    <variable columnNr=\"16\" endColumnNr=\"27\" endLineNr=\"1\" lineNr=\"1\">$methodName</variable>"
        "    <method_or_not columnNr=\"28\" endColumnNr=\"29\" endLineNr=\"1\" lineNr=\"1\">"
        "      <function_call_parameter_list columnNr=\"28\" endColumnNr=\"29\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    </method_or_not>"
        "    <variable_properties columnNr=\"28\" endColumnNr=\"29\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </object_operator>"
        "</top_statement_list>"
    );}
    void test_callMethodFromStringWrapped() { TEST_PHP_PARSER(
        "<?php $object->{$methodName}();",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">"
        "  <object_operator columnNr=\"6\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"6\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">$object</variable>"
        "    <variable columnNr=\"17\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">$methodName</variable>"
        "    <method_or_not columnNr=\"30\" endColumnNr=\"31\" endLineNr=\"1\" lineNr=\"1\">"
        "      <function_call_parameter_list columnNr=\"30\" endColumnNr=\"31\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    </method_or_not>"
        "    <variable_properties columnNr=\"30\" endColumnNr=\"31\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </object_operator>"
        "</top_statement_list>"
    );}
    void test_listWithEmptyVariables() { TEST_PHP_PARSER(
        "<?php list(,,$x) = $y;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "  <assignment columnNr=\"11\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "    <assignment_list columnNr=\"11\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "      <VOID columnNr=\"11\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\"/>"
        "      <VOID columnNr=\"12\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\"/>"
        "      <variable columnNr=\"13\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">$x</variable>"
        "    </assignment_list>"
        "    <variable columnNr=\"19\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">$y</variable>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_variableRefByVariable() { TEST_PHP_PARSER(
        "<?php $$x;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"9\" endLineNr=\"1\" lineNr=\"1\">"
        "  <simple_indirect_reference columnNr=\"7\" endColumnNr=\"9\" endLineNr=\"1\" lineNr=\"1\">"
        "    <variable columnNr=\"7\" endColumnNr=\"9\" endLineNr=\"1\" lineNr=\"1\">$x</variable>"
        "  </simple_indirect_reference>"
        "</top_statement_list>"
    );}
    void test_whitespaceInHeredocStart() { TEST_PHP_PARSER(
        "<?php \n$x = <<< XXX\nasd\nXXX;\n",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"4\" endLineNr=\"4\" lineNr=\"1\">"
        "  <assignment columnNr=\"0\" endColumnNr=\"4\" endLineNr=\"4\" lineNr=\"2\">"
        "    <variable columnNr=\"0\" endColumnNr=\"2\" endLineNr=\"2\" lineNr=\"2\">$x</variable>"
        "    <encapsed_and_whitespace columnNr=\"0\" endColumnNr=\"3\" endLineNr=\"3\" lineNr=\"3\">asd</encapsed_and_whitespace>"
        "  </assignment>"
        "</top_statement_list>"
    );}
    void test_alternativeIfElseEndifSyntax() { TEST_PHP_PARSER(
        "<?php if (1): echo $x; elseif (2): echo $z; else: echo $y; endif;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"65\" endLineNr=\"1\" lineNr=\"1\">"
        "  <if columnNr=\"10\" endColumnNr=\"65\" endLineNr=\"1\" lineNr=\"1\">"
        "    <lnumber columnNr=\"10\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "    <inner_statement_list columnNr=\"12\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "      <echo columnNr=\"19\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "        <echo_expr_list columnNr=\"19\" endColumnNr=\"22\" endLineNr=\"1\" lineNr=\"1\">"
        "          <variable columnNr=\"19\" endColumnNr=\"21\" endLineNr=\"1\" lineNr=\"1\">$x</variable>"
        "        </echo_expr_list>"
        "      </echo>"
        "    </inner_statement_list>"
        "    <elseif_list columnNr=\"23\" endColumnNr=\"48\" endLineNr=\"1\" lineNr=\"1\">"
        "      <elseif columnNr=\"31\" endColumnNr=\"48\" endLineNr=\"1\" lineNr=\"1\">"
        "        <lnumber columnNr=\"31\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\">2</lnumber>"
        "        <inner_statement_list columnNr=\"33\" endColumnNr=\"43\" endLineNr=\"1\" lineNr=\"1\">"
        "          <echo columnNr=\"40\" endColumnNr=\"43\" endLineNr=\"1\" lineNr=\"1\">"
        "            <echo_expr_list columnNr=\"40\" endColumnNr=\"43\" endLineNr=\"1\" lineNr=\"1\">"
        "              <variable columnNr=\"40\" endColumnNr=\"42\" endLineNr=\"1\" lineNr=\"1\">$z</variable>"
        "            </echo_expr_list>"
        "          </echo>"
        "        </inner_statement_list>"
        "      </elseif>"
        "    </elseif_list>"
        "    <else columnNr=\"48\" endColumnNr=\"64\" endLineNr=\"1\" lineNr=\"1\">"
        "      <inner_statement_list columnNr=\"48\" endColumnNr=\"58\" endLineNr=\"1\" lineNr=\"1\">"
        "        <echo columnNr=\"55\" endColumnNr=\"58\" endLineNr=\"1\" lineNr=\"1\">"
        "          <echo_expr_list columnNr=\"55\" endColumnNr=\"58\" endLineNr=\"1\" lineNr=\"1\">"
        "            <variable columnNr=\"55\" endColumnNr=\"57\" endLineNr=\"1\" lineNr=\"1\">$y</variable>"
        "          </echo_expr_list>"
        "        </echo>"
        "      </inner_statement_list>"
        "    </else>"
        "  </if>"
        "</top_statement_list>"
    );}
    void test_globalsUsage() { TEST_PHP_PARSER(
        "<?php function x() {global $x, $y;}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"35\" endLineNr=\"1\" lineNr=\"1\">"
        "  <function_declaration columnNr=\"15\" endColumnNr=\"35\" endLineNr=\"1\" lineNr=\"1\">"
        "    <is_reference columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" is_reference=\"0\" lineNr=\"1\"/>"
        "    <string columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">x</string>"
        "    <parameter_list columnNr=\"17\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <inner_statement_list columnNr=\"19\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "      <global columnNr=\"27\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
        "        <global_var_list columnNr=\"27\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">"
        "          <variable columnNr=\"27\" endColumnNr=\"29\" endLineNr=\"1\" lineNr=\"1\">$x</variable>"
        "          <variable columnNr=\"31\" endColumnNr=\"33\" endLineNr=\"1\" lineNr=\"1\">$y</variable>"
        "        </global_var_list>"
        "      </global>"
        "    </inner_statement_list>"
        "  </function_declaration>"
        "</top_statement_list>"
    );}
    void test_forWithEmptySpecifications() { TEST_PHP_PARSER(
        "<?php for(;;) ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "  <for columnNr=\"10\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\">"
        "    <VOID columnNr=\"10\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <VOID columnNr=\"11\" endColumnNr=\"12\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <VOID columnNr=\"12\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <empty statement columnNr=\"14\" endColumnNr=\"15\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </for>"
        "</top_statement_list>"
    );}
    void test_hereDocAsArgument() { TEST_PHP_PARSER(
        "<?php array('x'=> <<<XX\nasd\nXX\n) ;",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"4\" lineNr=\"1\">"
        "  <array columnNr=\"12\" endColumnNr=\"1\" endLineNr=\"4\" lineNr=\"1\">"
        "    <array_pair_list columnNr=\"12\" endColumnNr=\"1\" endLineNr=\"4\" lineNr=\"1\">"
        "      <array_pair columnNr=\"12\" endColumnNr=\"1\" endLineNr=\"4\" lineNr=\"1\">"
        "        <array_key columnNr=\"12\" endColumnNr=\"1\" endLineNr=\"4\" lineNr=\"1\">"
        "          <constant_encapsed_string columnNr=\"12\" endColumnNr=\"13\" endLineNr=\"1\" lineNr=\"1\">x</constant_encapsed_string>"
        "        </array_key>"
        "        <array_value columnNr=\"0\" endColumnNr=\"1\" endLineNr=\"4\" lineNr=\"2\">"
        "          <encapsed_and_whitespace columnNr=\"0\" endColumnNr=\"3\" endLineNr=\"2\" lineNr=\"2\">asd</encapsed_and_whitespace>"
        "        </array_value>"
        "      </array_pair>"
        "    </array_pair_list>"
        "  </array>"
        "</top_statement_list>"
    );}
    void test_danglingElse() { TEST_PHP_PARSER(
        "<?php if(1) if (2) {echo $y;} else {echo $z;}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"45\" endLineNr=\"1\" lineNr=\"1\">"
        "  <if columnNr=\"9\" endColumnNr=\"45\" endLineNr=\"1\" lineNr=\"1\">"
        "    <lnumber columnNr=\"9\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "    <if columnNr=\"16\" endColumnNr=\"45\" endLineNr=\"1\" lineNr=\"1\">"
        "      <lnumber columnNr=\"16\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">2</lnumber>"
        "      <inner_statement_list columnNr=\"19\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">"
        "        <echo columnNr=\"25\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">"
        "          <echo_expr_list columnNr=\"25\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">"
        "            <variable columnNr=\"25\" endColumnNr=\"27\" endLineNr=\"1\" lineNr=\"1\">$y</variable>"
        "          </echo_expr_list>"
        "        </echo>"
        "      </inner_statement_list>"
        "      <elseif_list columnNr=\"44\" endColumnNr=\"45\" endLineNr=\"1\" lineNr=\"1\"/>"
        "      <else columnNr=\"35\" endColumnNr=\"45\" endLineNr=\"1\" lineNr=\"1\">"
        "        <inner_statement_list columnNr=\"35\" endColumnNr=\"44\" endLineNr=\"1\" lineNr=\"1\">"
        "          <echo columnNr=\"41\" endColumnNr=\"44\" endLineNr=\"1\" lineNr=\"1\">"
        "            <echo_expr_list columnNr=\"41\" endColumnNr=\"44\" endLineNr=\"1\" lineNr=\"1\">"
        "              <variable columnNr=\"41\" endColumnNr=\"43\" endLineNr=\"1\" lineNr=\"1\">$z</variable>"
        "            </echo_expr_list>"
        "          </echo>"
        "        </inner_statement_list>"
        "      </else>"
        "    </if>"
        "    <elseif_list columnNr=\"44\" endColumnNr=\"45\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <else columnNr=\"44\" endColumnNr=\"45\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </if>"
        "</top_statement_list>"
    );}
    void test_danglingElseIf() { TEST_PHP_PARSER(
        "<?php if(1) if (2) {echo $y;} elseif (3) {echo $z;}",
        "<top_statement_list columnNr=\"0\" endColumnNr=\"51\" endLineNr=\"1\" lineNr=\"1\">"
        "  <if columnNr=\"9\" endColumnNr=\"51\" endLineNr=\"1\" lineNr=\"1\">"
        "    <lnumber columnNr=\"9\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
        "    <if columnNr=\"16\" endColumnNr=\"51\" endLineNr=\"1\" lineNr=\"1\">"
        "      <lnumber columnNr=\"16\" endColumnNr=\"17\" endLineNr=\"1\" lineNr=\"1\">2</lnumber>"
        "      <inner_statement_list columnNr=\"19\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">"
        "        <echo columnNr=\"25\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">"
        "          <echo_expr_list columnNr=\"25\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">"
        "            <variable columnNr=\"25\" endColumnNr=\"27\" endLineNr=\"1\" lineNr=\"1\">$y</variable>"
        "          </echo_expr_list>"
        "        </echo>"
        "      </inner_statement_list>"
        "      <elseif_list columnNr=\"38\" endColumnNr=\"51\" endLineNr=\"1\" lineNr=\"1\">"
        "        <elseif columnNr=\"38\" endColumnNr=\"51\" endLineNr=\"1\" lineNr=\"1\">"
        "          <lnumber columnNr=\"38\" endColumnNr=\"39\" endLineNr=\"1\" lineNr=\"1\">3</lnumber>"
        "          <inner_statement_list columnNr=\"41\" endColumnNr=\"50\" endLineNr=\"1\" lineNr=\"1\">"
        "            <echo columnNr=\"47\" endColumnNr=\"50\" endLineNr=\"1\" lineNr=\"1\">"
        "              <echo_expr_list columnNr=\"47\" endColumnNr=\"50\" endLineNr=\"1\" lineNr=\"1\">"
        "                <variable columnNr=\"47\" endColumnNr=\"49\" endLineNr=\"1\" lineNr=\"1\">$z</variable>"
        "              </echo_expr_list>"
        "            </echo>"
        "          </inner_statement_list>"
        "        </elseif>"
        "      </elseif_list>"
        "      <else columnNr=\"50\" endColumnNr=\"51\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    </if>"
        "    <elseif_list columnNr=\"50\" endColumnNr=\"51\" endLineNr=\"1\" lineNr=\"1\"/>"
        "    <else columnNr=\"50\" endColumnNr=\"51\" endLineNr=\"1\" lineNr=\"1\"/>"
        "  </if>"
        "</top_statement_list>"
    );}










    void test_ifPartialAstIsReturned_onErrorAtTopLevel() {
        QSharedPointer<IonPhp::Private::ParserResult> ret =
            QSharedPointer<IonPhp::Private::ParserResult>(
                IonPhp::Private::PhpParser().parseString("<?php $a=1; foreach($ar as $a=>$v) x';}")
            );
        if (ret->success) {
            QFAIL("there should have been an error parsing the input");
        }
        if (!ret->getRoot()) {
            QFAIL("root object is missing");
        }
        QString expectedAst(
            "<top_statement_list columnNr=\"0\" endColumnNr=\"40\" endLineNr=\"1\" lineNr=\"1\">"
            "  <assignment columnNr=\"6\" endColumnNr=\"11\" endLineNr=\"1\" lineNr=\"1\">"
            "    <variable columnNr=\"6\" endColumnNr=\"8\" endLineNr=\"1\" lineNr=\"1\">$a</variable>"
            "    <lnumber columnNr=\"9\" endColumnNr=\"10\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
            "  </assignment>"
            "  <__PARSE_ERROR columnNr=\"36\" endColumnNr=\"40\" endLineNr=\"1\" lineNr=\"1\"/>"
            "</top_statement_list>"
        );
        QCOMPARE_3(
            ret->getRoot()->toString().replace(" ", "").replace("\n", "").replace("\r", "").replace("<?xmlversion=\"1.0\"?>", ""),
            expectedAst.replace(" ", "").replace("\n", "").replace("\r", ""),
            PRINT_FOR_CODE(ret->getRoot()->toString())
        );
    }

    void test_ifPartialAstIsReturnedForFunction_onError() {
        QSharedPointer<IonPhp::Private::ParserResult> ret =
            QSharedPointer<IonPhp::Private::ParserResult>(
                IonPhp::Private::PhpParser().parseString("<?php function a(){ $aa = 1; 3aa ; }")
            );
        if (ret->success) {
            DEBUG_MSG(ret->getRoot()->toString());
            QFAIL("there should have been an error parsing the input");
        }
        DEBUG_MSG(ret->errors.front().getMessage());
        if (!ret->getRoot()) {
            QFAIL("root object is missing");
        }
        QString expectedAst(
            "<top_statement_list columnNr=\"0\" endColumnNr=\"36\" endLineNr=\"1\" lineNr=\"1\">"
            "  <function_declaration columnNr=\"15\" endColumnNr=\"36\" endLineNr=\"1\" lineNr=\"1\">"
            "    <is_reference columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" is_reference=\"0\" lineNr=\"1\"/>"
            "    <string columnNr=\"15\" endColumnNr=\"16\" endLineNr=\"1\" lineNr=\"1\">a</string>"
            "    <parameter_list columnNr=\"17\" endColumnNr=\"18\" endLineNr=\"1\" lineNr=\"1\"/>"
            "    <inner_statement_list columnNr=\"18\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\">"
            "      <assignment columnNr=\"20\" endColumnNr=\"28\" endLineNr=\"1\" lineNr=\"1\">"
            "        <variable columnNr=\"20\" endColumnNr=\"23\" endLineNr=\"1\" lineNr=\"1\">$aa</variable>"
            "        <lnumber columnNr=\"26\" endColumnNr=\"27\" endLineNr=\"1\" lineNr=\"1\">1</lnumber>"
            "      </assignment>"
            "      <__PARSE_ERROR columnNr=\"30\" endColumnNr=\"32\" endLineNr=\"1\" lineNr=\"1\"/>"
            "      <empty statement columnNr=\"33\" endColumnNr=\"34\" endLineNr=\"1\" lineNr=\"1\"/>"
            "    </inner_statement_list>"
            "  </function_declaration>"
            "</top_statement_list>"
        );
        QCOMPARE_3(
            ret->getRoot()->toString().replace(" ", "").replace("\n", "").replace("\r", "").replace("<?xmlversion=\"1.0\"?>", ""),
            expectedAst.replace(" ", "").replace("\n", "").replace("\r", ""),
            PRINT_FOR_CODE(ret->getRoot()->toString())
        );
    }
};

}

#endif // PHPPARSERTEST_H

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

#define PRINT(QSTR) std::cout << QSTR.toStdString() << std::endl;
#define TEST_PHP_PARSER(CODE, ASTSTR) { \
    IonPhp::Private::ParserResult ret = IonPhp::Private::phpParser().parseString(CODE); \
    if (!ret.success) { \
        QFAIL(ret.error.message.toLatin1()); \
    } \
    QCOMPARE_3( \
        ret.root->toString().replace(" ", "").replace("\n", "").replace("\r", "").replace("<?xmlversion=\"1.0\"?>", ""), \
        QString(ASTSTR).replace(" ", "").replace("\n", "").replace("\r", ""), \
        PRINT(ret.root->toString()) \
    ); \
}


class PhpParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_failsOnPhpError() {
        IonPhp::Private::ParserResult ret = IonPhp::Private::phpParser().parseString("<?php asd");
        QCOMPARE(ret.success, false);
    }
    void test_openCloseNoEnd() { TEST_PHP_PARSER(
        "<?php ?><?php ?><?php ",
        "<top_statement_list>"
        "  <empty statement/>"
        "  <empty statement/>"
        "</top_statement_list>"
    ); }
    void test_openCloseEnd() { TEST_PHP_PARSER(
        "<?php ?><?php ?><?php ?>",
        "<top_statement_list><emptystatement/><emptystatement/><emptystatement/></top_statement_list>"
    ); }
    void test_inlineHtml() { TEST_PHP_PARSER(
        "<?php ?>asd1<?php ?>asd2",
        "<top_statement_list>"
        "  <empty statement/>"
        "  <inline_html columnNr=\"8\" lineNr=\"0\">asd1</inline_html>"
        "  <empty statement/>"
        "  <inline_html columnNr=\"20\" lineNr=\"0\">asd2</inline_html>"
        "</top_statement_list>"
    ); }
    void test_scriptOpenTag() { TEST_PHP_PARSER(
        " <?<%<?php ?>asd?%<script language=\"php\">echo $d</script> asd",
        "<top_statement_list>"
        "  <inline_html columnNr=\"0\" lineNr=\"0\"> &lt;?&lt;%</inline_html>"
        "  <empty statement/>"
        "  <inline_html columnNr=\"13\" lineNr=\"0\">asd?%</inline_html>"
        "  <echo>"
        "    <echo_expr_list>"
        "      <variable columnNr=\"46\" lineNr=\"0\">$d</variable>"
        "    </echo_expr_list>"
        "  </echo>"
        "  <inline_html columnNr=\"57\" lineNr=\"0\"> asd</inline_html>"
        "</top_statement_list>"
    ); }
    void test_scriptOpenTagWOQuotes() { TEST_PHP_PARSER(
        "<?php \n?>asd<script \nlanguage=php>echo $d</script> asd",
        "<top_statement_list>"
        "  <empty statement/>"
        "  <inline_html columnNr=\"2\" lineNr=\"1\">asd</inline_html>"
        "  <echo>"
        "    <echo_expr_list>"
        "      <variable columnNr=\"18\" lineNr=\"2\">$d</variable>"
        "    </echo_expr_list>"
        "  </echo>"
        "  <inline_html columnNr=\"29\" lineNr=\"2\"> asd</inline_html>"
        "</top_statement_list>"
    ); }
    void test_scriptOpenTagWrong() { TEST_PHP_PARSER(
        "<?php ?>asd<script language=notphp>echo $d</script> asd",
        "<top_statement_list>"
        "  <empty statement/>"
        "  <inline_html columnNr=\"8\" lineNr=\"0\">asd&lt;script language=notphp&gt;echo $d&lt;/script&gt; asd</inline_html>"
        "</top_statement_list>"
    ); }
    void test_variableAssignmentFromFncCall() { TEST_PHP_PARSER(
        "<?php $a = moo();",
        "<top_statement_list>"
        "  <assignment>"
        "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
        "    <function_call>"
        "      <namespace_name>"
        "        <string columnNr=\"11\" lineNr=\"0\">moo</string>"
        "      </namespace_name>"
        "      <function_call_parameter_list/>"
        "    </function_call>"
        "  </assignment>"
        "</top_statement_list>"
    ); }
    void test_variableAssignmentFromFncCallWithParams() { TEST_PHP_PARSER(
        "<?php $a = moo(1, $s);",
        "<top_statement_list>"
        "  <assignment>"
        "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
        "    <function_call>"
        "      <namespace_name>"
        "        <string columnNr=\"11\" lineNr=\"0\">moo</string>"
        "      </namespace_name>"
        "      <function_call_parameter_list>"
        "        <lnumber columnNr=\"15\" lineNr=\"0\">1</lnumber>"
        "        <variable columnNr=\"18\" lineNr=\"0\">$s</variable>"
        "      </function_call_parameter_list>"
        "    </function_call>"
        "  </assignment>"
        "</top_statement_list>"
    ); }
    void test_functionDefinition() {
        TEST_PHP_PARSER(
            "<?php function myfnc() {}",
            "<top_statement_list>"
            "  <function_declaration>"
            "    <is_reference is_reference=\"0\"/>"
            "    <string columnNr=\"15\" lineNr=\"0\">myfnc</string>"
            "    <parameter_list/>"
            "    <inner_statement_list/>"
            "  </function_declaration>"
            "</top_statement_list>"
        );
    }
    void test_listDefinition() { TEST_PHP_PARSER(
        "<?php list($a, $b) = $c;",
        "<top_statement_list>"
        "  <assignment>"
        "    <assignment_list>"
        "      <variable columnNr=\"11\" lineNr=\"0\">$a</variable>"
        "      <variable columnNr=\"15\" lineNr=\"0\">$b</variable>"
        "    </assignment_list>"
        "    <variable columnNr=\"21\" lineNr=\"0\">$c</variable>"
        "  </assignment>"
        "</top_statement_list>"
    ); }
    void test_assignDefinition() { TEST_PHP_PARSER(
        "<?php $a = $c;",
        "<top_statement_list>"
        "  <assignment>"
        "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
        "    <variable columnNr=\"11\" lineNr=\"0\">$c</variable>"
        "  </assignment>"
        "</top_statement_list>"
    ); }
    void test_assignRefDefinition() { TEST_PHP_PARSER(
        "<?php $a = &$c;",
        "<top_statement_list>"
        "  <assignment is_reference=\"1\">"
        "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
        "    <variable columnNr=\"12\" lineNr=\"0\">$c</variable>"
        "  </assignment>"
        "</top_statement_list>"
    ); }
    void test_assignRefNewClassDefinition() { TEST_PHP_PARSER(
        "<?php $a = & new asd;",
        "<top_statement_list>"
        "  <assignment is_reference=\"1\">"
        "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
        "    <new>"
        "      <namespace_name>"
        "        <string columnNr=\"17\" lineNr=\"0\">asd</string>"
        "      </namespace_name>"
        "      <VOID/>"
        "    </new>"
        "  </assignment>"
        "</top_statement_list>"
    ); }
    void test_assignRefNewClassWithParamsDefinition() { TEST_PHP_PARSER(
        "<?php $a = & new asd(1);",
        "<top_statement_list>"
        "  <assignment is_reference=\"1\">"
        "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
        "    <new>"
        "      <namespace_name>"
        "        <string columnNr=\"17\" lineNr=\"0\">asd</string>"
        "      </namespace_name>"
        "      <function_call_parameter_list>"
        "        <lnumber columnNr=\"21\" lineNr=\"0\">1</lnumber>"
        "      </function_call_parameter_list>"
        "    </new>"
        "  </assignment>"
        "</top_statement_list>"
    ); }
    void test_newClassDefinition() { TEST_PHP_PARSER(
        "<?php new asd(1, '2');",
        "<top_statement_list>"
        "  <new>"
        "    <namespace_name>"
        "      <string columnNr=\"10\" lineNr=\"0\">asd</string>"
        "    </namespace_name>"
        "    <function_call_parameter_list>"
        "      <lnumber columnNr=\"14\" lineNr=\"0\">1</lnumber>"
        "      <constant_encapsed_string columnNr=\"17\" lineNr=\"0\">2</constant_encapsed_string>"
        "    </function_call_parameter_list>"
        "  </new>"
        "</top_statement_list>"
    ); }
    void test_cloneDefinition() { TEST_PHP_PARSER(
        "<?php clone $a;",
        "<top_statement_list>"
        "  <clone>"
        "    <variable columnNr=\"12\" lineNr=\"0\">$a</variable>"
        "  </clone>"
        "</top_statement_list>"
    ); }
    void test_assignPlusDefinition() { TEST_PHP_PARSER(
        "<?php $a+=$b;",
        "<top_statement_list>"
        "  <plus_equal>"
        "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
        "    <variable columnNr=\"10\" lineNr=\"0\">$b</variable>"
        "  </plus_equal>"
        "</top_statement_list>"
    ); }
    void test_assignMinusDefinition() { TEST_PHP_PARSER(
        "<?php $a-=$b;",
        "<top_statement_list>"
        "  <minus_equal>"
        "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
        "    <variable columnNr=\"10\" lineNr=\"0\">$b</variable>"
        "  </minus_equal>"
        "</top_statement_list>"
    ); }
    void test_assignMulDefinition() { TEST_PHP_PARSER(
        "<?php $a*=$b;",
        "<top_statement_list>"
        "  <mul_equal>"
        "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
        "    <variable columnNr=\"10\" lineNr=\"0\">$b</variable>"
        "  </mul_equal>"
        "</top_statement_list>"
    ); }
    void test_assignDivDefinition() { TEST_PHP_PARSER(
        "<?php $a/=$b;",
        "<top_statement_list>"
        "  <div_equal>"
        "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
        "    <variable columnNr=\"10\" lineNr=\"0\">$b</variable>"
        "  </div_equal>"
        "</top_statement_list>"
    ); }
    void test_assignConcatDefinition() { TEST_PHP_PARSER(
        "<?php $a.=$b;",
                    "<top_statement_list>"
                    "  <concat_equal>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"10\" lineNr=\"0\">$b</variable>"
                    "  </concat_equal>"
                    "</top_statement_list>"
    ); }
    void test_assignModDefinition() { TEST_PHP_PARSER(
        "<?php $a%=$b;",
                    "<top_statement_list>"
                    "  <mod_equal>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"10\" lineNr=\"0\">$b</variable>"
                    "  </mod_equal>"
                    "</top_statement_list>"
    ); }
    void test_assignAndDefinition() { TEST_PHP_PARSER(
        "<?php $a&=$b;",
                    "<top_statement_list>"
                    "  <and_equal>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"10\" lineNr=\"0\">$b</variable>"
                    "  </and_equal>"
                    "</top_statement_list>"
    ); }
    void test_assignOrDefinition() { TEST_PHP_PARSER(
        "<?php $a|=$b;",
                    "<top_statement_list>"
                    "  <or_equal>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"10\" lineNr=\"0\">$b</variable>"
                    "  </or_equal>"
                    "</top_statement_list>"
    ); }
    void test_assignXorDefinition() { TEST_PHP_PARSER(
        "<?php $a^=$b;",
                    "<top_statement_list>"
                    "  <xor_equal>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"10\" lineNr=\"0\">$b</variable>"
                    "  </xor_equal>"
                    "</top_statement_list>"
    ); }
    void test_assignShLeftDefinition() { TEST_PHP_PARSER(
        "<?php $a<<=$b;",
                    "<top_statement_list>"
                    "  <sl_equal>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </sl_equal>"
                    "</top_statement_list>"
    ); }
    void test_assignShRightDefinition() { TEST_PHP_PARSER(
        "<?php $a>>=$b;",
                    "<top_statement_list>"
                    "  <sr_equal>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </sr_equal>"
                    "</top_statement_list>"
    ); }
    void test_postIncDefinition() { TEST_PHP_PARSER(
        "<?php $a++;",
                    "<top_statement_list>"
                    "  <postinc>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "  </postinc>"
                    "</top_statement_list>"
    ); }
    void testPhpParserp_preIncDefinition() { TEST_PHP_PARSER(
        "<?php ++$b;",
                    "<top_statement_list>"
                    "  <preinc>"
                    "    <variable columnNr=\"8\" lineNr=\"0\">$b</variable>"
                    "  </preinc>"
                    "</top_statement_list>"
    ); }
    void test_postDecDefinition() { TEST_PHP_PARSER(
        "<?php $a--;",
                    "<top_statement_list>"
                    "  <postdec>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "  </postdec>"
                    "</top_statement_list>"
    ); }
    void test_preDecDefinition() { TEST_PHP_PARSER(
        "<?php --$a;",
                    "<top_statement_list>"
                    "  <predec>"
                    "    <variable columnNr=\"8\" lineNr=\"0\">$a</variable>"
                    "  </predec>"
                    "</top_statement_list>"
    ); }
    void test_boolOrDefinition() { TEST_PHP_PARSER(
        "<?php $a || $b;",
                    "<top_statement_list>"
                    "  <boolean_or>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"12\" lineNr=\"0\">$b</variable>"
                    "  </boolean_or>"
                    "</top_statement_list>"
    ); }
    void test_boolAndDefinition() { TEST_PHP_PARSER(
        "<?php $a && $b;",
                    "<top_statement_list>"
                    "  <boolean_and>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"12\" lineNr=\"0\">$b</variable>"
                    "  </boolean_and>"
                    "</top_statement_list>"
    ); }
    void test_logicalOrDefinition() { TEST_PHP_PARSER(
        "<?php $a or $b;",
                    "<top_statement_list>"
                    "  <logical_or>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"12\" lineNr=\"0\">$b</variable>"
                    "  </logical_or>"
                    "</top_statement_list>"
    ); }
    void test_logicalAndDefinition() { TEST_PHP_PARSER(
        "<?php $a and $b;",
                    "<top_statement_list>"
                    "  <logical_and>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"13\" lineNr=\"0\">$b</variable>"
                    "  </logical_and>"
                    "</top_statement_list>"
    ); }
    void test_logicalXorDefinition() { TEST_PHP_PARSER(
        "<?php $a xor $b;",
                    "<top_statement_list>"
                    "  <logical_xor>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"13\" lineNr=\"0\">$b</variable>"
                    "  </logical_xor>"
                    "</top_statement_list>"
    ); }
    void test_orDefinition() { TEST_PHP_PARSER(
        "<?php $a | $b;",
                    "<top_statement_list>"
                    "  <binary_or>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </binary_or>"
                    "</top_statement_list>"
    ); }
    void test_andDefinition() { TEST_PHP_PARSER(
        "<?php $a & $b;",
                    "<top_statement_list>"
                    "  <binary_and>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </binary_and>"
                    "</top_statement_list>"
    ); }
    void test_xorDefinition() { TEST_PHP_PARSER(
        "<?php $a ^ $b;",
                    "<top_statement_list>"
                    "  <binary_xor>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </binary_xor>"
                    "</top_statement_list>"
    ); }
    void tesconcatDefinition() { TEST_PHP_PARSER(
        "<?php $a . $b;",
                    "<top_statement_list>"
                    "  <concat>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </concat>"
                    "</top_statement_list>"
    ); }
    void tesplusDefinition() { TEST_PHP_PARSER(
        "<?php $a + $b;",
                    "<top_statement_list>"
                    "  <plus>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </plus>"
                    "</top_statement_list>"
    ); }
    void test_minusDefinition() { TEST_PHP_PARSER(
        "<?php $a - $b;",
                    "<top_statement_list>"
                    "  <minus>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </minus>"
                    "</top_statement_list>"
    ); }
    void test_mulDefinition() { TEST_PHP_PARSER(
        "<?php $a * $b;",
                    "<top_statement_list>"
                    "  <mul>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </mul>"
                    "</top_statement_list>"
    ); }
    void test_divDefinition() { TEST_PHP_PARSER(
        "<?php $a / $b;",
                    "<top_statement_list>"
                    "  <div>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </div>"
                    "</top_statement_list>"
    ); }
    void test_modDefinition() { TEST_PHP_PARSER(
        "<?php $a % $b;",
                    "<top_statement_list>"
                    "  <mod>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </mod>"
                    "</top_statement_list>"
    ); }
    void test_shLeftDefinition() { TEST_PHP_PARSER(
        "<?php $a << $b;",
                    "<top_statement_list>"
                    "  <shift_left>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"12\" lineNr=\"0\">$b</variable>"
                    "  </shift_left>"
                    "</top_statement_list>"
    ); }
    void test_shRightDefinition() { TEST_PHP_PARSER(
        "<?php $a >> $b;",
                    "<top_statement_list>"
                    "  <shift_right>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"12\" lineNr=\"0\">$b</variable>"
                    "  </shift_right>"
                    "</top_statement_list>"
    ); }
    void test_negateDefinition() { TEST_PHP_PARSER(
        "<?php !$a;",
                    "<top_statement_list>"
                    "  <logical_negate>"
                    "    <variable columnNr=\"7\" lineNr=\"0\">$a</variable>"
                    "  </logical_negate>"
                    "</top_statement_list>"
    ); }
    void test_inverseDefinition() { TEST_PHP_PARSER(
        "<?php ~$a;",
                    "<top_statement_list>"
                    "  <binary_inverse>"
                    "    <variable columnNr=\"7\" lineNr=\"0\">$a</variable>"
                    "  </binary_inverse>"
                    "</top_statement_list>"
    ); }
    void test_isIdenticalDefinition() { TEST_PHP_PARSER(
        "<?php $a === $b;",
                    "<top_statement_list>"
                    "  <is_identical>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"13\" lineNr=\"0\">$b</variable>"
                    "  </is_identical>"
                    "</top_statement_list>"
    ); }
    void test_isNotIdenticalDefinition() { TEST_PHP_PARSER(
        "<?php $a !== $b;",
                    "<top_statement_list>"
                    "  <is_not_identical>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"13\" lineNr=\"0\">$b</variable>"
                    "  </is_not_identical>"
                    "</top_statement_list>"
    ); }
    void test_isEqualDefinition() { TEST_PHP_PARSER(
        "<?php $a == $b;",
                    "<top_statement_list>"
                    "  <is_equal>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"12\" lineNr=\"0\">$b</variable>"
                    "  </is_equal>"
                    "</top_statement_list>"
    ); }
    void test_isNotEqualDefinition() { TEST_PHP_PARSER(
        "<?php $a != $b;",
                    "<top_statement_list>"
                    "  <is_not_equal>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"12\" lineNr=\"0\">$b</variable>"
                    "  </is_not_equal>"
                    "</top_statement_list>"
    ); }
    void test_lessDefinition() { TEST_PHP_PARSER(
        "<?php $a < $b;",
                    "<top_statement_list>"
                    "  <lessthan>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </lessthan>"
                    "</top_statement_list>"
    ); }
    void test_lessEqDefinition() { TEST_PHP_PARSER(
        "<?php $a <= $b;",
                    "<top_statement_list>"
                    "  <lessthan_equal>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"12\" lineNr=\"0\">$b</variable>"
                    "  </lessthan_equal>"
                    "</top_statement_list>"
    ); }
    void test_moreDefinition() { TEST_PHP_PARSER(
        "<?php $a > $b;",
                    "<top_statement_list>"
                    "  <greatherthan>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "  </greatherthan>"
                    "</top_statement_list>"
    ); }
    void test_moreEqDefinition() { TEST_PHP_PARSER(
        "<?php $a >= $b;",
                    "<top_statement_list>"
                    "  <greatherthan_equal>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"12\" lineNr=\"0\">$b</variable>"
                    "  </greatherthan_equal>"
                    "</top_statement_list>"
    ); }
    void test_instanceOfDefinition() { TEST_PHP_PARSER(
        "<?php $a instanceof B;",
                    "<top_statement_list>"
                    "  <instanceof>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <namespace_name>"
                    "      <string columnNr=\"20\" lineNr=\"0\">B</string>"
                    "    </namespace_name>"
                    "  </instanceof>"
                    "</top_statement_list>"
    ); }
    void test_parenthesesDefinition() { TEST_PHP_PARSER(
        "<?php ($a);",
                    "<top_statement_list>"
                    "  <variable columnNr=\"7\" lineNr=\"0\">$a</variable>"
                    "</top_statement_list>"
    ); }
    void test_ternaryDefinition() { TEST_PHP_PARSER(
        "<?php $a ? $b : $c ;",
                    "<top_statement_list>"
                    "  <ternaryop>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <variable columnNr=\"11\" lineNr=\"0\">$b</variable>"
                    "    <variable columnNr=\"16\" lineNr=\"0\">$c</variable>"
                    "  </ternaryop>"
                    "</top_statement_list>"
    ); }
    void test_ternaryPartialDefinition() { TEST_PHP_PARSER(
        "<?php $a ? : $c ;",
                    "<top_statement_list>"
                    "  <ternaryop>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <VOID/>"
                    "    <variable columnNr=\"13\" lineNr=\"0\">$c</variable>"
                    "  </ternaryop>"
                    "</top_statement_list>"
    ); }
    void test_castIntDefinition() { TEST_PHP_PARSER(
        "<?php (int) $a ;",
                    "<top_statement_list>"
                    "  <int_cast>"
                    "    <variable columnNr=\"12\" lineNr=\"0\">$a</variable>"
                    "  </int_cast>"
                    "</top_statement_list>"
    ); }
    void test_castDoubleDefinition() { TEST_PHP_PARSER(
        "<?php (double) $a ;",
                    "<top_statement_list>"
                    "  <double_cast>"
                    "    <variable columnNr=\"15\" lineNr=\"0\">$a</variable>"
                    "  </double_cast>"
                    "</top_statement_list>"
    ); }
    void test_castStringDefinition() { TEST_PHP_PARSER(
        "<?php (string) $a ;",
                    "<top_statement_list>"
                    "  <string_cast>"
                    "    <variable columnNr=\"15\" lineNr=\"0\">$a</variable>"
                    "  </string_cast>"
                    "</top_statement_list>"
    ); }
    void test_castArrayDefinition() { TEST_PHP_PARSER(
        "<?php (array) $a ;",
                    "<top_statement_list>"
                    "  <array_cast>"
                    "    <variable columnNr=\"14\" lineNr=\"0\">$a</variable>"
                    "  </array_cast>"
                    "</top_statement_list>"
    ); }
    void test_castObjectDefinition() { TEST_PHP_PARSER(
        "<?php (object) $a ;",
                    "<top_statement_list>"
                    "  <object_cast>"
                    "    <variable columnNr=\"15\" lineNr=\"0\">$a</variable>"
                    "  </object_cast>"
                    "</top_statement_list>"
    ); }
    void test_castBoolDefinition() { TEST_PHP_PARSER(
        "<?php (bool) $a ;",
                    "<top_statement_list>"
                    "  <bool_cast>"
                    "    <variable columnNr=\"13\" lineNr=\"0\">$a</variable>"
                    "  </bool_cast>"
                    "</top_statement_list>"
    ); }
    void test_castUnsetDefinition() { TEST_PHP_PARSER(
        "<?php (unset) $a ;",
                    "<top_statement_list>"
                    "  <unset_cast>"
                    "    <variable columnNr=\"14\" lineNr=\"0\">$a</variable>"
                    "  </unset_cast>"
                    "</top_statement_list>"
    ); }
    void test_exitDefinition() { TEST_PHP_PARSER(
        "<?php exit (1) ;",
                    "<top_statement_list>"
                    "  <exit>"
                    "    <lnumber columnNr=\"12\" lineNr=\"0\">1</lnumber>"
                    "  </exit>"
                    "</top_statement_list>"
    ); }
    void test_silenceDefinition() { TEST_PHP_PARSER(
        "<?php @$a ;",
                    "<top_statement_list>"
                    "  <silence>"
                    "    <variable columnNr=\"7\" lineNr=\"0\">$a</variable>"
                    "  </silence>"
                    "</top_statement_list>"
    ); }
    void test_arrayDefinition() { TEST_PHP_PARSER(
        "<?php array(1=>$a, &$b, 'c'=>3+2,) ;",
                    "<top_statement_list>"
                    "  <array>"
                    "    <array_pair_list>"
                    "      <array_pair>"
                    "        <array_key>"
                    "          <lnumber columnNr=\"12\" lineNr=\"0\">1</lnumber>"
                    "        </array_key>"
                    "        <array_value>"
                    "          <variable columnNr=\"15\" lineNr=\"0\">$a</variable>"
                    "        </array_value>"
                    "      </array_pair>"
                    "      <array_pair>"
                    "        <array_key/>"
                    "        <array_value is_reference=\"1\">"
                    "          <variable columnNr=\"20\" lineNr=\"0\">$b</variable>"
                    "        </array_value>"
                    "      </array_pair>"
                    "      <array_pair>"
                    "        <array_key>"
                    "          <constant_encapsed_string columnNr=\"24\" lineNr=\"0\">c</constant_encapsed_string>"
                    "        </array_key>"
                    "        <array_value>"
                    "          <plus>"
                    "            <lnumber columnNr=\"29\" lineNr=\"0\">3</lnumber>"
                    "            <lnumber columnNr=\"31\" lineNr=\"0\">2</lnumber>"
                    "          </plus>"
                    "        </array_value>"
                    "      </array_pair>"
                    "    </array_pair_list>"
                    "  </array>"
                    "</top_statement_list>"
    ); }
    void test_arrayDefinitionRef() { TEST_PHP_PARSER(
        "<?php array(&$a, &$b) ;",
                    "<top_statement_list>"
                    "  <array>"
                    "    <array_pair_list>"
                    "      <array_pair>"
                    "        <array_key/>"
                    "        <array_value is_reference=\"1\">"
                    "          <variable columnNr=\"13\" lineNr=\"0\">$a</variable>"
                    "        </array_value>"
                    "      </array_pair>"
                    "      <array_pair>"
                    "        <array_key/>"
                    "        <array_value is_reference=\"1\">"
                    "          <variable columnNr=\"18\" lineNr=\"0\">$b</variable>"
                    "        </array_value>"
                    "      </array_pair>"
                    "    </array_pair_list>"
                    "  </array>"
                    "</top_statement_list>"
    ); }
    void test_backticksDefinition() { TEST_PHP_PARSER(
        "<?php `$a boo $2 moo` ;",
                    "<top_statement_list>"
                    "  <backticks>"
                    "    <encaps_list>"
                    "      <variable columnNr=\"7\" lineNr=\"0\">$a</variable>"
                    "      <encapsed_and_whitespace columnNr=\"9\" lineNr=\"0\"> boo $2 moo</encapsed_and_whitespace>"
                    "    </encaps_list>"
                    "  </backticks>"
                    "</top_statement_list>"
    ); }
    void test_backticksStartsWithBackslash() { TEST_PHP_PARSER(
        "<?php `\\$a boo moo` ;",
                    "<top_statement_list>"
                    "  <backticks>"
                    "    <encapsed_and_whitespace columnNr=\"7\" lineNr=\"0\">\\$a boo moo</encapsed_and_whitespace>"
                    "  </backticks>"
                    "</top_statement_list>"
    ); }
    void test_backticksContainsBackslash() { TEST_PHP_PARSER(
        "<?php `asd\\$a boo {}moo` ;",
                    "<top_statement_list>"
                    "  <backticks>"
                    "    <encapsed_and_whitespace columnNr=\"7\" lineNr=\"0\">asd\\$a boo {}moo</encapsed_and_whitespace>"
                    "  </backticks>"
                    "</top_statement_list>"
    ); }
    void test_backticksWithTwoVariables() { TEST_PHP_PARSER(
        "<?php `asd$a $boo moo` ;",
                    "<top_statement_list>"
                    "  <backticks>"
                    "    <encaps_list>"
                    "      <encapsed_and_whitespace columnNr=\"7\" lineNr=\"0\">asd</encapsed_and_whitespace>"
                    "      <variable columnNr=\"10\" lineNr=\"0\">$a</variable>"
                    "      <encapsed_and_whitespace columnNr=\"12\" lineNr=\"0\"> </encapsed_and_whitespace>"
                    "      <variable columnNr=\"13\" lineNr=\"0\">$boo</variable>"
                    "      <encapsed_and_whitespace columnNr=\"17\" lineNr=\"0\"> moo</encapsed_and_whitespace>"
                    "    </encaps_list>"
                    "  </backticks>"
                    "</top_statement_list>"
    ); }
    void test_backticksWithTwoVariablesAndSecondIsBracketed() { TEST_PHP_PARSER(
        "<?php `asd$a {$boo} moo` ;",
                    "<top_statement_list>"
                    "  <backticks>"
                    "    <encaps_list>"
                    "      <encapsed_and_whitespace columnNr=\"7\" lineNr=\"0\">asd</encapsed_and_whitespace>"
                    "      <variable columnNr=\"10\" lineNr=\"0\">$a</variable>"
                    "      <encapsed_and_whitespace columnNr=\"12\" lineNr=\"0\"> </encapsed_and_whitespace>"
                    "      <variable columnNr=\"15\" lineNr=\"0\">$boo</variable>"
                    "      <encapsed_and_whitespace columnNr=\"20\" lineNr=\"0\"> moo</encapsed_and_whitespace>"
                    "    </encaps_list>"
                    "  </backticks>"
                    "</top_statement_list>"
    ); }
    void test_printDefinition() { TEST_PHP_PARSER(
        "<?php print $x ;",
                    "<top_statement_list>"
                    "  <print>"
                    "    <variable columnNr=\"12\" lineNr=\"0\">$x</variable>"
                    "  </print>"
                    "</top_statement_list>"
    ); }
    void test_lambdaFncDefinition() { TEST_PHP_PARSER(
        "<?php function ($a) { return $c; } ;",
                    "<top_statement_list>"
                    "  <LAMBDA_FUNCTION>"
                    "    <is_reference is_reference=\"0\"/>"
                    "    <parameter_list>"
                    "      <parameter>"
                    "        <__ANY_CLASS_TYPE__/>"
                    "        <variable columnNr=\"16\" lineNr=\"0\">$a</variable>"
                    "      </parameter>"
                    "    </parameter_list>"
                    "    <use/>"
                    "    <inner_statement_list>"
                    "      <return>"
                    "        <variable columnNr=\"29\" lineNr=\"0\">$c</variable>"
                    "      </return>"
                    "    </inner_statement_list>"
                    "  </LAMBDA_FUNCTION>"
                    "</top_statement_list>"
    ); }
    void test_lambdaFncUseDefinition() { TEST_PHP_PARSER(
        "<?php function ($a) use ($b) { return $c; } ;",
                    "<top_statement_list>"
                    "  <LAMBDA_FUNCTION>"
                    "    <is_reference is_reference=\"0\"/>"
                    "    <parameter_list>"
                    "      <parameter>"
                    "        <__ANY_CLASS_TYPE__/>"
                    "        <variable columnNr=\"16\" lineNr=\"0\">$a</variable>"
                    "      </parameter>"
                    "    </parameter_list>"
                    "    <use>"
                    "      <lexical_var_list>"
                    "        <variable columnNr=\"25\" lineNr=\"0\">$b</variable>"
                    "      </lexical_var_list>"
                    "    </use>"
                    "    <inner_statement_list>"
                    "      <return>"
                    "        <variable columnNr=\"38\" lineNr=\"0\">$c</variable>"
                    "      </return>"
                    "    </inner_statement_list>"
                    "  </LAMBDA_FUNCTION>"
                    "</top_statement_list>"
    ); }
    void test_lambdaFncRefDefinition() { TEST_PHP_PARSER(
        "<?php function & ($a) use ($b, &$c) { return $c; } ;",
                    "<top_statement_list>"
                    "  <LAMBDA_FUNCTION>"
                    "    <is_reference is_reference=\"1\"/>"
                    "    <parameter_list>"
                    "      <parameter>"
                    "        <__ANY_CLASS_TYPE__/>"
                    "        <variable columnNr=\"18\" lineNr=\"0\">$a</variable>"
                    "      </parameter>"
                    "    </parameter_list>"
                    "    <use>"
                    "      <lexical_var_list>"
                    "        <variable columnNr=\"27\" lineNr=\"0\">$b</variable>"
                    "        <variable columnNr=\"32\" is_reference=\"1\" lineNr=\"0\">$c</variable>"
                    "      </lexical_var_list>"
                    "    </use>"
                    "    <inner_statement_list>"
                    "      <return>"
                    "        <variable columnNr=\"45\" lineNr=\"0\">$c</variable>"
                    "      </return>"
                    "    </inner_statement_list>"
                    "  </LAMBDA_FUNCTION>"
                    "</top_statement_list>"
    ); }
    void test_doubleQuotesConst() { TEST_PHP_PARSER(
        "<?php $a = \"\\rtest $ {} string\\n\" ;",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <constant_encapsed_string columnNr=\"11\" lineNr=\"0\">\\rtest $ {} string\\n</constant_encapsed_string>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_doubleQuotesWithVars() { TEST_PHP_PARSER(
        "<?php $a = \"\\ntest $moo more text\n\\n {$boo}${buka}s${aa[2]} {$o->prop}trin $ {} g\" ;",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <doubleQuotes>"
                    "      <encaps_list>"
                    "        <encapsed_and_whitespace columnNr=\"12\" lineNr=\"0\">\\ntest </encapsed_and_whitespace>"
                    "        <variable columnNr=\"19\" lineNr=\"0\">$moo</variable>"
                    "        <encapsed_and_whitespace columnNr=\"23\" lineNr=\"0\"> more text"
                    "\\n </encapsed_and_whitespace>"
                    "        <variable columnNr=\"5\" lineNr=\"1\">$boo</variable>"
                    "        <string_varname columnNr=\"12\" lineNr=\"1\">buka</string_varname>"
                    "        <encapsed_and_whitespace columnNr=\"17\" lineNr=\"1\">s</encapsed_and_whitespace>"
                    "        <offset>"
                    "          <string_varname columnNr=\"20\" lineNr=\"1\">aa</string_varname>"
                    "          <lnumber columnNr=\"23\" lineNr=\"1\">2</lnumber>"
                    "        </offset>"
                    "        <encapsed_and_whitespace columnNr=\"26\" lineNr=\"1\"> </encapsed_and_whitespace>"
                    "        <object_operator>"
                    "          <variable columnNr=\"29\" lineNr=\"1\">$o</variable>"
                    "          <string columnNr=\"33\" lineNr=\"1\">prop</string>"
                    "          <method_or_not/>"
                    "          <variable_properties/>"
                    "        </object_operator>"
                    "        <encapsed_and_whitespace columnNr=\"38\" lineNr=\"1\">trin $ {} g</encapsed_and_whitespace>"
                    "      </encaps_list>"
                    "    </doubleQuotes>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_doubleQuotesWithVarsStartsBracketed() { TEST_PHP_PARSER(
        "<?php $a = \"\\ntest {$moo} more text\n\\n {$boo}${buka}s${aa[2]} {$o->prop}trin $ {} g\" ;",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <doubleQuotes>"
                    "      <encaps_list>"
                    "        <encapsed_and_whitespace columnNr=\"12\" lineNr=\"0\">\\ntest </encapsed_and_whitespace>"
                    "        <variable columnNr=\"21\" lineNr=\"0\">$moo</variable>"
                    "        <encapsed_and_whitespace columnNr=\"26\" lineNr=\"0\"> more text"
                    "\\n </encapsed_and_whitespace>"
                    "        <variable columnNr=\"5\" lineNr=\"1\">$boo</variable>"
                    "        <string_varname columnNr=\"12\" lineNr=\"1\">buka</string_varname>"
                    "        <encapsed_and_whitespace columnNr=\"17\" lineNr=\"1\">s</encapsed_and_whitespace>"
                    "        <offset>"
                    "          <string_varname columnNr=\"20\" lineNr=\"1\">aa</string_varname>"
                    "          <lnumber columnNr=\"23\" lineNr=\"1\">2</lnumber>"
                    "        </offset>"
                    "        <encapsed_and_whitespace columnNr=\"26\" lineNr=\"1\"> </encapsed_and_whitespace>"
                    "        <object_operator>"
                    "          <variable columnNr=\"29\" lineNr=\"1\">$o</variable>"
                    "          <string columnNr=\"33\" lineNr=\"1\">prop</string>"
                    "          <method_or_not/>"
                    "          <variable_properties/>"
                    "        </object_operator>"
                    "        <encapsed_and_whitespace columnNr=\"38\" lineNr=\"1\">trin $ {} g</encapsed_and_whitespace>"
                    "      </encaps_list>"
                    "    </doubleQuotes>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_doubleQuotesWithNonVars() { TEST_PHP_PARSER(
        "<?php $a = \"test $12 more text\" ;",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <constant_encapsed_string columnNr=\"11\" lineNr=\"0\">test $12 more text</constant_encapsed_string>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_singleQuotesEscaping() { TEST_PHP_PARSER(
        "<?php $a = 'test\\' $a more text' ;",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <constant_encapsed_string columnNr=\"11\" lineNr=\"0\">test\\' $a more text</constant_encapsed_string>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_hereDoc() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\ntex{t\nMYDOC; ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <encapsed_and_whitespace columnNr=\"0\" lineNr=\"1\">tex{t</encapsed_and_whitespace>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_hereDocWithWindowsEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\r\ntext\r\nMYDOC; ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <encapsed_and_whitespace columnNr=\"0\" lineNr=\"1\">text</encapsed_and_whitespace>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_hereDocWithMacEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\rtext\rMYDOC; ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <encapsed_and_whitespace columnNr=\"20\" lineNr=\"0\">text</encapsed_and_whitespace>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_hereDocMultiline() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\rtext\na\rs\r\nd\n\rMYDOC; ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <encapsed_and_whitespace columnNr=\"20\" lineNr=\"0\">text"
                    "a&#xD;s&#xD;"
                    "d"
                    "</encapsed_and_whitespace>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_hereDocWithVars() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\ntex$var1 {$var2} {$var3[$i]} {$var4->prop} ${var5[$i]}$var6[$i]$var7->prop t\nMYDOC; ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <hereDoc>"
                    "      <encaps_list>"
                    "        <encapsed_and_whitespace columnNr=\"0\" lineNr=\"1\">tex</encapsed_and_whitespace>"
                    "        <variable columnNr=\"3\" lineNr=\"1\">$var1</variable>"
                    "        <encapsed_and_whitespace columnNr=\"8\" lineNr=\"1\"> </encapsed_and_whitespace>"
                    "        <variable columnNr=\"11\" lineNr=\"1\">$var2</variable>"
                    "        <encapsed_and_whitespace columnNr=\"17\" lineNr=\"1\"> </encapsed_and_whitespace>"
                    "        <offset>"
                    "          <variable columnNr=\"20\" lineNr=\"1\">$var3</variable>"
                    "          <variable columnNr=\"26\" lineNr=\"1\">$i</variable>"
                    "        </offset>"
                    "        <encapsed_and_whitespace columnNr=\"30\" lineNr=\"1\"> </encapsed_and_whitespace>"
                    "        <object_operator>"
                    "          <variable columnNr=\"33\" lineNr=\"1\">$var4</variable>"
                    "          <string columnNr=\"40\" lineNr=\"1\">prop</string>"
                    "          <method_or_not/>"
                    "          <variable_properties/>"
                    "        </object_operator>"
                    "        <encapsed_and_whitespace columnNr=\"45\" lineNr=\"1\"> </encapsed_and_whitespace>"
                    "        <offset>"
                    "          <string_varname columnNr=\"48\" lineNr=\"1\">var5</string_varname>"
                    "          <variable columnNr=\"53\" lineNr=\"1\">$i</variable>"
                    "        </offset>"
                    "        <offset>"
                    "          <variable columnNr=\"57\" lineNr=\"1\">$var6</variable>"
                    "          <variable columnNr=\"64\" lineNr=\"1\">$i</variable>"
                    "        </offset>"
                    "        <object_operator>"
                    "          <variable columnNr=\"67\" lineNr=\"1\">$var7</variable>"
                    "          <string columnNr=\"77\" lineNr=\"1\">prop</string>"
                    "        </object_operator>"
                    "        <encapsed_and_whitespace columnNr=\"81\" lineNr=\"1\"> t</encapsed_and_whitespace>"
                    "      </encaps_list>"
                    "    </hereDoc>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_hereDocWithQuoteLabels() { TEST_PHP_PARSER(
        "<?php $a = <<<\"MYDOC\"\ntext\nMYDOC; ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <encapsed_and_whitespace columnNr=\"0\" lineNr=\"1\">text</encapsed_and_whitespace>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_hereDocWithNonVars() { TEST_PHP_PARSER(
        "<?php $a = <<<\"MYDOC\"\ntex $123 t\nMYDOC; ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <encapsed_and_whitespace columnNr=\"0\" lineNr=\"1\">tex $123 t</encapsed_and_whitespace>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_nowDoc() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\ntext$nonvar w\nMYDOC; ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <encapsed_and_whitespace columnNr=\"0\" lineNr=\"1\">text$nonvar w</encapsed_and_whitespace>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_nowDocWithWindowsEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\r\ntext\r\nMYDOC; ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <encapsed_and_whitespace columnNr=\"0\" lineNr=\"1\">text</encapsed_and_whitespace>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_nowDocWithMacEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\rtext\rMYDOC; ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <encapsed_and_whitespace columnNr=\"22\" lineNr=\"0\">text</encapsed_and_whitespace>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_nowDocMultiline() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\rtext\na\rs\r\nd\n\rMYDOC; ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <encapsed_and_whitespace columnNr=\"22\" lineNr=\"0\">text"
                    "a&#xD;s&#xD;"
                    "d"
                    "</encapsed_and_whitespace>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_singleLineComment() { TEST_PHP_PARSER(
        "<?php //$a =? <<<'MYDOC'\n$s=1; // asd \n // //..  ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"0\" lineNr=\"1\">$s</variable>"
                    "    <lnumber columnNr=\"3\" lineNr=\"1\">1</lnumber>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_singleLineCommentWindowsEndline() { TEST_PHP_PARSER(
        "<?php //$a = <<<'MYDOC'\r\n$s=1; // asd \r\n // //..  ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"0\" lineNr=\"1\">$s</variable>"
                    "    <lnumber columnNr=\"3\" lineNr=\"1\">1</lnumber>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_singleLineCommentMacEndline() { TEST_PHP_PARSER(
        "<?php //$a = <<<'MYDOC'\r$s=1; // asd \r // //..  ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"24\" lineNr=\"0\">$s</variable>"
                    "    <lnumber columnNr=\"27\" lineNr=\"0\">1</lnumber>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_singleLineCommentScriptStop() { TEST_PHP_PARSER(
        "<?php //$a =? <<<'MYDOC'?>$s=1; // asd \n // //..  ",
                    "<top_statement_list>"
                    "  <empty statement/>"
                    "  <inline_html columnNr=\"26\" lineNr=\"0\">$s=1; // asd "
                    " // //..  </inline_html>"
                    "</top_statement_list>"
    ); }
    void test_singleLineHashComment() { TEST_PHP_PARSER(
        "<?php #$a = <<<'MYDOC'\n$s=1; # asd \n # /..  ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"0\" lineNr=\"1\">$s</variable>"
                    "    <lnumber columnNr=\"3\" lineNr=\"1\">1</lnumber>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_multiLineComment() { TEST_PHP_PARSER(
        "<?php /* $a = *<<<'MYDOC'\n$s=1; # asd \n # /.. */ $b=2; ",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"10\" lineNr=\"2\">$b</variable>"
                    "    <lnumber columnNr=\"13\" lineNr=\"2\">2</lnumber>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_docComment() { TEST_PHP_PARSER(
        "<?php /** lala */ ",
                    "<top_statement_list/>"
    ); }
    void test_numbers() { TEST_PHP_PARSER(
        "<?php $a=12+12.2+.2+0x02+0123+2e1;",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <plus>"
                    "      <plus>"
                    "        <plus>"
                    "          <plus>"
                    "            <plus>"
                    "              <lnumber columnNr=\"9\" lineNr=\"0\">12</lnumber>"
                    "              <dnumber columnNr=\"12\" lineNr=\"0\">12.2</dnumber>"
                    "            </plus>"
                    "            <dnumber columnNr=\"17\" lineNr=\"0\">.2</dnumber>"
                    "          </plus>"
                    "          <lnumber columnNr=\"20\" lineNr=\"0\">0x02</lnumber>"
                    "        </plus>"
                    "        <lnumber columnNr=\"25\" lineNr=\"0\">0123</lnumber>"
                    "      </plus>"
                    "      <dnumber columnNr=\"30\" lineNr=\"0\">2e1</dnumber>"
                    "    </plus>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_phpConstants() { TEST_PHP_PARSER(
        "<?php $a=__CLASS__.__FUNCTION__.__METHOD__.__LINE__.__FILE__.__DIR__.__NAMESPACE__;",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$a</variable>"
                    "    <concat>"
                    "      <concat>"
                    "        <concat>"
                    "          <concat>"
                    "            <concat>"
                    "              <concat>"
                    "                <__CLASS__ columnNr=\"9\" lineNr=\"0\"/>"
                    "                <__FUNCTION__ columnNr=\"19\" lineNr=\"0\"/>"
                    "              </concat>"
                    "              <__METHOD__ columnNr=\"32\" lineNr=\"0\"/>"
                    "            </concat>"
                    "            <__LINE__ columnNr=\"43\" lineNr=\"0\"/>"
                    "          </concat>"
                    "          <__FILE__ columnNr=\"52\" lineNr=\"0\"/>"
                    "        </concat>"
                    "        <__DIR__ columnNr=\"61\" lineNr=\"0\"/>"
                    "      </concat>"
                    "      <__NAMESPACE__ columnNr=\"69\" lineNr=\"0\"/>"
                    "    </concat>"
                    "  </assignment>"
                    "</top_statement_list>"
    ); }
    void test_classDefinition() { TEST_PHP_PARSER(
        "<?php class A { }",
                    "<top_statement_list>"
                    "  <class_declaration>"
                    "    <class columnNr=\"6\" lineNr=\"0\"/>"
                    "    <string columnNr=\"12\" lineNr=\"0\">A</string>"
                    "    <extends/>"
                    "    <implements/>"
                    "    <class_statement_list/>"
                    "  </class_declaration>"
                    "</top_statement_list>"
    );}
    void test_classExtendingOtherDefinition() { TEST_PHP_PARSER(
        "<?php class A extends B { }",
                    "<top_statement_list>"
                    "  <class_declaration>"
                    "    <class columnNr=\"6\" lineNr=\"0\"/>"
                    "    <string columnNr=\"12\" lineNr=\"0\">A</string>"
                    "    <extends>"
                    "      <namespace_name>"
                    "        <string columnNr=\"22\" lineNr=\"0\">B</string>"
                    "      </namespace_name>"
                    "    </extends>"
                    "    <implements/>"
                    "    <class_statement_list/>"
                    "  </class_declaration>"
                    "</top_statement_list>"
    );}
    void test_classImplementingInterfaceDefinition() { TEST_PHP_PARSER(
        "<?php class A implements I { }",
                    "<top_statement_list>"
                    "  <class_declaration>"
                    "    <class columnNr=\"6\" lineNr=\"0\"/>"
                    "    <string columnNr=\"12\" lineNr=\"0\">A</string>"
                    "    <extends/>"
                    "    <implements>"
                    "      <interfaceList>"
                    "        <namespace_name>"
                    "          <string columnNr=\"25\" lineNr=\"0\">I</string>"
                    "        </namespace_name>"
                    "      </interfaceList>"
                    "    </implements>"
                    "    <class_statement_list/>"
                    "  </class_declaration>"
                    "</top_statement_list>"
    );}
    void test_abstractClassDefinition() { TEST_PHP_PARSER(
        "<?php abstract class A { }",
                    "<top_statement_list>"
                    "  <class_declaration>"
                    "    <class columnNr=\"15\" lineNr=\"0\" type=\"abstract\"/>"
                    "    <string columnNr=\"21\" lineNr=\"0\">A</string>"
                    "    <extends/>"
                    "    <implements/>"
                    "    <class_statement_list/>"
                    "  </class_declaration>"
                    "</top_statement_list>"
    );}
    void test_finalClassDefinition() { TEST_PHP_PARSER(
        "<?php final class A { }",
                    "<top_statement_list>"
                    "  <class_declaration>"
                    "    <class columnNr=\"12\" lineNr=\"0\" type=\"final\"/>"
                    "    <string columnNr=\"18\" lineNr=\"0\">A</string>"
                    "    <extends/>"
                    "    <implements/>"
                    "    <class_statement_list/>"
                    "  </class_declaration>"
                    "</top_statement_list>"
    );}
    void test_interfaceDefinition() { TEST_PHP_PARSER(
        "<?php interface A { }",
                    "<top_statement_list>"
                    "  <interface_declaration>"
                    "    <interface columnNr=\"6\" lineNr=\"0\"/>"
                    "    <string columnNr=\"16\" lineNr=\"0\">A</string>"
                    "    <extends/>"
                    "    <class_statement_list/>"
                    "  </interface_declaration>"
                    "</top_statement_list>"
    );}
    void test_interfaceInheritanceDefinition() { TEST_PHP_PARSER(
        "<?php interface A extends B, C { }",
                    "<top_statement_list>"
                    "  <interface_declaration>"
                    "    <interface columnNr=\"6\" lineNr=\"0\"/>"
                    "    <string columnNr=\"16\" lineNr=\"0\">A</string>"
                    "    <extends>"
                    "      <interfaceList>"
                    "        <namespace_name>"
                    "          <string columnNr=\"26\" lineNr=\"0\">B</string>"
                    "        </namespace_name>"
                    "        <namespace_name>"
                    "          <string columnNr=\"29\" lineNr=\"0\">C</string>"
                    "        </namespace_name>"
                    "      </interfaceList>"
                    "    </extends>"
                    "    <class_statement_list/>"
                    "  </interface_declaration>"
                    "</top_statement_list>"
    );}

    void test_statementIf() { TEST_PHP_PARSER(
        "<?php if(1) {echo 'x';} elseif (2) { echo 'y'; } else echo 'z';",
                    "<top_statement_list>"
                    "  <if>"
                    "    <lnumber columnNr=\"9\" lineNr=\"0\">1</lnumber>"
                    "    <inner_statement_list>"
                    "      <echo>"
                    "        <echo_expr_list>"
                    "          <constant_encapsed_string columnNr=\"18\" lineNr=\"0\">x</constant_encapsed_string>"
                    "        </echo_expr_list>"
                    "      </echo>"
                    "    </inner_statement_list>"
                    "    <elseif_list>"
                    "      <elseif>"
                    "        <lnumber columnNr=\"32\" lineNr=\"0\">2</lnumber>"
                    "        <inner_statement_list>"
                    "          <echo>"
                    "            <echo_expr_list>"
                    "              <constant_encapsed_string columnNr=\"42\" lineNr=\"0\">y</constant_encapsed_string>"
                    "            </echo_expr_list>"
                    "          </echo>"
                    "        </inner_statement_list>"
                    "      </elseif>"
                    "    </elseif_list>"
                    "    <else>"
                    "      <echo>"
                    "        <echo_expr_list>"
                    "          <constant_encapsed_string columnNr=\"59\" lineNr=\"0\">z</constant_encapsed_string>"
                    "        </echo_expr_list>"
                    "      </echo>"
                    "    </else>"
                    "  </if>"
                    "</top_statement_list>"
    );}
    void test_statementWhile() { TEST_PHP_PARSER(
        "<?php while(1) {echo 'x';}",
                    "<top_statement_list>"
                    "  <while>"
                    "    <lnumber columnNr=\"12\" lineNr=\"0\">1</lnumber>"
                    "    <inner_statement_list>"
                    "      <echo>"
                    "        <echo_expr_list>"
                    "          <constant_encapsed_string columnNr=\"21\" lineNr=\"0\">x</constant_encapsed_string>"
                    "        </echo_expr_list>"
                    "      </echo>"
                    "    </inner_statement_list>"
                    "  </while>"
                    "</top_statement_list>"
    );}
    void test_statementDoWhile() { TEST_PHP_PARSER(
        "<?php do {echo 'x';} while(1);",
                    "<top_statement_list>"
                    "  <dowhile>"
                    "    <inner_statement_list>"
                    "      <echo>"
                    "        <echo_expr_list>"
                    "          <constant_encapsed_string columnNr=\"15\" lineNr=\"0\">x</constant_encapsed_string>"
                    "        </echo_expr_list>"
                    "      </echo>"
                    "    </inner_statement_list>"
                    "    <lnumber columnNr=\"27\" lineNr=\"0\">1</lnumber>"
                    "  </dowhile>"
                    "</top_statement_list>"
    );}
    void test_statementFor() { TEST_PHP_PARSER(
        "<?php for($i=0;$i<1;$i++) {echo 'x';}",
                    "<top_statement_list>"
                    "  <for>"
                    "    <assignment>"
                    "      <variable columnNr=\"10\" lineNr=\"0\">$i</variable>"
                    "      <lnumber columnNr=\"13\" lineNr=\"0\">0</lnumber>"
                    "    </assignment>"
                    "    <lessthan>"
                    "      <variable columnNr=\"15\" lineNr=\"0\">$i</variable>"
                    "      <lnumber columnNr=\"18\" lineNr=\"0\">1</lnumber>"
                    "    </lessthan>"
                    "    <postinc>"
                    "      <variable columnNr=\"20\" lineNr=\"0\">$i</variable>"
                    "    </postinc>"
                    "    <inner_statement_list>"
                    "      <echo>"
                    "        <echo_expr_list>"
                    "          <constant_encapsed_string columnNr=\"32\" lineNr=\"0\">x</constant_encapsed_string>"
                    "        </echo_expr_list>"
                    "      </echo>"
                    "    </inner_statement_list>"
                    "  </for>"
                    "</top_statement_list>"
    );}
    void test_statementSwitch() { TEST_PHP_PARSER(
        "<?php switch($i) {case 'x': echo 'y'; default: echo 'z';}",
                    "<top_statement_list>"
                    "  <switch>"
                    "    <variable columnNr=\"13\" lineNr=\"0\">$i</variable>"
                    "    <case_list>"
                    "      <case>"
                    "        <constant_encapsed_string columnNr=\"23\" lineNr=\"0\">x</constant_encapsed_string>"
                    "        <inner_statement_list>"
                    "          <echo>"
                    "            <echo_expr_list>"
                    "              <constant_encapsed_string columnNr=\"33\" lineNr=\"0\">y</constant_encapsed_string>"
                    "            </echo_expr_list>"
                    "          </echo>"
                    "        </inner_statement_list>"
                    "      </case>"
                    "      <default_case>"
                    "        <inner_statement_list>"
                    "          <echo>"
                    "            <echo_expr_list>"
                    "              <constant_encapsed_string columnNr=\"52\" lineNr=\"0\">z</constant_encapsed_string>"
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
                    "<top_statement_list>"
                    "  <break/>"
                    "</top_statement_list>"
    );}
    void test_statementContinue() { TEST_PHP_PARSER(
        "<?php continue;",
                    "<top_statement_list>"
                    "  <continue/>"
                    "</top_statement_list>"
    );}
    void test_statementReturn() { TEST_PHP_PARSER(
        "<?php function x(){return 2;}",
                    "<top_statement_list>"
                    "  <function_declaration>"
                    "    <is_reference is_reference=\"0\"/>"
                    "    <string columnNr=\"15\" lineNr=\"0\">x</string>"
                    "    <parameter_list/>"
                    "    <inner_statement_list>"
                    "      <return>"
                    "        <lnumber columnNr=\"26\" lineNr=\"0\">2</lnumber>"
                    "      </return>"
                    "    </inner_statement_list>"
                    "  </function_declaration>"
                    "</top_statement_list>"
    );}
    void test_statementStaticVars() { TEST_PHP_PARSER(
        "<?php function x(){static $a = 2; return $a;}",
                    "<top_statement_list>"
                    "  <function_declaration>"
                    "    <is_reference is_reference=\"0\"/>"
                    "    <string columnNr=\"15\" lineNr=\"0\">x</string>"
                    "    <parameter_list/>"
                    "    <inner_statement_list>"
                    "      <static>"
                    "        <static_var_list>"
                    "          <variable columnNr=\"26\" lineNr=\"0\">$a<lnumber columnNr=\"31\" lineNr=\"0\">2</lnumber></variable>"
                    "        </static_var_list>"
                    "      </static>"
                    "      <return>"
                    "        <variable columnNr=\"41\" lineNr=\"0\">$a</variable>"
                    "      </return>"
                    "    </inner_statement_list>"
                    "  </function_declaration>"
                    "</top_statement_list>"
    );}
    void test_statementForeach() { TEST_PHP_PARSER(
        "<?php foreach($ar as $a=>$v) {echo 'x';}",
                    "<top_statement_list>"
                    "  <foreach>"
                    "    <variable columnNr=\"14\" lineNr=\"0\">$ar</variable>"
                    "    <variable columnNr=\"21\" lineNr=\"0\">$a</variable>"
                    "    <foreach_optional_arg>"
                    "      <variable columnNr=\"25\" lineNr=\"0\">$v</variable>"
                    "    </foreach_optional_arg>"
                    "    <inner_statement_list>"
                    "      <echo>"
                    "        <echo_expr_list>"
                    "          <constant_encapsed_string columnNr=\"35\" lineNr=\"0\">x</constant_encapsed_string>"
                    "        </echo_expr_list>"
                    "      </echo>"
                    "    </inner_statement_list>"
                    "  </foreach>"
                    "</top_statement_list>"
    );}
    void test_statementForeachRef() { TEST_PHP_PARSER(
        "<?php foreach($ar as $a=>&$v) {echo 'x';}",
                    "<top_statement_list>"
                    "  <foreach>"
                    "    <variable columnNr=\"14\" lineNr=\"0\">$ar</variable>"
                    "    <variable columnNr=\"21\" lineNr=\"0\">$a</variable>"
                    "    <foreach_optional_arg>"
                    "      <variable columnNr=\"26\" is_reference=\"1\" lineNr=\"0\">$v</variable>"
                    "    </foreach_optional_arg>"
                    "    <inner_statement_list>"
                    "      <echo>"
                    "        <echo_expr_list>"
                    "          <constant_encapsed_string columnNr=\"36\" lineNr=\"0\">x</constant_encapsed_string>"
                    "        </echo_expr_list>"
                    "      </echo>"
                    "    </inner_statement_list>"
                    "  </foreach>"
                    "</top_statement_list>"
    );}
    void test_statementThrow() { TEST_PHP_PARSER(
        "<?php\nthrow new Exception();",
                    "<top_statement_list>"
                    "  <throw>"
                    "    <new>"
                    "      <namespace_name>"
                    "        <string columnNr=\"10\" lineNr=\"1\">Exception</string>"
                    "      </namespace_name>"
                    "      <function_call_parameter_list/>"
                    "    </new>"
                    "  </throw>"
                    "</top_statement_list>"
    );}
    void test_statementThyCatchCatch() { TEST_PHP_PARSER(
        "<?php try {echo 'x';} catch (MyException $e) {} catch (Exception $e) {}",
                    "<top_statement_list>"
                    "  <try>"
                    "    <inner_statement_list>"
                    "      <echo>"
                    "        <echo_expr_list>"
                    "          <constant_encapsed_string columnNr=\"16\" lineNr=\"0\">x</constant_encapsed_string>"
                    "        </echo_expr_list>"
                    "      </echo>"
                    "    </inner_statement_list>"
                    "    <catch>"
                    "      <namespace_name>"
                    "        <string columnNr=\"29\" lineNr=\"0\">MyException</string>"
                    "      </namespace_name>"
                    "      <variable columnNr=\"41\" lineNr=\"0\">$e</variable>"
                    "      <inner_statement_list/>"
                    "    </catch>"
                    "    <additional_catches>"
                    "      <catch>"
                    "        <namespace_name>"
                    "          <string columnNr=\"55\" lineNr=\"0\">Exception</string>"
                    "        </namespace_name>"
                    "        <variable columnNr=\"65\" lineNr=\"0\">$e</variable>"
                    "        <inner_statement_list/>"
                    "      </catch>"
                    "    </additional_catches>"
                    "  </try>"
                    "</top_statement_list>"
    );}
    void test_parsingRequire() { TEST_PHP_PARSER(
        "<?php require 'file.php';\n",
                    "<top_statement_list>"
                    "  <require>"
                    "    <constant_encapsed_string columnNr=\"14\" lineNr=\"0\">file.php</constant_encapsed_string>"
                    "  </require>"
                    "</top_statement_list>"
    );}
    void test_classMethod() { TEST_PHP_PARSER(
        "<?php\n"
        "class my_class\n"
        "{\n"
        "    public function my_method()\n"
        "    {\n"
        "    }\n"
        "}",
                    "<top_statement_list>"
                    "  <class_declaration>"
                    "    <class columnNr=\"0\" lineNr=\"1\"/>"
                    "    <string columnNr=\"6\" lineNr=\"1\">my_class</string>"
                    "    <extends/>"
                    "    <implements/>"
                    "    <class_statement_list>"
                    "      <METHOD>"
                    "        <MODIFIERS>"
                    "          <public/>"
                    "        </MODIFIERS>"
                    "        <is_reference is_reference=\"0\"/>"
                    "        <string columnNr=\"20\" lineNr=\"3\">my_method</string>"
                    "        <parameter_list/>"
                    "        <METHOD_BODY>"
                    "          <inner_statement_list/>"
                    "        </METHOD_BODY>"
                    "      </METHOD>"
                    "    </class_statement_list>"
                    "  </class_declaration>"
                    "</top_statement_list>"
    );}
    void test_classProperty() { TEST_PHP_PARSER(
        "<?php\n"
        "class my_class\n"
        "{\n"
        "    public $prop = 1;\n"
        "}",
                    "<top_statement_list>"
                    "  <class_declaration>"
                    "    <class columnNr=\"0\" lineNr=\"1\"/>"
                    "    <string columnNr=\"6\" lineNr=\"1\">my_class</string>"
                    "    <extends/>"
                    "    <implements/>"
                    "    <class_statement_list>"
                    "      <PROPERTY>"
                    "        <MODIFIERS>"
                    "          <public/>"
                    "        </MODIFIERS>"
                    "        <class_properties>"
                    "          <variable columnNr=\"11\" lineNr=\"3\">$prop<lnumber columnNr=\"19\" lineNr=\"3\">1</lnumber></variable>"
                    "        </class_properties>"
                    "      </PROPERTY>"
                    "    </class_statement_list>"
                    "  </class_declaration>"
                    "</top_statement_list>"
    );}
    void test_classPropertyList() { TEST_PHP_PARSER(
        "<?php\n"
        "class my_class\n"
        "{\n"
        "    private final $prop = 1, $x, $y = array('c'=>3,);\n"
        "}",
                    "<top_statement_list>"
                    "  <class_declaration>"
                    "    <class columnNr=\"0\" lineNr=\"1\"/>"
                    "    <string columnNr=\"6\" lineNr=\"1\">my_class</string>"
                    "    <extends/>"
                    "    <implements/>"
                    "    <class_statement_list>"
                    "      <PROPERTY>"
                    "        <MODIFIERS>"
                    "          <private/>"
                    "          <final/>"
                    "        </MODIFIERS>"
                    "        <class_properties>"
                    "          <variable columnNr=\"18\" lineNr=\"3\">$prop<lnumber columnNr=\"26\" lineNr=\"3\">1</lnumber></variable>"
                    "          <variable columnNr=\"29\" lineNr=\"3\">$x</variable>"
                    "          <variable columnNr=\"33\" lineNr=\"3\">$y<array><array_pair_list><array_pair><array_key><constant_encapsed_string columnNr=\"44\" lineNr=\"3\">c</constant_encapsed_string></array_key><array_value><lnumber columnNr=\"49\" lineNr=\"3\">3</lnumber></array_value></array_pair></array_pair_list></array></variable>"
                    "        </class_properties>"
                    "      </PROPERTY>"
                    "    </class_statement_list>"
                    "  </class_declaration>"
                    "</top_statement_list>"
    );}
    void test_classConstant() { TEST_PHP_PARSER(
        "<?php\n"
        "class my_class\n"
        "{\n"
        "    const MYCONST = 1, OTHER=2;\n"
        "}",
                    "<top_statement_list>"
                    "  <class_declaration>"
                    "    <class columnNr=\"0\" lineNr=\"1\"/>"
                    "    <string columnNr=\"6\" lineNr=\"1\">my_class</string>"
                    "    <extends/>"
                    "    <implements/>"
                    "    <class_statement_list>"
                    "      <class_constants>"
                    "        <string columnNr=\"10\" lineNr=\"3\">MYCONST<lnumber columnNr=\"20\" lineNr=\"3\">1</lnumber></string>"
                    "        <string columnNr=\"23\" lineNr=\"3\">OTHER<lnumber columnNr=\"29\" lineNr=\"3\">2</lnumber></string>"
                    "      </class_constants>"
                    "    </class_statement_list>"
                    "  </class_declaration>"
                    "</top_statement_list>"
    );}
    void test_methodCallWithNegativeNumber() { TEST_PHP_PARSER(
        "<?php $object->method(-2);",
                    "<top_statement_list>"
                    "  <object_operator>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$object</variable>"
                    "    <string columnNr=\"15\" lineNr=\"0\">method</string>"
                    "    <method_or_not>"
                    "      <function_call_parameter_list>"
                    "        <negative>"
                    "          <lnumber columnNr=\"23\" lineNr=\"0\">2</lnumber>"
                    "        </negative>"
                    "      </function_call_parameter_list>"
                    "    </method_or_not>"
                    "    <variable_properties/>"
                    "  </object_operator>"
                    "</top_statement_list>"
    );}
    void test_negativeNrAsOperation() { TEST_PHP_PARSER(
        "<?php $x-1;",
                    "<top_statement_list>"
                    "  <minus>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$x</variable>"
                    "    <lnumber columnNr=\"9\" lineNr=\"0\">1</lnumber>"
                    "  </minus>"
                    "</top_statement_list>"
    );}
    void test_addToArray() { TEST_PHP_PARSER(
        "<?php $array[] = $x;",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <offset>"
                    "      <variable columnNr=\"6\" lineNr=\"0\">$array</variable>"
                    "      <empty_offset/>"
                    "    </offset>"
                    "    <variable columnNr=\"17\" lineNr=\"0\">$x</variable>"
                    "  </assignment>"
                    "</top_statement_list>"
    );}
    void test_exitWithoutParameters() { TEST_PHP_PARSER(
        "<?php exit;",
                    "<top_statement_list>"
                    "  <exit>"
                    "    <VOID/>"
                    "  </exit>"
                    "</top_statement_list>"
    );}

    void test_functionParametersDefinitionAllowsDefaults() { TEST_PHP_PARSER(
        "<?php function method($parameter, $type = null) {}",
                    "<top_statement_list>"
                    "  <function_declaration>"
                    "    <is_reference is_reference=\"0\"/>"
                    "    <string columnNr=\"15\" lineNr=\"0\">method</string>"
                    "    <parameter_list>"
                    "      <parameter>"
                    "        <__ANY_CLASS_TYPE__/>"
                    "        <variable columnNr=\"22\" lineNr=\"0\">$parameter</variable>"
                    "      </parameter>"
                    "      <parameter>"
                    "        <__ANY_CLASS_TYPE__/>"
                    "        <variable columnNr=\"34\" lineNr=\"0\">$type</variable>"
                    "        <namespace_name>"
                    "          <string columnNr=\"42\" lineNr=\"0\">null</string>"
                    "        </namespace_name>"
                    "      </parameter>"
                    "    </parameter_list>"
                    "    <inner_statement_list/>"
                    "  </function_declaration>"
                    "</top_statement_list>"
    );}
    void test_functionParametersDefinitionAllowsReference() { TEST_PHP_PARSER(
        "<?php function method($parameter, &$variable) {}",
                    "<top_statement_list>"
                    "  <function_declaration>"
                    "    <is_reference is_reference=\"0\"/>"
                    "    <string columnNr=\"15\" lineNr=\"0\">method</string>"
                    "    <parameter_list>"
                    "      <parameter>"
                    "        <__ANY_CLASS_TYPE__/>"
                    "        <variable columnNr=\"22\" lineNr=\"0\">$parameter</variable>"
                    "      </parameter>"
                    "      <parameter is_reference=\"1\">"
                    "        <__ANY_CLASS_TYPE__/>"
                    "        <variable columnNr=\"35\" lineNr=\"0\">$variable</variable>"
                    "      </parameter>"
                    "    </parameter_list>"
                    "    <inner_statement_list/>"
                    "  </function_declaration>"
                    "</top_statement_list>"
    );}
    void test_instantiateObjectWithoutParameters() { TEST_PHP_PARSER(
        "<?php new classname;",
                    "<top_statement_list>"
                    "  <new>"
                    "    <namespace_name>"
                    "      <string columnNr=\"10\" lineNr=\"0\">classname</string>"
                    "    </namespace_name>"
                    "    <VOID/>"
                    "  </new>"
                    "</top_statement_list>"
    );}
    void test_arrayDefinitionInsideForeach() { TEST_PHP_PARSER(
        "<?php foreach(array('x') as $x);",
                    "<top_statement_list>"
                    "  <foreach>"
                    "    <array>"
                    "      <array_pair_list>"
                    "        <array_pair>"
                    "          <array_key/>"
                    "          <array_value>"
                    "            <constant_encapsed_string columnNr=\"20\" lineNr=\"0\">x</constant_encapsed_string>"
                    "          </array_value>"
                    "        </array_pair>"
                    "      </array_pair_list>"
                    "    </array>"
                    "    <variable columnNr=\"28\" lineNr=\"0\">$x</variable>"
                    "    <foreach_optional_arg/>"
                    "    <empty statement/>"
                    "  </foreach>"
                    "</top_statement_list>"
    );}
    void test_negativeConstant() { TEST_PHP_PARSER(
        "<?php class classname { const constname = -1; }",
                    "<top_statement_list>"
                    "  <class_declaration>"
                    "    <class columnNr=\"6\" lineNr=\"0\"/>"
                    "    <string columnNr=\"12\" lineNr=\"0\">classname</string>"
                    "    <extends/>"
                    "    <implements/>"
                    "    <class_statement_list>"
                    "      <class_constants>"
                    "        <string columnNr=\"30\" lineNr=\"0\">constname<negative><lnumber columnNr=\"43\" lineNr=\"0\">1</lnumber></negative></string>"
                    "      </class_constants>"
                    "    </class_statement_list>"
                    "  </class_declaration>"
                    "</top_statement_list>"
    );}
    void test_callMethodFromString() { TEST_PHP_PARSER(
        "<?php $object->$methodName();",
                    "<top_statement_list>"
                    "  <object_operator>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$object</variable>"
                    "    <variable columnNr=\"16\" lineNr=\"0\">$methodName</variable>"
                    "    <method_or_not>"
                    "      <function_call_parameter_list/>"
                    "    </method_or_not>"
                    "    <variable_properties/>"
                    "  </object_operator>"
                    "</top_statement_list>"
    );}
    void test_callMethodFromStringWrapped() { TEST_PHP_PARSER(
        "<?php $object->{$methodName}();",
                    "<top_statement_list>"
                    "  <object_operator>"
                    "    <variable columnNr=\"6\" lineNr=\"0\">$object</variable>"
                    "    <variable columnNr=\"17\" lineNr=\"0\">$methodName</variable>"
                    "    <method_or_not>"
                    "      <function_call_parameter_list/>"
                    "    </method_or_not>"
                    "    <variable_properties/>"
                    "  </object_operator>"
                    "</top_statement_list>"
    );}
    void test_listWithEmptyVariables() { TEST_PHP_PARSER(
        "<?php list(,,$x) = $y;",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <assignment_list>"
                    "      <VOID/>"
                    "      <VOID/>"
                    "      <variable columnNr=\"13\" lineNr=\"0\">$x</variable>"
                    "    </assignment_list>"
                    "    <variable columnNr=\"19\" lineNr=\"0\">$y</variable>"
                    "  </assignment>"
                    "</top_statement_list>"
    );}
    void test_variableRefByVariable() { TEST_PHP_PARSER(
        "<?php $$x;",
                    "<top_statement_list>"
                    "  <simple_indirect_reference>"
                    "    <variable columnNr=\"7\" lineNr=\"0\">$x</variable>"
                    "  </simple_indirect_reference>"
                    "</top_statement_list>"
    );}
    void test_whitespaceInHeredocStart() { TEST_PHP_PARSER(
        "<?php \n"
        "$x = <<< XXX\n"
        "asd\n"
        "XXX;\n",
                    "<top_statement_list>"
                    "  <assignment>"
                    "    <variable columnNr=\"0\" lineNr=\"1\">$x</variable>"
                    "    <encapsed_and_whitespace columnNr=\"0\" lineNr=\"2\">asd</encapsed_and_whitespace>"
                    "  </assignment>"
                    "</top_statement_list>"
    );}
    void test_alternativeIfElseEndifSyntax() { TEST_PHP_PARSER(
        "<?php if (1): echo $x; elseif (2): echo $z; else: echo $y; endif;",
                    "<top_statement_list>"
                    "  <if>"
                    "    <lnumber columnNr=\"10\" lineNr=\"0\">1</lnumber>"
                    "    <inner_statement_list>"
                    "      <echo>"
                    "        <echo_expr_list>"
                    "          <variable columnNr=\"19\" lineNr=\"0\">$x</variable>"
                    "        </echo_expr_list>"
                    "      </echo>"
                    "    </inner_statement_list>"
                    "    <elseif_list>"
                    "      <elseif>"
                    "        <lnumber columnNr=\"31\" lineNr=\"0\">2</lnumber>"
                    "        <inner_statement_list>"
                    "          <echo>"
                    "            <echo_expr_list>"
                    "              <variable columnNr=\"40\" lineNr=\"0\">$z</variable>"
                    "            </echo_expr_list>"
                    "          </echo>"
                    "        </inner_statement_list>"
                    "      </elseif>"
                    "    </elseif_list>"
                    "    <else>"
                    "      <inner_statement_list>"
                    "        <echo>"
                    "          <echo_expr_list>"
                    "            <variable columnNr=\"55\" lineNr=\"0\">$y</variable>"
                    "          </echo_expr_list>"
                    "        </echo>"
                    "      </inner_statement_list>"
                    "    </else>"
                    "  </if>"
                    "</top_statement_list>"
    );}
    void test_globalsUsage() { TEST_PHP_PARSER(
        "<?php function x() {global $x, $y;}",
                    "<top_statement_list>"
                    "  <function_declaration>"
                    "    <is_reference is_reference=\"0\"/>"
                    "    <string columnNr=\"15\" lineNr=\"0\">x</string>"
                    "    <parameter_list/>"
                    "    <inner_statement_list>"
                    "      <global>"
                    "        <global_var_list>"
                    "          <variable columnNr=\"27\" lineNr=\"0\">$x</variable>"
                    "          <variable columnNr=\"31\" lineNr=\"0\">$y</variable>"
                    "        </global_var_list>"
                    "      </global>"
                    "    </inner_statement_list>"
                    "  </function_declaration>"
                    "</top_statement_list>"
    );}
    void test_forWithEmptySpecifications() { TEST_PHP_PARSER(
        "<?php for(;;) ;",
                    "<top_statement_list>"
                    "  <for>"
                    "    <VOID/>"
                    "    <VOID/>"
                    "    <VOID/>"
                    "    <empty statement/>"
                    "  </for>"
                    "</top_statement_list>"
    );}
    void test_hereDocAsArgument() { TEST_PHP_PARSER(
        "<?php array('x'=> <<<XX\nasd\nXX\n) ;",
                    "<top_statement_list>"
                    "  <array>"
                    "    <array_pair_list>"
                    "      <array_pair>"
                    "        <array_key>"
                    "          <constant_encapsed_string columnNr=\"12\" lineNr=\"0\">x</constant_encapsed_string>"
                    "        </array_key>"
                    "        <array_value>"
                    "          <encapsed_and_whitespace columnNr=\"0\" lineNr=\"1\">asd</encapsed_and_whitespace>"
                    "        </array_value>"
                    "      </array_pair>"
                    "    </array_pair_list>"
                    "  </array>"
                    "</top_statement_list>"
    );}
    void test_danglingElse() { TEST_PHP_PARSER(
        "<?php if(1) if (2) {echo $y;} else {echo $z;}",
                    "<top_statement_list>"
                    "  <if>"
                    "    <lnumber columnNr=\"9\" lineNr=\"0\">1</lnumber>"
                    "    <if>"
                    "      <lnumber columnNr=\"16\" lineNr=\"0\">2</lnumber>"
                    "      <inner_statement_list>"
                    "        <echo>"
                    "          <echo_expr_list>"
                    "            <variable columnNr=\"25\" lineNr=\"0\">$y</variable>"
                    "          </echo_expr_list>"
                    "        </echo>"
                    "      </inner_statement_list>"
                    "      <elseif_list/>"
                    "      <else>"
                    "        <inner_statement_list>"
                    "          <echo>"
                    "            <echo_expr_list>"
                    "              <variable columnNr=\"41\" lineNr=\"0\">$z</variable>"
                    "            </echo_expr_list>"
                    "          </echo>"
                    "        </inner_statement_list>"
                    "      </else>"
                    "    </if>"
                    "    <elseif_list/>"
                    "    <else/>"
                    "  </if>"
                    "</top_statement_list>"
    );}
    void test_danglingElseIf() { TEST_PHP_PARSER(
        "<?php if(1) if (2) {echo $y;} elseif (3) {echo $z;}",
                    "<top_statement_list>"
                    "  <if>"
                    "    <lnumber columnNr=\"9\" lineNr=\"0\">1</lnumber>"
                    "    <if>"
                    "      <lnumber columnNr=\"16\" lineNr=\"0\">2</lnumber>"
                    "      <inner_statement_list>"
                    "        <echo>"
                    "          <echo_expr_list>"
                    "            <variable columnNr=\"25\" lineNr=\"0\">$y</variable>"
                    "          </echo_expr_list>"
                    "        </echo>"
                    "      </inner_statement_list>"
                    "      <elseif_list>"
                    "        <elseif>"
                    "          <lnumber columnNr=\"38\" lineNr=\"0\">3</lnumber>"
                    "          <inner_statement_list>"
                    "            <echo>"
                    "              <echo_expr_list>"
                    "                <variable columnNr=\"47\" lineNr=\"0\">$z</variable>"
                    "              </echo_expr_list>"
                    "            </echo>"
                    "          </inner_statement_list>"
                    "        </elseif>"
                    "      </elseif_list>"
                    "      <else/>"
                    "    </if>"
                    "    <elseif_list/>"
                    "    <else/>"
                    "  </if>"
                    "</top_statement_list>"
    );}
};

}

#endif // PHPPARSERTEST_H

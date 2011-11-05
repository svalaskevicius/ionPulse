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
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
        actions; \
        return;\
} while (0)

#define PRINT(QSTR) std::cout << QSTR.toStdString() << std::endl;
#define TEST_PHP_PARSER(CODE, ASTSTR) { \
    ASTRoot ret; \
    QVERIFY((ret = IonPhp::phpParser().parse(CODE)).data()); \
    QCOMPARE_3( \
        ret->toString(), \
        QString(ASTSTR), \
        PRINT(ret->toString()) \
    ); \
}


class PhpParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_openCloseNoEnd() { TEST_PHP_PARSER(
        "<?php ?><?php ?><?php ",
        "top_statement_list(empty statement; empty statement)"
    ); }
    void test_openCloseEnd() { TEST_PHP_PARSER(
        "<?php ?><?php ?><?php ?>",
        "top_statement_list(empty statement; empty statement; empty statement)"
    ); }
    void test_inlineHtml() { TEST_PHP_PARSER(
        "<?php ?>asd1<?php ?>asd2",
        "top_statement_list(empty statement; T_INLINE_HTML (@ 0:8) [text:asd1]; empty statement; T_INLINE_HTML (@ 0:20) [text:asd2])"
    ); }
    void test_scriptOpenTag() { TEST_PHP_PARSER(
        " <?<%<?php ?>asd?%<script language=\"php\">echo $d</script> asd",
        "top_statement_list(T_INLINE_HTML (@ 0:0) [text: <?<%]; empty statement; T_INLINE_HTML (@ 0:13) [text:asd?%]; echo(echo_expr_list(T_VARIABLE (@ 0:46) [text:$d])); T_INLINE_HTML (@ 0:57) [text: asd])"
    ); }
    void test_scriptOpenTagWOQuotes() { TEST_PHP_PARSER(
        "<?php \n?>asd<script \nlanguage=php>echo $d</script> asd",
        "top_statement_list(empty statement; T_INLINE_HTML (@ 1:2) [text:asd]; echo(echo_expr_list(T_VARIABLE (@ 2:18) [text:$d])); T_INLINE_HTML (@ 2:29) [text: asd])"
    ); }
    void test_scriptOpenTagWrong() { TEST_PHP_PARSER(
        "<?php ?>asd<script language=notphp>echo $d</script> asd",
        "top_statement_list(empty statement; T_INLINE_HTML (@ 0:8) [text:asd<script language=notphp>echo $d</script> asd])"
    ); }
    void test_variableAssignmentFromFncCall() { TEST_PHP_PARSER(
        "<?php $a = moo();",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; function_call(namespace_name(T_STRING (@ 0:11) [text:moo]); function_call_parameter_list)))"
    ); }
    void test_variableAssignmentFromFncCallWithParams() { TEST_PHP_PARSER(
        "<?php $a = moo(1, $s);",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; function_call(namespace_name(T_STRING (@ 0:11) [text:moo]); function_call_parameter_list(T_LNUMBER (@ 0:15) [text:1]; T_VARIABLE (@ 0:18) [text:$s]))))"
    ); }
    void test_functionDefinition() {
        TEST_PHP_PARSER(
            "<?php function myfnc() {}",
            "top_statement_list(function_declaration(is_reference [is_reference:0]; T_STRING (@ 0:15) [text:myfnc]; parameter_list; inner_statement_list))"
        );
    }
    void test_listDefinition() { TEST_PHP_PARSER(
        "<?php list($a, $b) = $c;",
        "top_statement_list(assignment(assignment_list(T_VARIABLE (@ 0:11) [text:$a]; T_VARIABLE (@ 0:15) [text:$b]); T_VARIABLE (@ 0:21) [text:$c]))"
    ); }
    void test_assignDefinition() { TEST_PHP_PARSER(
        "<?php $a = $c;",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$c]))"
    ); }
    void test_assignRefDefinition() { TEST_PHP_PARSER(
        "<?php $a = &$c;",
        "top_statement_list(assignment [is_reference:1](T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:12) [text:$c]))"
    ); }
    void test_assignRefNewClassDefinition() { TEST_PHP_PARSER(
        "<?php $a = & new asd;",
        "top_statement_list(assignment [is_reference:1](T_VARIABLE (@ 0:6) [text:$a]; T_NEW(namespace_name(T_STRING (@ 0:17) [text:asd]); VOID)))"
    ); }
    void test_assignRefNewClassWithParamsDefinition() { TEST_PHP_PARSER(
        "<?php $a = & new asd(1);",
        "top_statement_list(assignment [is_reference:1](T_VARIABLE (@ 0:6) [text:$a]; T_NEW(namespace_name(T_STRING (@ 0:17) [text:asd]); function_call_parameter_list(T_LNUMBER (@ 0:21) [text:1]))))"
    ); }
    void test_newClassDefinition() { TEST_PHP_PARSER(
        "<?php new asd(1, '2');",
        "top_statement_list(T_NEW(namespace_name(T_STRING (@ 0:10) [text:asd]); function_call_parameter_list(T_LNUMBER (@ 0:14) [text:1]; T_CONSTANT_ENCAPSED_STRING (@ 0:17) [text:2])))"
    ); }
    void test_cloneDefinition() { TEST_PHP_PARSER(
        "<?php clone $a;",
        "top_statement_list(clone(T_VARIABLE (@ 0:12) [text:$a]))"
    ); }
    void test_assignPlusDefinition() { TEST_PHP_PARSER(
        "<?php $a+=$b;",
        "top_statement_list(T_PLUS_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:10) [text:$b]))"
    ); }
    void test_assignMinusDefinition() { TEST_PHP_PARSER(
        "<?php $a-=$b;",
        "top_statement_list(T_MINUS_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:10) [text:$b]))"
    ); }
    void test_assignMulDefinition() { TEST_PHP_PARSER(
        "<?php $a*=$b;",
        "top_statement_list(T_MUL_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:10) [text:$b]))"
    ); }
    void test_assignDivDefinition() { TEST_PHP_PARSER(
        "<?php $a/=$b;",
        "top_statement_list(T_DIV_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:10) [text:$b]))"
    ); }
    void test_assignConcatDefinition() { TEST_PHP_PARSER(
        "<?php $a.=$b;",
        "top_statement_list(T_CONCAT_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:10) [text:$b]))"
    ); }
    void test_assignModDefinition() { TEST_PHP_PARSER(
        "<?php $a%=$b;",
        "top_statement_list(T_MOD_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:10) [text:$b]))"
    ); }
    void test_assignAndDefinition() { TEST_PHP_PARSER(
        "<?php $a&=$b;",
        "top_statement_list(T_AND_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:10) [text:$b]))"
    ); }
    void test_assignOrDefinition() { TEST_PHP_PARSER(
        "<?php $a|=$b;",
        "top_statement_list(T_OR_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:10) [text:$b]))"
    ); }
    void test_assignXorDefinition() { TEST_PHP_PARSER(
        "<?php $a^=$b;",
        "top_statement_list(T_XOR_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:10) [text:$b]))"
    ); }
    void test_assignShLeftDefinition() { TEST_PHP_PARSER(
        "<?php $a<<=$b;",
        "top_statement_list(T_SL_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_assignShRightDefinition() { TEST_PHP_PARSER(
        "<?php $a>>=$b;",
        "top_statement_list(T_SR_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_postIncDefinition() { TEST_PHP_PARSER(
        "<?php $a++;",
        "top_statement_list(POSTINC(T_VARIABLE (@ 0:6) [text:$a]))"
    ); }
    void testPhpParserp_preIncDefinition() { TEST_PHP_PARSER(
        "<?php ++$b;",
        "top_statement_list(PREINC(T_VARIABLE (@ 0:8) [text:$b]))"
    ); }
    void test_postDecDefinition() { TEST_PHP_PARSER(
        "<?php $a--;",
        "top_statement_list(POSTDEC(T_VARIABLE (@ 0:6) [text:$a]))"
    ); }
    void test_preDecDefinition() { TEST_PHP_PARSER(
        "<?php --$a;",
        "top_statement_list(PREDEC(T_VARIABLE (@ 0:8) [text:$a]))"
    ); }
    void test_boolOrDefinition() { TEST_PHP_PARSER(
        "<?php $a || $b;",
        "top_statement_list(T_BOOLEAN_OR(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:12) [text:$b]))"
    ); }
    void test_boolAndDefinition() { TEST_PHP_PARSER(
        "<?php $a && $b;",
        "top_statement_list(T_BOOLEAN_AND(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:12) [text:$b]))"
    ); }
    void test_logicalOrDefinition() { TEST_PHP_PARSER(
        "<?php $a or $b;",
        "top_statement_list(T_LOGICAL_OR(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:12) [text:$b]))"
    ); }
    void test_logicalAndDefinition() { TEST_PHP_PARSER(
        "<?php $a and $b;",
        "top_statement_list(T_LOGICAL_AND(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:13) [text:$b]))"
    ); }
    void test_logicalXorDefinition() { TEST_PHP_PARSER(
        "<?php $a xor $b;",
        "top_statement_list(T_LOGICAL_XOR(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:13) [text:$b]))"
    ); }
    void test_orDefinition() { TEST_PHP_PARSER(
        "<?php $a | $b;",
        "top_statement_list(T_BINARY_OR(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_andDefinition() { TEST_PHP_PARSER(
        "<?php $a & $b;",
        "top_statement_list(T_BINARY_AND(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_xorDefinition() { TEST_PHP_PARSER(
        "<?php $a ^ $b;",
        "top_statement_list(T_BINARY_XOR(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_concatDefinition() { TEST_PHP_PARSER(
        "<?php $a . $b;",
        "top_statement_list(T_CONCAT(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_plusDefinition() { TEST_PHP_PARSER(
        "<?php $a + $b;",
        "top_statement_list(T_PLUS(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_minusDefinition() { TEST_PHP_PARSER(
        "<?php $a - $b;",
        "top_statement_list(T_MINUS(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_mulDefinition() { TEST_PHP_PARSER(
        "<?php $a * $b;",
        "top_statement_list(T_MUL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_divDefinition() { TEST_PHP_PARSER(
        "<?php $a / $b;",
        "top_statement_list(T_DIV(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_modDefinition() { TEST_PHP_PARSER(
        "<?php $a % $b;",
        "top_statement_list(T_MOD(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_shLeftDefinition() { TEST_PHP_PARSER(
        "<?php $a << $b;",
        "top_statement_list(T_SHIFT_LEFT(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:12) [text:$b]))"
    ); }
    void test_shRightDefinition() { TEST_PHP_PARSER(
        "<?php $a >> $b;",
        "top_statement_list(T_SHIFT_RIGHT(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:12) [text:$b]))"
    ); }
    void test_negateDefinition() { TEST_PHP_PARSER(
        "<?php !$a;",
        "top_statement_list(T_NEGATE(T_VARIABLE (@ 0:7) [text:$a]))"
    ); }
    void test_inverseDefinition() { TEST_PHP_PARSER(
        "<?php ~$a;",
        "top_statement_list(T_INVERSE(T_VARIABLE (@ 0:7) [text:$a]))"
    ); }
    void test_isIdenticalDefinition() { TEST_PHP_PARSER(
        "<?php $a === $b;",
        "top_statement_list(T_IS_IDENTICAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:13) [text:$b]))"
    ); }
    void test_isNotIdenticalDefinition() { TEST_PHP_PARSER(
        "<?php $a !== $b;",
        "top_statement_list(T_IS_NOT_IDENTICAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:13) [text:$b]))"
    ); }
    void test_isEqualDefinition() { TEST_PHP_PARSER(
        "<?php $a == $b;",
        "top_statement_list(T_IS_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:12) [text:$b]))"
    ); }
    void test_isNotEqualDefinition() { TEST_PHP_PARSER(
        "<?php $a != $b;",
        "top_statement_list(T_IS_NOT_EQUAL(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:12) [text:$b]))"
    ); }
    void test_lessDefinition() { TEST_PHP_PARSER(
        "<?php $a < $b;",
        "top_statement_list(T_LESSTHAN(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_lessEqDefinition() { TEST_PHP_PARSER(
        "<?php $a <= $b;",
        "top_statement_list(T_LESSTHAN_EQ(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:12) [text:$b]))"
    ); }
    void test_moreDefinition() { TEST_PHP_PARSER(
        "<?php $a > $b;",
        "top_statement_list(T_GREATERTHAN(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]))"
    ); }
    void test_moreEqDefinition() { TEST_PHP_PARSER(
        "<?php $a >= $b;",
        "top_statement_list(T_GREATERTHAN_EQ(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:12) [text:$b]))"
    ); }
    void test_instanceOfDefinition() { TEST_PHP_PARSER(
        "<?php $a instanceof B;",
        "top_statement_list(T_INSTANCEOF(T_VARIABLE (@ 0:6) [text:$a]; namespace_name(T_STRING (@ 0:20) [text:B])))"
    ); }
    void test_parenthesesDefinition() { TEST_PHP_PARSER(
        "<?php ($a);",
        "top_statement_list(T_VARIABLE (@ 0:7) [text:$a])"
    ); }
    void test_ternaryDefinition() { TEST_PHP_PARSER(
        "<?php $a ? $b : $c ;",
        "top_statement_list(TERNARYOP(T_VARIABLE (@ 0:6) [text:$a]; T_VARIABLE (@ 0:11) [text:$b]; T_VARIABLE (@ 0:16) [text:$c]))"
    ); }
    void test_ternaryPartialDefinition() { TEST_PHP_PARSER(
        "<?php $a ? : $c ;",
        "top_statement_list(TERNARYOP(T_VARIABLE (@ 0:6) [text:$a]; VOID; T_VARIABLE (@ 0:13) [text:$c]))"
    ); }
    void test_castIntDefinition() { TEST_PHP_PARSER(
        "<?php (int) $a ;",
        "top_statement_list(T_INT_CAST(T_VARIABLE (@ 0:12) [text:$a]))"
    ); }
    void test_castDoubleDefinition() { TEST_PHP_PARSER(
        "<?php (double) $a ;",
        "top_statement_list(T_DOUBLE_CAST(T_VARIABLE (@ 0:15) [text:$a]))"
    ); }
    void test_castStringDefinition() { TEST_PHP_PARSER(
        "<?php (string) $a ;",
        "top_statement_list(T_STRING_CAST(T_VARIABLE (@ 0:15) [text:$a]))"
    ); }
    void test_castArrayDefinition() { TEST_PHP_PARSER(
        "<?php (array) $a ;",
        "top_statement_list(T_ARRAY_CAST(T_VARIABLE (@ 0:14) [text:$a]))"
    ); }
    void test_castObjectDefinition() { TEST_PHP_PARSER(
        "<?php (object) $a ;",
        "top_statement_list(T_OBJECT_CAST(T_VARIABLE (@ 0:15) [text:$a]))"
    ); }
    void test_castBoolDefinition() { TEST_PHP_PARSER(
        "<?php (bool) $a ;",
        "top_statement_list(T_BOOL_CAST(T_VARIABLE (@ 0:13) [text:$a]))"
    ); }
    void test_castUnsetDefinition() { TEST_PHP_PARSER(
        "<?php (unset) $a ;",
        "top_statement_list(T_UNSET_CAST(T_VARIABLE (@ 0:14) [text:$a]))"
    ); }
    void test_exitDefinition() { TEST_PHP_PARSER(
        "<?php exit (1) ;",
        "top_statement_list(T_EXIT(T_LNUMBER (@ 0:12) [text:1]))"
    ); }
    void test_silenceDefinition() { TEST_PHP_PARSER(
        "<?php @$a ;",
        "top_statement_list(SILENCE(T_VARIABLE (@ 0:7) [text:$a]))"
    ); }
    void test_arrayDefinition() { TEST_PHP_PARSER(
        "<?php array(1=>$a, &$b, 'c'=>3+2,) ;",
        "top_statement_list(T_ARRAY(array_pair_list(array_pair(array_key(T_LNUMBER (@ 0:12) [text:1]); array_value(T_VARIABLE (@ 0:15) [text:$a])); array_pair(array_key; array_value [is_reference:1](T_VARIABLE (@ 0:20) [text:$b])); array_pair(array_key(T_CONSTANT_ENCAPSED_STRING (@ 0:24) [text:c]); array_value(T_PLUS(T_LNUMBER (@ 0:29) [text:3]; T_LNUMBER (@ 0:31) [text:2]))))))"
    ); }
    void test_arrayDefinitionRef() { TEST_PHP_PARSER(
        "<?php array(&$a, &$b) ;",
        "top_statement_list(T_ARRAY(array_pair_list(array_pair(array_key; array_value [is_reference:1](T_VARIABLE (@ 0:13) [text:$a])); array_pair(array_key; array_value [is_reference:1](T_VARIABLE (@ 0:18) [text:$b])))))"
    ); }
    void test_backticksDefinition() { TEST_PHP_PARSER(
        "<?php `$a boo $2 moo` ;",
        "top_statement_list(BACKTICKS(encaps_list(T_VARIABLE (@ 0:7) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 0:9) [text: boo $2 moo])))"
    ); }
    void test_backticksStartsWithBackslash() { TEST_PHP_PARSER(
        "<?php `\\$a boo moo` ;",
        "top_statement_list(BACKTICKS(T_ENCAPSED_AND_WHITESPACE (@ 0:7) [text:\\$a boo moo]))"
    ); }
    void test_backticksContainsBackslash() { TEST_PHP_PARSER(
        "<?php `asd\\$a boo {}moo` ;",
        "top_statement_list(BACKTICKS(T_ENCAPSED_AND_WHITESPACE (@ 0:7) [text:asd\\$a boo {}moo]))"
    ); }
    void test_backticksWithTwoVariables() { TEST_PHP_PARSER(
        "<?php `asd$a $boo moo` ;",
        "top_statement_list(BACKTICKS(encaps_list(T_ENCAPSED_AND_WHITESPACE (@ 0:7) [text:asd]; T_VARIABLE (@ 0:10) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 0:12) [text: ]; T_VARIABLE (@ 0:13) [text:$boo]; T_ENCAPSED_AND_WHITESPACE (@ 0:17) [text: moo])))"
    ); }
    void test_backticksWithTwoVariablesAndSecondIsBracketed() { TEST_PHP_PARSER(
        "<?php `asd$a {$boo} moo` ;",
        "top_statement_list(BACKTICKS(encaps_list(T_ENCAPSED_AND_WHITESPACE (@ 0:7) [text:asd]; T_VARIABLE (@ 0:10) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 0:12) [text: ]; T_VARIABLE (@ 0:15) [text:$boo]; T_ENCAPSED_AND_WHITESPACE (@ 0:20) [text: moo])))"
    ); }
    void test_printDefinition() { TEST_PHP_PARSER(
        "<?php print $x ;",
        "top_statement_list(T_PRINT(T_VARIABLE (@ 0:12) [text:$x]))"
    ); }
    void test_lambdaFncDefinition() { TEST_PHP_PARSER(
        "<?php function ($a) { return $c; } ;",
        "top_statement_list(LAMBDA_FUNCTION(is_reference [is_reference:0]; parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE (@ 0:16) [text:$a])); T_USE; inner_statement_list(return(T_VARIABLE (@ 0:29) [text:$c]))))"
    ); }
    void test_lambdaFncUseDefinition() { TEST_PHP_PARSER(
        "<?php function ($a) use ($b) { return $c; } ;",
        "top_statement_list(LAMBDA_FUNCTION(is_reference [is_reference:0]; parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE (@ 0:16) [text:$a])); T_USE(lexical_var_list(T_VARIABLE (@ 0:25) [text:$b])); inner_statement_list(return(T_VARIABLE (@ 0:38) [text:$c]))))"
    ); }
    void test_lambdaFncRefDefinition() { TEST_PHP_PARSER(
        "<?php function & ($a) use ($b, &$c) { return $c; } ;",
        "top_statement_list(LAMBDA_FUNCTION(is_reference [is_reference:1]; parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE (@ 0:18) [text:$a])); T_USE(lexical_var_list(T_VARIABLE (@ 0:27) [text:$b]; T_VARIABLE (@ 0:32) [is_reference:1, text:$c])); inner_statement_list(return(T_VARIABLE (@ 0:45) [text:$c]))))"
    ); }
    void test_doubleQuotesConst() { TEST_PHP_PARSER(
        "<?php $a = \"\\rtest $ {} string\\n\" ;",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_CONSTANT_ENCAPSED_STRING (@ 0:11) [text:\\rtest $ {} string\\n]))"
    ); }
    void test_doubleQuotesWithVars() { TEST_PHP_PARSER(
        "<?php $a = \"\\ntest $moo more text\n\\n {$boo}${buka}s${aa[2]} {$o->prop}trin $ {} g\" ;",
        "top_statement_list("
            "assignment("
                "T_VARIABLE (@ 0:6) [text:$a]; "
                "doubleQuotes("
                    "encaps_list("
                        "T_ENCAPSED_AND_WHITESPACE (@ 0:12) [text:\\ntest ]; "
                        "T_VARIABLE (@ 0:19) [text:$moo]; "
                        "T_ENCAPSED_AND_WHITESPACE (@ 0:23) [text: more text\n\\n ]; "
                        "T_VARIABLE (@ 1:5) [text:$boo]; "
                        "T_STRING_VARNAME (@ 1:12) [text:buka]; "
                        "T_ENCAPSED_AND_WHITESPACE (@ 1:17) [text:s]; "
                        "offset(T_STRING_VARNAME (@ 1:20) [text:aa]; T_LNUMBER (@ 1:23) [text:2]); "
                        "T_ENCAPSED_AND_WHITESPACE (@ 1:26) [text: ]; "
                        "T_OBJECT_OPERATOR("
                            "T_VARIABLE (@ 1:29) [text:$o]; "
                            "T_STRING (@ 1:33) [text:prop]; "
                            "method_or_not; "
                            "variable_properties"
                        "); "
                        "T_ENCAPSED_AND_WHITESPACE (@ 1:38) [text:trin $ {} g]"
                    ")"
                ")"
            ")"
        ")"
    ); }
    void test_doubleQuotesWithVarsStartsBracketed() { TEST_PHP_PARSER(
        "<?php $a = \"\\ntest {$moo} more text\n\\n {$boo}${buka}s${aa[2]} {$o->prop}trin $ {} g\" ;",
        "top_statement_list("
            "assignment("
                "T_VARIABLE (@ 0:6) [text:$a]; "
                "doubleQuotes("
                    "encaps_list("
                        "T_ENCAPSED_AND_WHITESPACE (@ 0:12) [text:\\ntest ]; "
                        "T_VARIABLE (@ 0:21) [text:$moo]; "
                        "T_ENCAPSED_AND_WHITESPACE (@ 0:26) [text: more text\n\\n ]; "
                        "T_VARIABLE (@ 1:5) [text:$boo]; "
                        "T_STRING_VARNAME (@ 1:12) [text:buka]; "
                        "T_ENCAPSED_AND_WHITESPACE (@ 1:17) [text:s]; "
                        "offset(T_STRING_VARNAME (@ 1:20) [text:aa]; T_LNUMBER (@ 1:23) [text:2]); "
                        "T_ENCAPSED_AND_WHITESPACE (@ 1:26) [text: ]; "
                        "T_OBJECT_OPERATOR("
                            "T_VARIABLE (@ 1:29) [text:$o]; "
                            "T_STRING (@ 1:33) [text:prop]; "
                            "method_or_not; "
                            "variable_properties"
                        "); "
                        "T_ENCAPSED_AND_WHITESPACE (@ 1:38) [text:trin $ {} g]"
                    ")"
                ")"
            ")"
        ")"
    ); }
    void test_doubleQuotesWithNonVars() { TEST_PHP_PARSER(
        "<?php $a = \"test $12 more text\" ;",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_CONSTANT_ENCAPSED_STRING (@ 0:11) [text:test $12 more text]))"
    ); }
    void test_singleQuotesEscaping() { TEST_PHP_PARSER(
        "<?php $a = 'test\\' $a more text' ;",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_CONSTANT_ENCAPSED_STRING (@ 0:11) [text:test\\' $a more text]))"
    ); }
    void test_hereDoc() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\ntex{t\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 1:0) [text:tex{t]))"
    ); }
    void test_hereDocWithWindowsEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\r\ntext\r\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 1:0) [text:text]))"
    ); }
    void test_hereDocWithMacEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\rtext\rMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 0:20) [text:text]))"
    ); }
    void test_hereDocMultiline() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\rtext\na\rs\r\nd\n\rMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 0:20) [text:text\na\rs\r\nd\n]))"
    ); }
    void test_hereDocWithVars() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\ntex$var1 {$var2} {$var3[$i]} {$var4->prop} ${var5[$i]}$var6[$i]$var7->prop t\nMYDOC; ",
        "top_statement_list("
            "assignment("
                "T_VARIABLE (@ 0:6) [text:$a]; "
                "hereDoc("
                    "encaps_list("
                        "T_ENCAPSED_AND_WHITESPACE (@ 1:0) [text:tex]; "
                        "T_VARIABLE (@ 1:3) [text:$var1]; "
                        "T_ENCAPSED_AND_WHITESPACE (@ 1:8) [text: ]; "
                        "T_VARIABLE (@ 1:11) [text:$var2]; "
                        "T_ENCAPSED_AND_WHITESPACE (@ 1:17) [text: ]; "
                        "offset(T_VARIABLE (@ 1:20) [text:$var3]; T_VARIABLE (@ 1:26) [text:$i]); "
                        "T_ENCAPSED_AND_WHITESPACE (@ 1:30) [text: ]; "
                        "T_OBJECT_OPERATOR(T_VARIABLE (@ 1:33) [text:$var4]; T_STRING (@ 1:40) [text:prop]; method_or_not; variable_properties); "
                        "T_ENCAPSED_AND_WHITESPACE (@ 1:45) [text: ]; "
                        "offset(T_STRING_VARNAME (@ 1:48) [text:var5]; T_VARIABLE (@ 1:53) [text:$i]); "
                        "offset(T_VARIABLE (@ 1:57) [text:$var6]; T_VARIABLE (@ 1:64) [text:$i]); "
                        "T_OBJECT_OPERATOR(T_VARIABLE (@ 1:67) [text:$var7]; T_STRING (@ 1:77) [text:prop]); "
                        "T_ENCAPSED_AND_WHITESPACE (@ 1:81) [text: t]"
                    ")"
                ")"
            ")"
        ")"
    ); }
    void test_hereDocWithQuoteLabels() { TEST_PHP_PARSER(
        "<?php $a = <<<\"MYDOC\"\ntext\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 1:0) [text:text]))"
    ); }
    void test_hereDocWithNonVars() { TEST_PHP_PARSER(
        "<?php $a = <<<\"MYDOC\"\ntex $123 t\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 1:0) [text:tex $123 t]))"
    ); }
    void test_nowDoc() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\ntext$nonvar w\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 1:0) [text:text$nonvar w]))"
    ); }
    void test_nowDocWithWindowsEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\r\ntext\r\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 1:0) [text:text]))"
    ); }
    void test_nowDocWithMacEndLines() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\rtext\rMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 0:22) [text:text]))"
    ); }
    void test_nowDocMultiline() { TEST_PHP_PARSER(
        "<?php $a = <<<'MYDOC'\rtext\na\rs\r\nd\n\rMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE (@ 0:6) [text:$a]; T_ENCAPSED_AND_WHITESPACE (@ 0:22) [text:text\na\rs\r\nd\n]))"
    ); }
    void test_singleLineComment() { TEST_PHP_PARSER(
        "<?php //$a =? <<<'MYDOC'\n$s=1; // asd \n // //..  ",
        "top_statement_list(assignment(T_VARIABLE (@ 1:0) [text:$s]; T_LNUMBER (@ 1:3) [text:1]))"
    ); }
    void test_singleLineCommentWindowsEndline() { TEST_PHP_PARSER(
        "<?php //$a = <<<'MYDOC'\r\n$s=1; // asd \r\n // //..  ",
        "top_statement_list(assignment(T_VARIABLE (@ 1:0) [text:$s]; T_LNUMBER (@ 1:3) [text:1]))"
    ); }
    void test_singleLineCommentMacEndline() { TEST_PHP_PARSER(
        "<?php //$a = <<<'MYDOC'\r$s=1; // asd \r // //..  ",
        "top_statement_list(assignment(T_VARIABLE (@ 0:24) [text:$s]; T_LNUMBER (@ 0:27) [text:1]))"
    ); }
    void test_singleLineCommentScriptStop() { TEST_PHP_PARSER(
        "<?php //$a =? <<<'MYDOC'?>$s=1; // asd \n // //..  ",
        "top_statement_list(empty statement; T_INLINE_HTML (@ 0:26) [text:$s=1; // asd \n // //..  ])"
    ); }
    void test_singleLineHashComment() { TEST_PHP_PARSER(
        "<?php #$a = <<<'MYDOC'\n$s=1; # asd \n # /..  ",
        "top_statement_list(assignment(T_VARIABLE (@ 1:0) [text:$s]; T_LNUMBER (@ 1:3) [text:1]))"
    ); }
    void test_multiLineComment() { TEST_PHP_PARSER(
        "<?php /* $a = *<<<'MYDOC'\n$s=1; # asd \n # /.. */ $b=2; ",
        "top_statement_list(assignment(T_VARIABLE (@ 2:10) [text:$b]; T_LNUMBER (@ 2:13) [text:2]))"
    ); }
    void test_docComment() { TEST_PHP_PARSER(
        "<?php /** lala */ ",
        "top_statement_list"
    ); }
    void test_numbers() { TEST_PHP_PARSER(
        "<?php $a=12+12.2+.2+0x02+0123+2e1;",
        "top_statement_list("
            "assignment("
                "T_VARIABLE (@ 0:6) [text:$a]; "
                "T_PLUS("
                    "T_PLUS("
                        "T_PLUS("
                            "T_PLUS("
                                "T_PLUS("
                                    "T_LNUMBER (@ 0:9) [text:12]; "
                                    "T_DNUMBER (@ 0:12) [text:12.2]"
                                "); "
                                "T_DNUMBER (@ 0:17) [text:.2]"
                            "); "
                            "T_LNUMBER (@ 0:20) [text:0x02]"
                        "); "
                        "T_LNUMBER (@ 0:25) [text:0123]"
                    "); "
                    "T_DNUMBER (@ 0:30) [text:2e1]"
                ")"
            ")"
        ")"
    ); }
    void test_phpConstants() { TEST_PHP_PARSER(
        "<?php $a=__CLASS__.__FUNCTION__.__METHOD__.__LINE__.__FILE__.__DIR__.__NAMESPACE__;",
        "top_statement_list("
            "assignment("
                "T_VARIABLE (@ 0:6) [text:$a]; "
                "T_CONCAT("
                    "T_CONCAT("
                        "T_CONCAT("
                            "T_CONCAT("
                                "T_CONCAT("
                                    "T_CONCAT("
                                        "__CLASS__ (@ 0:9); "
                                        "__FUNCTION__ (@ 0:19)"
                                    "); "
                                    "__METHOD__ (@ 0:32)"
                                "); "
                                "__LINE__ (@ 0:43)"
                            "); "
                            "__FILE__ (@ 0:52)"
                        "); "
                        "__DIR__ (@ 0:61)"
                    "); "
                    "__NAMESPACE__ (@ 0:69)"
                ")"
            ")"
        ")"
    ); }
    void test_classDefinition() { TEST_PHP_PARSER(
        "<?php class A { }",
        "top_statement_list(class_declaration(class (@ 0:6); T_STRING (@ 0:12) [text:A]; extends; implements; class_statement_list))"
    );}
    void test_classExtendingOtherDefinition() { TEST_PHP_PARSER(
        "<?php class A extends B { }",
        "top_statement_list(class_declaration(class (@ 0:6); T_STRING (@ 0:12) [text:A]; extends(namespace_name(T_STRING (@ 0:22) [text:B])); implements; class_statement_list))"
    );}
    void test_classImplementingInterfaceDefinition() { TEST_PHP_PARSER(
        "<?php class A implements I { }",
        "top_statement_list(class_declaration(class (@ 0:6); T_STRING (@ 0:12) [text:A]; extends; implements(interfaceList(namespace_name(T_STRING (@ 0:25) [text:I]))); class_statement_list))"
    );}
    void test_abstractClassDefinition() { TEST_PHP_PARSER(
        "<?php abstract class A { }",
        "top_statement_list(class_declaration(class (@ 0:15) [type:abstract]; T_STRING (@ 0:21) [text:A]; extends; implements; class_statement_list))"
    );}
    void test_finalClassDefinition() { TEST_PHP_PARSER(
        "<?php final class A { }",
        "top_statement_list(class_declaration(class (@ 0:12) [type:final]; T_STRING (@ 0:18) [text:A]; extends; implements; class_statement_list))"
    );}
    void test_interfaceDefinition() { TEST_PHP_PARSER(
        "<?php interface A { }",
        "top_statement_list(interface_declaration(interface (@ 0:6); T_STRING (@ 0:16) [text:A]; extends; class_statement_list))"
    );}
    void test_interfaceInheritanceDefinition() { TEST_PHP_PARSER(
        "<?php interface A extends B, C { }",
        "top_statement_list(interface_declaration(interface (@ 0:6); T_STRING (@ 0:16) [text:A]; extends(interfaceList(namespace_name(T_STRING (@ 0:26) [text:B]); namespace_name(T_STRING (@ 0:29) [text:C]))); class_statement_list))"
    );}

    void test_statementIf() { TEST_PHP_PARSER(
        "<?php if(1) {echo 'x';} elseif (2) { echo 'y'; } else echo 'z';",
        "top_statement_list("
            "if("
                "T_LNUMBER (@ 0:9) [text:1]; "
                "inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING (@ 0:18) [text:x]))); "
                "elseif_list("
                    "elseif("
                        "T_LNUMBER (@ 0:32) [text:2]; "
                        "inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING (@ 0:42) [text:y])))"
                    ")"
                "); "
                "else("
                    "echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING (@ 0:59) [text:z]))"
                ")"
            ")"
        ")"
    );}
    void test_statementWhile() { TEST_PHP_PARSER(
        "<?php while(1) {echo 'x';}",
        "top_statement_list(while(T_LNUMBER (@ 0:12) [text:1]; inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING (@ 0:21) [text:x])))))"
    );}
    void test_statementDoWhile() { TEST_PHP_PARSER(
        "<?php do {echo 'x';} while(1);",
        "top_statement_list(dowhile(inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING (@ 0:15) [text:x]))); T_LNUMBER (@ 0:27) [text:1]))"
    );}
    void test_statementFor() { TEST_PHP_PARSER(
        "<?php for($i=0;$i<1;$i++) {echo 'x';}",
        "top_statement_list("
            "for("
                "assignment(T_VARIABLE (@ 0:10) [text:$i]; T_LNUMBER (@ 0:13) [text:0]); "
                "T_LESSTHAN(T_VARIABLE (@ 0:15) [text:$i]; T_LNUMBER (@ 0:18) [text:1]); "
                "POSTINC(T_VARIABLE (@ 0:20) [text:$i]); "
                "inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING (@ 0:32) [text:x])))"
            ")"
        ")"
    );}
    void test_statementSwitch() { TEST_PHP_PARSER(
        "<?php switch($i) {case 'x': echo 'y'; default: echo 'z';}",
        "top_statement_list("
            "switch("
                "T_VARIABLE (@ 0:13) [text:$i]; "
                "case_list("
                    "case("
                        "T_CONSTANT_ENCAPSED_STRING (@ 0:23) [text:x]; "
                        "inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING (@ 0:33) [text:y])))"
                    "); "
                    "default_case(inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING (@ 0:52) [text:z]))))"
                ")"
            ")"
        ")"
    );}
    void test_statementBreak() { TEST_PHP_PARSER(
        "<?php break;",
        "top_statement_list(break)"
    );}
    void test_statementContinue() { TEST_PHP_PARSER(
        "<?php continue;",
        "top_statement_list(continue)"
    );}
    void test_statementReturn() { TEST_PHP_PARSER(
        "<?php function x(){return 2;}",
        "top_statement_list(function_declaration(is_reference [is_reference:0]; T_STRING (@ 0:15) [text:x]; parameter_list; inner_statement_list(return(T_LNUMBER (@ 0:26) [text:2]))))"
    );}
    void test_statementStaticVars() { TEST_PHP_PARSER(
        "<?php function x(){static $a = 2; return $a;}",
        "top_statement_list(function_declaration(is_reference [is_reference:0]; T_STRING (@ 0:15) [text:x]; parameter_list; inner_statement_list(static(T_VARIABLE (@ 0:26) [text:$a]); return(T_VARIABLE (@ 0:41) [text:$a]))))"
    );}
    void test_statementForeach() { TEST_PHP_PARSER(
        "<?php foreach($ar as $a=>$v) {echo 'x';}",
        "top_statement_list(foreach(T_VARIABLE (@ 0:14) [text:$ar]; T_VARIABLE (@ 0:21) [text:$a]; foreach_optional_arg(T_VARIABLE (@ 0:25) [text:$v]); inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING (@ 0:35) [text:x])))))"
    );}
    void test_statementForeachRef() { TEST_PHP_PARSER(
        "<?php foreach($ar as $a=>&$v) {echo 'x';}",
        "top_statement_list(foreach(T_VARIABLE (@ 0:14) [text:$ar]; T_VARIABLE (@ 0:21) [text:$a]; foreach_optional_arg(T_VARIABLE (@ 0:26) [is_reference:1, text:$v]); inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING (@ 0:36) [text:x])))))"
    );}
    void test_statementThrow() { TEST_PHP_PARSER(
        "<?php\nthrow new Exception();",
        "top_statement_list(throw(T_NEW(namespace_name(T_STRING (@ 1:10) [text:Exception]); function_call_parameter_list)))"
    );}
    void test_statementThyCatchCatch() { TEST_PHP_PARSER(
        "<?php try {echo 'x';} catch (MyException $e) {} catch (Exception $e) {}",
        "top_statement_list("
            "try("
                "inner_statement_list(echo(echo_expr_list(T_CONSTANT_ENCAPSED_STRING (@ 0:16) [text:x]))); "
                "catch("
                    "namespace_name(T_STRING (@ 0:29) [text:MyException]); "
                    "T_VARIABLE (@ 0:41) [text:$e]; "
                    "inner_statement_list"
                "); "
                "additional_catches("
                    "catch("
                        "namespace_name(T_STRING (@ 0:55) [text:Exception]); "
                        "T_VARIABLE (@ 0:65) [text:$e]; "
                        "inner_statement_list"
                    ")"
                ")"
            ")"
        ")"
    );}
    void test_parsingRequire() { TEST_PHP_PARSER(
        "<?php require 'file.php';\n",
        "top_statement_list(require(T_CONSTANT_ENCAPSED_STRING (@ 0:14) [text:file.php]))"
    );}
    void test_classMethod() { TEST_PHP_PARSER(
        "<?php\n"
        "class my_class\n"
        "{\n"
        "    public function my_method()\n"
        "    {\n"
        "    }\n"
        "}",
        "top_statement_list(class_declaration(class (@ 1:0); T_STRING (@ 1:6) [text:my_class]; extends; implements; class_statement_list(METHOD(MODIFIERS(T_PUBLIC); is_reference [is_reference:0]; T_STRING (@ 3:20) [text:my_method]; parameter_list; METHOD_BODY(inner_statement_list)))))"
    );}
    void test_classProperty() { TEST_PHP_PARSER(
        "<?php\n"
        "class my_class\n"
        "{\n"
        "    public $prop = 1;\n"
        "}",
        "top_statement_list(class_declaration(class (@ 1:0); T_STRING (@ 1:6) [text:my_class]; extends; implements; class_statement_list(PROPERTY(MODIFIERS(T_PUBLIC); class_properties(T_VARIABLE (@ 3:11) [text:$prop](T_LNUMBER (@ 3:19) [text:1]))))))"
    );}
    void test_classPropertyList() { TEST_PHP_PARSER(
        "<?php\n"
        "class my_class\n"
        "{\n"
        "    private final $prop = 1, $x, $y = array('c'=>3,);\n"
        "}",
        "top_statement_list(class_declaration(class (@ 1:0); T_STRING (@ 1:6) [text:my_class]; extends; implements; class_statement_list("
            "PROPERTY(MODIFIERS(T_PRIVATE; T_FINAL); class_properties("
                "T_VARIABLE (@ 3:18) [text:$prop](T_LNUMBER (@ 3:26) [text:1]); "
                "T_VARIABLE (@ 3:29) [text:$x]; "
                "T_VARIABLE (@ 3:33) [text:$y](T_ARRAY(array_pair_list(array_pair(array_key(T_CONSTANT_ENCAPSED_STRING (@ 3:44) [text:c]); array_value(T_LNUMBER (@ 3:49) [text:3]))))))))))"
    );}
    void test_classConstant() { TEST_PHP_PARSER(
        "<?php\n"
        "class my_class\n"
        "{\n"
        "    const MYCONST = 1, OTHER=2;\n"
        "}",
        "top_statement_list(class_declaration(class (@ 1:0); T_STRING (@ 1:6) [text:my_class]; extends; implements; class_statement_list(class_constants(T_STRING (@ 3:10) [text:MYCONST](T_LNUMBER (@ 3:20) [text:1]); T_STRING (@ 3:23) [text:OTHER](T_LNUMBER (@ 3:29) [text:2])))))"
    );}
    void test_methodCallWithNegativeNumber() { TEST_PHP_PARSER(
        "<?php $object->method(-2);",
        "top_statement_list(T_OBJECT_OPERATOR(T_VARIABLE (@ 0:6) [text:$object]; T_STRING (@ 0:15) [text:method]; method_or_not(function_call_parameter_list(T_NEGATIVE(T_LNUMBER (@ 0:23) [text:2]))); variable_properties))"
    );}
    void test_negativeNrAsOperation() { TEST_PHP_PARSER(
        "<?php $x-1;",
        "top_statement_list(T_MINUS(T_VARIABLE (@ 0:6) [text:$x]; T_LNUMBER (@ 0:9) [text:1]))"
    );}
    void test_addToArray() { TEST_PHP_PARSER(
        "<?php $array[] = $x;",
        "top_statement_list(assignment(offset(T_VARIABLE (@ 0:6) [text:$array]; empty_offset); T_VARIABLE (@ 0:17) [text:$x]))"
    );}
    void test_exitWithoutParameters() { TEST_PHP_PARSER(
        "<?php exit;",
        "top_statement_list(T_EXIT(VOID))"
    );}

    void test_functionParametersDefinitionAllowsDefaults() { TEST_PHP_PARSER(
        "<?php function method($parameter, $type = null) {}",
        "top_statement_list(function_declaration(is_reference [is_reference:0]; T_STRING (@ 0:15) [text:method]; parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE (@ 0:22) [text:$parameter]); parameter(__ANY_CLASS_TYPE__; T_VARIABLE (@ 0:34) [text:$type]; namespace_name(T_STRING (@ 0:42) [text:null]))); inner_statement_list))"
    );}
    void test_functionParametersDefinitionAllowsReference() { TEST_PHP_PARSER(
        "<?php function method($parameter, &$variable) {}",
        "top_statement_list(function_declaration(is_reference [is_reference:0]; T_STRING (@ 0:15) [text:method]; parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE (@ 0:22) [text:$parameter]); parameter [is_reference:1](__ANY_CLASS_TYPE__; T_VARIABLE (@ 0:35) [text:$variable])); inner_statement_list))"
    );}
    void test_instantiateObjectWithoutParameters() { TEST_PHP_PARSER(
        "<?php new classname;",
        "top_statement_list(T_NEW(namespace_name(T_STRING (@ 0:10) [text:classname]); VOID))"
    );}
    void test_arrayDefinitionInsideForeach() { TEST_PHP_PARSER(
        "<?php foreach(array('x') as $x);",
        "top_statement_list(foreach(T_ARRAY(array_pair_list(array_pair(array_key; array_value(T_CONSTANT_ENCAPSED_STRING (@ 0:20) [text:x])))); T_VARIABLE (@ 0:28) [text:$x]; foreach_optional_arg; empty statement))"
    );}
    void test_negativeConstant() { TEST_PHP_PARSER(
        "<?php class classname { const constname = -1; }",
        "top_statement_list(class_declaration(class (@ 0:6); T_STRING (@ 0:12) [text:classname]; extends; implements; class_statement_list(class_constants(T_STRING (@ 0:30) [text:constname](T_NEGATIVE(T_LNUMBER (@ 0:43) [text:1]))))))"
    );}
    void test_callMethodFromString() { TEST_PHP_PARSER(
        "<?php $object->$methodName();",
        "top_statement_list(T_OBJECT_OPERATOR(T_VARIABLE (@ 0:6) [text:$object]; T_VARIABLE (@ 0:16) [text:$methodName]; method_or_not(function_call_parameter_list); variable_properties))"
    );}
    void test_callMethodFromStringWrapped() { TEST_PHP_PARSER(
        "<?php $object->{$methodName}();",
        "top_statement_list(T_OBJECT_OPERATOR(T_VARIABLE (@ 0:6) [text:$object]; T_VARIABLE (@ 0:17) [text:$methodName]; method_or_not(function_call_parameter_list); variable_properties))"
    );}
    void test_listWithEmptyVariables() { TEST_PHP_PARSER(
        "<?php list(,,$x) = $y;",
        "top_statement_list(assignment(assignment_list(VOID; VOID; T_VARIABLE (@ 0:13) [text:$x]); T_VARIABLE (@ 0:19) [text:$y]))"
    );}
    void test_variableRefByVariable() { TEST_PHP_PARSER(
        "<?php $$x;",
        "top_statement_list(simple_indirect_reference(T_VARIABLE (@ 0:7) [text:$x]))"
    );}
    void test_whitespaceInHeredocStart() { TEST_PHP_PARSER(
        "<?php \n"
        "$x = <<< XXX\n"
        "asd\n"
        "XXX;\n",
        "top_statement_list(assignment(T_VARIABLE (@ 1:0) [text:$x]; T_ENCAPSED_AND_WHITESPACE (@ 2:0) [text:asd]))"
    );}
    void test_alternativeIfElseEndifSyntax() { TEST_PHP_PARSER(
        "<?php if (1): echo $x; elseif (2): echo $z; else: echo $y; endif;",
        "top_statement_list(if(T_LNUMBER (@ 0:10) [text:1]; inner_statement_list(echo(echo_expr_list(T_VARIABLE (@ 0:19) [text:$x]))); elseif_list(elseif(T_LNUMBER (@ 0:31) [text:2]; inner_statement_list(echo(echo_expr_list(T_VARIABLE (@ 0:40) [text:$z]))))); else(inner_statement_list(echo(echo_expr_list(T_VARIABLE (@ 0:55) [text:$y]))))))"
    );}
    void test_globalsUsage() { TEST_PHP_PARSER(
        "<?php function x() {global $x, $y;}",
        "top_statement_list(function_declaration(is_reference [is_reference:0]; T_STRING (@ 0:15) [text:x]; parameter_list; inner_statement_list(global(global_var_list(T_VARIABLE (@ 0:27) [text:$x]; T_VARIABLE (@ 0:31) [text:$y])))))"
    );}
    void test_forWithEmptySpecifications() { TEST_PHP_PARSER(
        "<?php for(;;) ;",
        "top_statement_list(for(VOID; VOID; VOID; empty statement))"
    );}
    void test_hereDocAsArgument() { TEST_PHP_PARSER(
        "<?php array('x'=> <<<XX\nasd\nXX\n) ;",
        "top_statement_list(T_ARRAY(array_pair_list(array_pair(array_key(T_CONSTANT_ENCAPSED_STRING (@ 0:12) [text:x]); array_value(T_ENCAPSED_AND_WHITESPACE (@ 1:0) [text:asd])))))"
    );}
    void test_danglingElse() { TEST_PHP_PARSER(
        "<?php if(1) if (2) {echo $y;} else {echo $z;}",
        ""
    );}
    void test_danglingElseIf() { TEST_PHP_PARSER(
        "<?php if(1) if (2) {echo $y;} elseif (3) {echo $z;}",
        ""
    );}

//    void test_asd() { TEST_PHP_PARSER(
//        "<?php if (1): echo $x; else: echo $y; endif;",
//        ""
//    );}
};

}

#endif // PHPPARSERTEST_H

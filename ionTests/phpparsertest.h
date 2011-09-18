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
        PRINT(ret->toMlString()) \
    ); \
}

class PhpParserTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_openCloseNoEnd() { TEST_PHP_PARSER(
        "<?php ?><?php ?><?php ",
        "top_statement_list"
    ); }
    void test_openCloseEnd() { TEST_PHP_PARSER(
        "<?php ?><?php ?><?php ?>",
        "top_statement_list"
    ); }
    void test_inlineHtml() { TEST_PHP_PARSER(
        "<?php ?>asd1<?php ?>asd2",
        "top_statement_list(T_INLINE_HTML [text:asd1]; T_INLINE_HTML [text:asd2])"
    ); }
    void test_scriptOpenTag() { TEST_PHP_PARSER(
        "<?php ?>asd<script language=\"php\">echo $d</script> asd",
        "top_statement_list(T_INLINE_HTML [text:asd]; echo(echo_expr_list(T_VARIABLE [text:$d])); T_INLINE_HTML [text: asd])"
    ); }
    void test_scriptOpenTagWOQuotes() { TEST_PHP_PARSER(
        "<?php ?>asd<script language=php>echo $d</script> asd",
        "top_statement_list(T_INLINE_HTML [text:asd]; echo(echo_expr_list(T_VARIABLE [text:$d])); T_INLINE_HTML [text: asd])"
    ); }
    void test_scriptOpenTagWrong() { TEST_PHP_PARSER(
        "<?php ?>asd<script language=notphp>echo $d</script> asd",
        "top_statement_list(T_INLINE_HTML [text:asd<script language=notphp>echo $d</script> asd])"
    ); }
    void test_variableAssignmentFromFncCall() { TEST_PHP_PARSER(
        "<?php $a = moo();",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; function_call(namespace_name(T_STRING [text:moo]); function_call_parameter_list)))"
    ); }
    void test_variableAssignmentFromFncCallWithParams() { TEST_PHP_PARSER(
        "<?php $a = moo(1, $s);",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; function_call(namespace_name(T_STRING [text:moo]); function_call_parameter_list(T_LNUMBER [text:1]; T_VARIABLE [text:$s]))))"
    ); }
    void test_functionDefinition() {
        TEST_PHP_PARSER(
            "<?php function myfnc() {}",
            "top_statement_list(function_declaration(T_STRING [text:myfnc]; parameter_list; inner_statement_list))"
        );
    }
    void test_listDefinition() { TEST_PHP_PARSER(
        "<?php list($a, $b) = $c;",
        "top_statement_list(assignment(assignment_list(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]); T_VARIABLE [text:$c]))"
    ); }
    void test_assignDefinition() { TEST_PHP_PARSER(
        "<?php $a = $c;",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_VARIABLE [text:$c]))"
    ); }
    void test_assignRefDefinition() { TEST_PHP_PARSER(
        "<?php $a = &$c;",
        "top_statement_list(assignment [is_reference:1](T_VARIABLE [text:$a]; T_VARIABLE [text:$c]))"
    ); }
    void test_assignRefNewClassDefinition() { TEST_PHP_PARSER(
        "<?php $a = & new asd;",
        "top_statement_list(assignment [is_reference:1](T_VARIABLE [text:$a]; T_NEW(namespace_name(T_STRING [text:asd]))))"
    ); }
    void test_newClassDefinition() { TEST_PHP_PARSER(
        "<?php new asd(1, '2');",
        "top_statement_list(T_NEW(namespace_name(T_STRING [text:asd]); function_call_parameter_list(T_LNUMBER [text:1]; T_CONSTANT_ENCAPSED_STRING [text:2])))"
    ); }
    void test_cloneDefinition() { TEST_PHP_PARSER(
        "<?php clone $a;",
        "top_statement_list(clone(T_VARIABLE [text:$a]))"
    ); }
    void test_assignPlusDefinition() { TEST_PHP_PARSER(
        "<?php $a+=$b;",
        "top_statement_list(T_PLUS_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignMinusDefinition() { TEST_PHP_PARSER(
        "<?php $a-=$b;",
        "top_statement_list(T_MINUS_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignMulDefinition() { TEST_PHP_PARSER(
        "<?php $a*=$b;",
        "top_statement_list(T_MUL_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignDivDefinition() { TEST_PHP_PARSER(
        "<?php $a/=$b;",
        "top_statement_list(T_DIV_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignConcatDefinition() { TEST_PHP_PARSER(
        "<?php $a.=$b;",
        "top_statement_list(T_CONCAT_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignModDefinition() { TEST_PHP_PARSER(
        "<?php $a%=$b;",
        "top_statement_list(T_MOD_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignAndDefinition() { TEST_PHP_PARSER(
        "<?php $a&=$b;",
        "top_statement_list(T_AND_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignOrDefinition() { TEST_PHP_PARSER(
        "<?php $a|=$b;",
        "top_statement_list(T_OR_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignXorDefinition() { TEST_PHP_PARSER(
        "<?php $a^=$b;",
        "top_statement_list(T_XOR_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignShLeftDefinition() { TEST_PHP_PARSER(
        "<?php $a<<=$b;",
        "top_statement_list(T_SL_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_assignShRightDefinition() { TEST_PHP_PARSER(
        "<?php $a>>=$b;",
        "top_statement_list(T_SR_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_postIncDefinition() { TEST_PHP_PARSER(
        "<?php $a++;",
        "top_statement_list(POSTINC(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParserp_preIncDefinition() { TEST_PHP_PARSER(
        "<?php ++$b;",
        "top_statement_list(PREINC(T_VARIABLE [text:$b]))"
    ); }
    void test_postDecDefinition() { TEST_PHP_PARSER(
        "<?php $a--;",
        "top_statement_list(POSTDEC(T_VARIABLE [text:$a]))"
    ); }
    void test_preDecDefinition() { TEST_PHP_PARSER(
        "<?php --$a;",
        "top_statement_list(PREDEC(T_VARIABLE [text:$a]))"
    ); }
    void test_boolOrDefinition() { TEST_PHP_PARSER(
        "<?php $a || $b;",
        "top_statement_list(T_BOOLEAN_OR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_boolAndDefinition() { TEST_PHP_PARSER(
        "<?php $a && $b;",
        "top_statement_list(T_BOOLEAN_AND(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_logicalOrDefinition() { TEST_PHP_PARSER(
        "<?php $a or $b;",
        "top_statement_list(T_LOGICAL_OR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_logicalAndDefinition() { TEST_PHP_PARSER(
        "<?php $a and $b;",
        "top_statement_list(T_LOGICAL_AND(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_logicalXorDefinition() { TEST_PHP_PARSER(
        "<?php $a xor $b;",
        "top_statement_list(T_LOGICAL_XOR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_orDefinition() { TEST_PHP_PARSER(
        "<?php $a | $b;",
        "top_statement_list(T_BINARY_OR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_andDefinition() { TEST_PHP_PARSER(
        "<?php $a & $b;",
        "top_statement_list(T_BINARY_AND(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_xorDefinition() { TEST_PHP_PARSER(
        "<?php $a ^ $b;",
        "top_statement_list(T_BINARY_XOR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_concatDefinition() { TEST_PHP_PARSER(
        "<?php $a . $b;",
        "top_statement_list(T_CONCAT(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_plusDefinition() { TEST_PHP_PARSER(
        "<?php $a + $b;",
        "top_statement_list(T_PLUS(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_minusDefinition() { TEST_PHP_PARSER(
        "<?php $a - $b;",
        "top_statement_list(T_MINUS(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_mulDefinition() { TEST_PHP_PARSER(
        "<?php $a * $b;",
        "top_statement_list(T_MUL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_divDefinition() { TEST_PHP_PARSER(
        "<?php $a / $b;",
        "top_statement_list(T_DIV(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_modDefinition() { TEST_PHP_PARSER(
        "<?php $a % $b;",
        "top_statement_list(T_MOD(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_shLeftDefinition() { TEST_PHP_PARSER(
        "<?php $a << $b;",
        "top_statement_list(T_SHIFT_LEFT(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_shRightDefinition() { TEST_PHP_PARSER(
        "<?php $a >> $b;",
        "top_statement_list(T_SHIFT_RIGHT(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_negateDefinition() { TEST_PHP_PARSER(
        "<?php !$a;",
        "top_statement_list(T_NEGATE(T_VARIABLE [text:$a]))"
    ); }
    void test_inverseDefinition() { TEST_PHP_PARSER(
        "<?php ~$a;",
        "top_statement_list(T_INVERSE(T_VARIABLE [text:$a]))"
    ); }
    void test_isIdenticalDefinition() { TEST_PHP_PARSER(
        "<?php $a === $b;",
        "top_statement_list(T_IS_IDENTICAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_isNotIdenticalDefinition() { TEST_PHP_PARSER(
        "<?php $a !== $b;",
        "top_statement_list(T_IS_NOT_IDENTICAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_isEqualDefinition() { TEST_PHP_PARSER(
        "<?php $a == $b;",
        "top_statement_list(T_IS_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_isNotEqualDefinition() { TEST_PHP_PARSER(
        "<?php $a != $b;",
        "top_statement_list(T_IS_NOT_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_lessDefinition() { TEST_PHP_PARSER(
        "<?php $a < $b;",
        "top_statement_list(T_LESSTHAN(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_lessEqDefinition() { TEST_PHP_PARSER(
        "<?php $a <= $b;",
        "top_statement_list(T_LESSTHAN_EQ(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_moreDefinition() { TEST_PHP_PARSER(
        "<?php $a > $b;",
        "top_statement_list(T_GREATERTHAN(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_moreEqDefinition() { TEST_PHP_PARSER(
        "<?php $a >= $b;",
        "top_statement_list(T_GREATERTHAN_EQ(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void test_instanceOfDefinition() { TEST_PHP_PARSER(
        "<?php $a instanceof B;",
        "top_statement_list(T_INSTANCEOF(T_VARIABLE [text:$a]; namespace_name(T_STRING [text:B])))"
    ); }
    void test_parenthesesDefinition() { TEST_PHP_PARSER(
        "<?php ($a);",
        "top_statement_list(T_VARIABLE [text:$a])"
    ); }
    void test_ternaryDefinition() { TEST_PHP_PARSER(
        "<?php $a ? $b : $c ;",
        "top_statement_list(TERNARYOP(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]; T_VARIABLE [text:$c]))"
    ); }
    void test_ternaryPartialDefinition() { TEST_PHP_PARSER(
        "<?php $a ? : $c ;",
        "top_statement_list(TERNARYOP(T_VARIABLE [text:$a]; VOID; T_VARIABLE [text:$c]))"
    ); }
    void test_castIntDefinition() { TEST_PHP_PARSER(
        "<?php (int) $a ;",
        "top_statement_list(T_INT_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_castDoubleDefinition() { TEST_PHP_PARSER(
        "<?php (double) $a ;",
        "top_statement_list(T_DOUBLE_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_castStringDefinition() { TEST_PHP_PARSER(
        "<?php (string) $a ;",
        "top_statement_list(T_STRING_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_castArrayDefinition() { TEST_PHP_PARSER(
        "<?php (array) $a ;",
        "top_statement_list(T_ARRAY_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_castObjectDefinition() { TEST_PHP_PARSER(
        "<?php (object) $a ;",
        "top_statement_list(T_OBJECT_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_castBoolDefinition() { TEST_PHP_PARSER(
        "<?php (bool) $a ;",
        "top_statement_list(T_BOOL_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_castUnsetDefinition() { TEST_PHP_PARSER(
        "<?php (unset) $a ;",
        "top_statement_list(T_UNSET_CAST(T_VARIABLE [text:$a]))"
    ); }
    void test_exitDefinition() { TEST_PHP_PARSER(
        "<?php exit (1) ;",
        "top_statement_list(T_EXIT(T_LNUMBER [text:1]))"
    ); }
    void test_silenceDefinition() { TEST_PHP_PARSER(
        "<?php @$a ;",
        "top_statement_list(SILENCE(T_VARIABLE [text:$a]))"
    ); }
    void test_arrayDefinition() { TEST_PHP_PARSER(
        "<?php array(1=>$a, &$b, 'c'=>3+2,) ;",
        "top_statement_list(T_ARRAY(array_pair_list(array_pair(array_key(T_LNUMBER [text:1]); array_value(T_VARIABLE [text:$a])); array_pair(array_key; array_value [is_reference:1](T_VARIABLE [text:$b])); array_pair(array_key(T_CONSTANT_ENCAPSED_STRING [text:c]); array_value(T_PLUS(T_LNUMBER [text:3]; T_LNUMBER [text:2]))))))"
    ); }
    void test_arrayDefinitionRef() { TEST_PHP_PARSER(
        "<?php array(&$a, &$b) ;",
        "top_statement_list(T_ARRAY(array_pair_list(array_pair(array_key; array_value [is_reference:1](T_VARIABLE [text:$a])); array_pair(array_key; array_value [is_reference:1](T_VARIABLE [text:$b])))))"
    ); }
    void test_backticksDefinition() { TEST_PHP_PARSER(
        "<?php `$a boo moo` ;",
        "top_statement_list(BACKTICKS(encaps_list(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text: boo moo])))"
    ); }
    void test_printDefinition() { TEST_PHP_PARSER(
        "<?php print $x ;",
        "top_statement_list(T_PRINT(T_VARIABLE [text:$x]))"
    ); }
    void test_lambdaFncDefinition() { TEST_PHP_PARSER(
        "<?php function ($a) { return $c; } ;",
        "top_statement_list(LAMBDA_FUNCTION(parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE [text:$a])); T_USE; inner_statement_list(return(T_VARIABLE [text:$c]))))"
    ); }
    void test_lambdaFncUseDefinition() { TEST_PHP_PARSER(
        "<?php function ($a) use ($b) { return $c; } ;",
        "top_statement_list(LAMBDA_FUNCTION(parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE [text:$a])); T_USE(lexical_var_list(T_VARIABLE [text:$b])); inner_statement_list(return(T_VARIABLE [text:$c]))))"
    ); }
    void test_lambdaFncRefDefinition() { TEST_PHP_PARSER(
        "<?php function & ($a) use ($b, &$c) { return $c; } ;",
        "top_statement_list(LAMBDA_FUNCTION(parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE [text:$a])); T_USE(lexical_var_list(T_VARIABLE [text:$b]; T_VARIABLE [is_reference:1, text:$c])); inner_statement_list(return(T_VARIABLE [text:$c]))))"
    ); }
    void test_doubleQuotesConst() { TEST_PHP_PARSER(
        "<?php $a = \"\\rtest string\\n\" ;",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_CONSTANT_ENCAPSED_STRING [text:\\rtest string\\n]))"
    ); }
    void test_doubleQuotesWithVars() { TEST_PHP_PARSER(
        "<?php $a = \"\\ntest $moo more text\n\\n {$boo}${buka}s${aa[2]}tring\" ;",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; doubleQuotes(encaps_list(T_ENCAPSED_AND_WHITESPACE [text:\\ntest ]; T_VARIABLE [text:$moo]; T_ENCAPSED_AND_WHITESPACE [text: more text\n\\n ]; T_VARIABLE [text:$boo]; T_STRING_VARNAME [text:buka]; T_ENCAPSED_AND_WHITESPACE [text:s]; offset(T_STRING_VARNAME [text:aa]; T_LNUMBER [text:2]); T_ENCAPSED_AND_WHITESPACE [text:tring]))))"
    ); }
    void test_doubleQuotesWithNonVars() { TEST_PHP_PARSER(
        "<?php $a = \"test $12 more text\" ;",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_CONSTANT_ENCAPSED_STRING [text:test $12 more text]))"
    ); }
    void test_hereDoc() { TEST_PHP_PARSER(
        "<?php $a = <<<MYDOC\ntext\nMYDOC; ",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_ENCAPSED_AND_WHITESPACE [text:text]))"
    ); }
    /*
      heredoc
      nowdoc
        |    internal_functions { $$ = $1; }
        |    scalar    	    	{ $$ = $1; }
        | statements

*/
};

#endif // PHPPARSERTEST_H

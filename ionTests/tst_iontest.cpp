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
    pASTNode ret; \
    QVERIFY(ret = IonPhp::phpParser().parse(CODE)); \
    QCOMPARE_3( \
        ret->toString(), \
        QString(ASTSTR), \
        PRINT(ret->toMlString()) \
    ); \
}

class IonTest : public QObject
{
    Q_OBJECT

public:
    IonTest();

private Q_SLOTS:
    void testPhpParser_openCloseNoEnd() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php ?><?php ?><?php "));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list")
        );
    }
    void testPhpParser_openCloseEnd() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php ?><?php ?><?php ?>"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list")
        );
    }
    void testPhpParser_inlineHtml() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php ?>asd1<?php ?>asd2"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list(T_INLINE_HTML [text:asd1]; T_INLINE_HTML [text:asd2])")
         );
    }
    void testPhpParser_scriptOpenTag() {
        //QSKIP("boo", SkipSingle);
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php ?>asd<script language=\"php\">echo $d</script> asd"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list(T_INLINE_HTML [text:asd]; echo_expr_list(T_VARIABLE [text:$d]); T_INLINE_HTML [text: asd])")
        );
    }
    void testPhpParser_scriptOpenTagWOQuotes() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php ?>asd<script language=php>echo $d</script> asd"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list(T_INLINE_HTML [text:asd]; echo_expr_list(T_VARIABLE [text:$d]); T_INLINE_HTML [text: asd])")
        );
    }
    void testPhpParser_scriptOpenTagWrong() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php ?>asd<script language=notphp>echo $d</script> asd"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list(T_INLINE_HTML [text:asd<script language=notphp>echo $d</script> asd])")
        );
    }
    void testPhpParser_variableAssignmentFromFncCall() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php $a = moo();"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list(assignment(T_VARIABLE [text:$a]; function_call(namespace_name(T_STRING [text:moo]); function_call_parameter_list)))")
        );
    }
    void testPhpParser_variableAssignmentFromFncCallWithParams() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php $a = moo(1, $s);"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list(assignment(T_VARIABLE [text:$a]; function_call(namespace_name(T_STRING [text:moo]); function_call_parameter_list(T_LNUMBER [text:1]; T_VARIABLE [text:$s]))))")
        );
    }
    void testPhpParser_functionDefinition() {
        TEST_PHP_PARSER(
            "<?php function myfnc() {}",
            "top_statement_list(function_declaration(T_STRING [text:myfnc]; parameter_list; inner_statement_list))"
        );
    }
    void testPhpParser_listDefinition() { TEST_PHP_PARSER(
        "<?php list($a, $b) = $c;",
        ""
    ); }
    void testPhpParser_assignDefinition() { TEST_PHP_PARSER(
        "<?php $a = $c;",
        "top_statement_list(assignment(T_VARIABLE [text:$a]; T_VARIABLE [text:$c]))"
    ); }
    void testPhpParser_assignRefDefinition() { TEST_PHP_PARSER(
        "<?php $a = &$c;",
        "top_statement_list(assignment [is_reference:1](T_VARIABLE [text:$a]; T_VARIABLE [text:$c]))"
    ); }
    void testPhpParser_assignRefNewClassDefinition() { TEST_PHP_PARSER(
        "<?php $a = new & asd;",
        ""
    ); }
    void testPhpParser_newClassDefinition() { TEST_PHP_PARSER(
        "<?php new asd(1, '2');",
        ""
    ); }
    void testPhpParser_cloneDefinition() { TEST_PHP_PARSER(
        "<?php clone $a;",
        "top_statement_list(clone(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_assignPlusDefinition() { TEST_PHP_PARSER(
        "<?php $a+=$b;",
        "top_statement_list(T_PLUS_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_assignMinusDefinition() { TEST_PHP_PARSER(
        "<?php $a-=$b;",
        "top_statement_list(T_MINUS_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_assignMulDefinition() { TEST_PHP_PARSER(
        "<?php $a*=$b;",
        "top_statement_list(T_MUL_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_assignDivDefinition() { TEST_PHP_PARSER(
        "<?php $a/=$b;",
        "top_statement_list(T_DIV_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_assignConcatDefinition() { TEST_PHP_PARSER(
        "<?php $a.=$b;",
        "top_statement_list(T_CONCAT_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_assignModDefinition() { TEST_PHP_PARSER(
        "<?php $a%=$b;",
        "top_statement_list(T_MOD_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_assignAndDefinition() { TEST_PHP_PARSER(
        "<?php $a&=$b;",
        "top_statement_list(T_AND_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_assignOrDefinition() { TEST_PHP_PARSER(
        "<?php $a|=$b;",
        "top_statement_list(T_OR_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_assignXorDefinition() { TEST_PHP_PARSER(
        "<?php $a^=$b;",
        "top_statement_list(T_XOR_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_assignShLeftDefinition() { TEST_PHP_PARSER(
        "<?php $a<<=$b;",
        "top_statement_list(T_SL_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_assignShRightDefinition() { TEST_PHP_PARSER(
        "<?php $a>>=$b;",
        "top_statement_list(T_SR_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_postIncDefinition() { TEST_PHP_PARSER(
        "<?php $a++;",
        "top_statement_list(POSTINC(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParserp_preIncDefinition() { TEST_PHP_PARSER(
        "<?php ++$b;",
        "top_statement_list(PREINC(T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_postDecDefinition() { TEST_PHP_PARSER(
        "<?php $a--;",
        "top_statement_list(POSTDEC(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_preDecDefinition() { TEST_PHP_PARSER(
        "<?php --$a;",
        "top_statement_list(PREDEC(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_boolOrDefinition() { TEST_PHP_PARSER(
        "<?php $a || $b;",
        "top_statement_list(T_BOOLEAN_OR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_boolAndDefinition() { TEST_PHP_PARSER(
        "<?php $a && $b;",
        "top_statement_list(T_BOOLEAN_AND(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_logicalOrDefinition() { TEST_PHP_PARSER(
        "<?php $a or $b;",
        "top_statement_list(T_LOGICAL_OR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_logicalAndDefinition() { TEST_PHP_PARSER(
        "<?php $a and $b;",
        "top_statement_list(T_LOGICAL_AND(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_logicalXorDefinition() { TEST_PHP_PARSER(
        "<?php $a xor $b;",
        "top_statement_list(T_LOGICAL_XOR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_orDefinition() { TEST_PHP_PARSER(
        "<?php $a | $b;",
        "top_statement_list(T_BINARY_OR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_andDefinition() { TEST_PHP_PARSER(
        "<?php $a & $b;",
        "top_statement_list(T_BINARY_AND(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_xorDefinition() { TEST_PHP_PARSER(
        "<?php $a ^ $b;",
        "top_statement_list(T_BINARY_XOR(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_concatDefinition() { TEST_PHP_PARSER(
        "<?php $a . $b;",
        "top_statement_list(T_CONCAT(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_plusDefinition() { TEST_PHP_PARSER(
        "<?php $a + $b;",
        "top_statement_list(T_PLUS(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_minusDefinition() { TEST_PHP_PARSER(
        "<?php $a - $b;",
        "top_statement_list(T_MINUS(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_mulDefinition() { TEST_PHP_PARSER(
        "<?php $a * $b;",
        "top_statement_list(T_MUL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_divDefinition() { TEST_PHP_PARSER(
        "<?php $a / $b;",
        "top_statement_list(T_DIV(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_modDefinition() { TEST_PHP_PARSER(
        "<?php $a % $b;",
        "top_statement_list(T_MOD(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_shLeftDefinition() { TEST_PHP_PARSER(
        "<?php $a << $b;",
        "top_statement_list(T_SHIFT_LEFT(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_shRightDefinition() { TEST_PHP_PARSER(
        "<?php $a >> $b;",
        "top_statement_list(T_SHIFT_RIGHT(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_negateDefinition() { TEST_PHP_PARSER(
        "<?php !$a;",
        "top_statement_list(T_NEGATE(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_inverseDefinition() { TEST_PHP_PARSER(
        "<?php ~$a;",
        "top_statement_list(T_INVERSE(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_isIdenticalDefinition() { TEST_PHP_PARSER(
        "<?php $a === $b;",
        "top_statement_list(T_IS_IDENTICAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_isNotIdenticalDefinition() { TEST_PHP_PARSER(
        "<?php $a !== $b;",
        "top_statement_list(T_IS_NOT_IDENTICAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_isEqualDefinition() { TEST_PHP_PARSER(
        "<?php $a == $b;",
        "top_statement_list(T_IS_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_isNotEqualDefinition() { TEST_PHP_PARSER(
        "<?php $a != $b;",
        "top_statement_list(T_IS_NOT_EQUAL(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_lessDefinition() { TEST_PHP_PARSER(
        "<?php $a < $b;",
        "top_statement_list(T_LESSTHAN(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_lessEqDefinition() { TEST_PHP_PARSER(
        "<?php $a <= $b;",
        "top_statement_list(T_LESSTHAN_EQ(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_moreDefinition() { TEST_PHP_PARSER(
        "<?php $a > $b;",
        "top_statement_list(T_GREATERTHAN(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_moreEqDefinition() { TEST_PHP_PARSER(
        "<?php $a >= $b;",
        "top_statement_list(T_GREATERTHAN_EQ(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]))"
    ); }
    void testPhpParser_instanceOfDefinition() { TEST_PHP_PARSER(
        "<?php $a instanceof B;",
        "top_statement_list(T_INSTANCEOF(T_VARIABLE [text:$a]; namespace_name(T_STRING [text:B])))"
    ); }
    void testPhpParser_parenthesesDefinition() { TEST_PHP_PARSER(
        "<?php ($a);",
        "top_statement_list(T_VARIABLE [text:$a])"
    ); }
    void testPhpParser_ternaryDefinition() { TEST_PHP_PARSER(
        "<?php $a ? $b : $c ;",
        "top_statement_list(TERNARYOP(T_VARIABLE [text:$a]; T_VARIABLE [text:$b]; T_VARIABLE [text:$c]))"
    ); }
    void testPhpParser_ternaryPartialDefinition() { TEST_PHP_PARSER(
        "<?php $a ? : $c ;",
        "top_statement_list(TERNARYOP(T_VARIABLE [text:$a]; VOID; T_VARIABLE [text:$c]))"
    ); }
    void testPhpParser_castIntDefinition() { TEST_PHP_PARSER(
        "<?php (int) $a ;",
        "top_statement_list(T_INT_CAST(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_castDoubleDefinition() { TEST_PHP_PARSER(
        "<?php (double) $a ;",
        "top_statement_list(T_DOUBLE_CAST(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_castStringDefinition() { TEST_PHP_PARSER(
        "<?php (string) $a ;",
        "top_statement_list(T_STRING_CAST(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_castArrayDefinition() { TEST_PHP_PARSER(
        "<?php (array) $a ;",
        "top_statement_list(T_ARRAY_CAST(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_castObjectDefinition() { TEST_PHP_PARSER(
        "<?php (object) $a ;",
        "top_statement_list(T_OBJECT_CAST(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_castBoolDefinition() { TEST_PHP_PARSER(
        "<?php (bool) $a ;",
        "top_statement_list(T_BOOL_CAST(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_castUnsetDefinition() { TEST_PHP_PARSER(
        "<?php (unset) $a ;",
        "top_statement_list(T_UNSET_CAST(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_exitDefinition() { TEST_PHP_PARSER(
        "<?php exit (1) ;",
        "top_statement_list(T_EXIT(T_LNUMBER [text:1]))"
    ); }
    void testPhpParser_silenceDefinition() { TEST_PHP_PARSER(
        "<?php @$a ;",
        "top_statement_list(SILENCE(T_VARIABLE [text:$a]))"
    ); }
    void testPhpParser_arrayDefinition() { TEST_PHP_PARSER(
        "<?php array(1=>$a, $b, 'c'=>\"d\",) ;",
        ""
    ); }
    void testPhpParser_backticksDefinition() { TEST_PHP_PARSER(
        "<?php `$a boo moo` ;",
        ""
    ); }
    void testPhpParser_printDefinition() { TEST_PHP_PARSER(
        "<?php print $x ;",
        "top_statement_list(T_PRINT(T_VARIABLE [text:$x]))"
    ); }
    void testPhpParser_lambdaFncDefinition() { TEST_PHP_PARSER(
        "<?php function ($a) { return $c; } ;",
        "top_statement_list(LAMBDA_FUNCTION(parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE [text:$a])); T_USE; inner_statement_list(return(T_VARIABLE [text:$c]))))"
    ); }
    void testPhpParser_lambdaFncUseDefinition() { TEST_PHP_PARSER(
        "<?php function ($a) use ($b) { return $c; } ;",
        "top_statement_list(LAMBDA_FUNCTION(parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE [text:$a])); T_USE(lexical_var_list(T_VARIABLE [text:$b])); inner_statement_list(return(T_VARIABLE [text:$c]))))"
    ); }
    void testPhpParser_lambdaFncRefDefinition() { TEST_PHP_PARSER(
        "<?php function & ($a) use ($b, &$c) { return $c; } ;",
        "top_statement_list(LAMBDA_FUNCTION(parameter_list(parameter(__ANY_CLASS_TYPE__; T_VARIABLE [text:$a])); T_USE(lexical_var_list(T_VARIABLE [text:$b]; T_VARIABLE [is_reference:1, text:$c])); inner_statement_list(return(T_VARIABLE [text:$c]))))"
    ); }

    /*
        |    internal_functions { $$ = $1; }
        |    scalar    	    	{ $$ = $1; }

*/
};

IonTest::IonTest()
{
}

QTEST_APPLESS_MAIN(IonTest);

#include "tst_iontest.moc"

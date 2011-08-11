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
    void testPhpParser_arrayDefinition() {
        TEST_PHP_PARSER(
            "<?php array('a', 2, 'sd'=>2);",
            ""
        );
    }
};

IonTest::IonTest()
{
}

QTEST_APPLESS_MAIN(IonTest);

#include "tst_iontest.moc"

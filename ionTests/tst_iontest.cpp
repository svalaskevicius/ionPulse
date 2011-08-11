#include <QtCore/QString>
#include <QtTest/QtTest>
#include <iostream>
#include <ionParticles/ionPhp/phpparser.h>

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
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php function myfnc() {}"));
        QCOMPARE(
            ret->toString(),
            QString("top_statement_list(function_declaration(T_STRING [text:myfnc]; parameter_list; inner_statement_list))")
        );
    }
    void testPhpParser_arrayDefinition() {
        pASTNode ret;
        QVERIFY(ret = IonPhp::phpParser().parse("<?php array('a', 2, 'sd'=>2);"));
        QCOMPARE(
            ret->toString(),
            QString("")
        );
    }
};

IonTest::IonTest()
{
}

QTEST_APPLESS_MAIN(IonTest);

#include "tst_iontest.moc"

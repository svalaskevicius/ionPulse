#include <QtCore/QString>
#include <QtTest/QtTest>

#include <ionParticles/ionPhp/phpparser.h>

class IonTest : public QObject
{
    Q_OBJECT

public:
    IonTest();

private Q_SLOTS:
    void testPhpParser_1() {QVERIFY(IonPhp::phpParser().parse("<?php ?><?php ?><?php "));}
    void testPhpParser_2() {QVERIFY(IonPhp::phpParser().parse("<?php ?><?php ?><?php ?>"));}
    void testPhpParser_3() {QVERIFY(IonPhp::phpParser().parse("<?php ?>asd1<?php ?>asd2"));}
    void testPhpParser_4() {QVERIFY(IonPhp::phpParser().parse("<?php ?>asd<script language=\"php\">echo $d</script> asd"));}
    void testPhpParser_5() {QVERIFY(IonPhp::phpParser().parse("<?php ?>asd<script language=php>echo $d</script> asd"));}
    void testPhpParser_6() {QVERIFY(IonPhp::phpParser().parse("<?php ?>asd<script language=notphp>echo $d</script> asd"));}
    void testPhpParser_7() {QVERIFY(IonPhp::phpParser().parse("<?php $a = moo();"));}
};

IonTest::IonTest()
{
}

QTEST_APPLESS_MAIN(IonTest);

#include "tst_iontest.moc"

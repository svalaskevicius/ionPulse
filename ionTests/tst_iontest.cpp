#include <QtCore/QString>
#include <QtTest/QtTest>

#include <ionParticles/ionPhp/phpparser.h>


class IonTest : public QObject
{
    Q_OBJECT

public:
    IonTest();

private Q_SLOTS:
    void testPhpParser();
};

IonTest::IonTest()
{
}

void IonTest::testPhpParser()
{
    IonPhp::phpParser p;
    QVERIFY2(p.parse("<?php $a = moo();", "test"), "Failure");
    QVERIFY2(p.parse("<?php?><?php?><?php", "test"), "Failure");
    QVERIFY2(p.parse("<?php?><?php?><?php?>", "test"), "Failure");
    QVERIFY2(p.parse("<?php?>asd<?php?>asd", "test"), "Failure");
}

QTEST_APPLESS_MAIN(IonTest);

#include "tst_iontest.moc"

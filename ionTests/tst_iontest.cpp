#include <QtCore/QString>
#include <QtTest/QtTest>

class IonTest : public QObject
{
    Q_OBJECT

public:
    IonTest();

private Q_SLOTS:
    void testCase1();
};

IonTest::IonTest()
{
}

void IonTest::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(IonTest);

#include "tst_iontest.moc"

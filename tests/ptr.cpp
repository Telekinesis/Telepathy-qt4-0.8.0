#include <QtTest/QtTest>

#include <TelepathyQt4/SharedPtr>

using namespace Tp;

class TestSharedPtr : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testSharedPtrDict();
    void testSharedPtrBoolConversion();
};

class Data;
typedef SharedPtr<Data> DataPtr;

class Data : public QObject,
             public RefCounted
{
    Q_OBJECT
    Q_DISABLE_COPY(Data);

public:
    static DataPtr create() { return DataPtr(new Data()); }
    static DataPtr createNull() { return DataPtr(0); }

private:
    Data() {}
};

void TestSharedPtr::testSharedPtrDict()
{
    QHash<DataPtr, int> dict;
    DataPtr nullPtr = Data::createNull();
    dict[nullPtr] = 1;
    QCOMPARE(dict.size(), 1);
    QCOMPARE(dict[nullPtr], 1);

    DataPtr validPtr1 = Data::create();
    QCOMPARE(qHash(validPtr1.data()), qHash(validPtr1));
    dict[validPtr1] = 2;
    QCOMPARE(dict.size(), 2);
    QCOMPARE(dict[nullPtr], 1);
    QCOMPARE(dict[validPtr1], 2);

    DataPtr validPtr2 = validPtr1;
    QCOMPARE(validPtr1.data(), validPtr2.data());
    QCOMPARE(qHash(validPtr1), qHash(validPtr2));
    dict[validPtr2] = 3;
    QCOMPARE(dict.size(), 2);
    QCOMPARE(dict[nullPtr], 1);
    QCOMPARE(dict[validPtr1], 3);
    QCOMPARE(dict[validPtr2], 3);

    DataPtr validPtrAlternative = Data::create();
    QVERIFY(validPtr1.data() != validPtrAlternative.data());
    QVERIFY(validPtr1 != validPtrAlternative);
    QVERIFY(qHash(validPtr1) != qHash(validPtrAlternative));
    dict[validPtrAlternative] = 4;
    QCOMPARE(dict.size(), 3);
    QCOMPARE(dict[nullPtr], 1);
    QCOMPARE(dict[validPtr1], 3);
    QCOMPARE(dict[validPtr2], 3);
    QCOMPARE(dict[validPtrAlternative], 4);
}

void TestSharedPtr::testSharedPtrBoolConversion()
{
    DataPtr nullPtr1;
    DataPtr nullPtr2 = Data::createNull();
    DataPtr validPtr1 = Data::create();
    DataPtr validPtr2 = validPtr1;
    DataPtr validPtrAlternative = Data::create();

    // Boolean conditions
    QVERIFY(!validPtr1.isNull());
    QVERIFY(nullPtr1.isNull());
    QVERIFY(validPtr1 ? true : false);
    QVERIFY(!validPtr1 ? false : true);
    QVERIFY(nullPtr1 ? false : true);
    QVERIFY(!nullPtr1 ? true : false);
    QVERIFY(validPtr1);
    QVERIFY(!!validPtr1);
    QVERIFY(!nullPtr1);

    // Supported operators
    QVERIFY(nullPtr1 == nullPtr1);
    QVERIFY(nullPtr1 == nullPtr2);

    QVERIFY(validPtr1 == validPtr1);
    QVERIFY(validPtr1 == validPtr2);
    QVERIFY(validPtr1 != validPtrAlternative);
    QCOMPARE(validPtr1 == validPtrAlternative, false);

    QVERIFY(validPtr1 != nullPtr1);
    QCOMPARE(validPtr1 == nullPtr1, false);

    // Supported conversions, constructors and copy operators
    bool trueBool1 = validPtr1;
    QVERIFY(trueBool1);
    bool trueBool2(validPtr2);
    QVERIFY(trueBool2);
    trueBool1 = validPtrAlternative;
    QVERIFY(trueBool1);

    bool falseBool1 = nullPtr1;
    QVERIFY(!falseBool1);
    bool falseBool2(nullPtr2);
    QVERIFY(!falseBool2);
    falseBool1 = nullPtr1;
    QVERIFY(!falseBool1);

#if 0
    // Unsupported operators, this should not compile
    bool condition;
    condition = validPtr1 > nullPtr1;
    condition = validPtr1 + nullPtr1;

    // Unsupported conversions, this should not compile
    int validInt1 = validPtr1;
    int validInt2(validPtr1);
    validInt1 = validPtr1;

    int nullInt1 = nullPtr1;
    int nullInt2(nullPtr1);
    nullInt1 = nullPtr1;

    float validFloat1 = validPtr1;
    float validFloat2(validPtr1);
    validFloat1 = validPtr1;

    float nullFloat1 = nullPtr1;
    float nullFloat2(nullPtr1);
    nullFloat1 = nullPtr1;

    Q_UNUSED(validInt1);
    Q_UNUSED(validInt2);
    Q_UNUSED(nullInt1);
    Q_UNUSED(nullInt2);
    Q_UNUSED(validFloat1);
    Q_UNUSED(validFloat2);
    Q_UNUSED(nullFloat1);
    Q_UNUSED(nullFloat2);
#endif
}

QTEST_MAIN(TestSharedPtr)

#include "_gen/ptr.cpp.moc.hpp"

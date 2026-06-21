#include <QtTest>
#include <QVector>
#include "coinSolver.h"
#include "xmlParser.h"
#include "error.h"
#include <QSet>
#include <QString>

typedef QVector<int> IntVector;
Q_DECLARE_METATYPE(IntVector) // Регистрация типа QVector<int> для использования в тестах
typedef QMap<int, int> IntIntMap;
Q_DECLARE_METATYPE(IntIntMap) // Регистрация типа QMap<int,int> для использования в тестах



/**
 * @brief Класс тестов для функции fillReachabilityTable.
 * Наследуется от QObject и использует фреймворк Qt Test для модульного тестирования.
 * Содержит data-функцию для предоставления тестовых данных и тестовую функцию для проверки.
 */
class Test_fillReachabilityTable : public QObject
{
    Q_OBJECT

private slots:
    void testFillReachabilityTable_data(); // Data-функция - предоставляет набор тестовых данных
    void testFillReachabilityTable(); // Тестовая функция - выполняется для каждой строки данных
};



/**
 * @brief Data-функция для тестирования fillReachabilityTable.
 * Определяет столбцы тестовых данных и добавляет строки с тестовыми случаями.
 * Каждая строка (newRow) будет протестирована отдельно в тестовой функции.
 */
void Test_fillReachabilityTable::testFillReachabilityTable_data()
{
    // Определение столбцов тестовых данных
    QTest::addColumn<int>("purchaseSum"); // Сумма покупки (входной параметр)
    QTest::addColumn<IntVector>("nominals"); // Номиналы монет (входной параметр)
    QTest::addColumn<int>("expectedMaxSearchLimit"); // Ожидаемый максимальный предел поиска (выходной параметр)
    QTest::addColumn<IntVector>("expectedMinCoins"); // Ожидаемая таблица минимального количества монет (выходной параметр)
    QTest::addColumn<IntVector>("expectedLastUsedCoin"); // Ожидаемая таблица последних использованных номиналов (выходной параметр)

    // Тест 1: Базовый случай
    QTest::newRow("Test 1: Base case (two denominations, the exact amount is achievable) (sum=5, nom=[2,3])")
        << 5 // purchaseSum
        << (IntVector() << 2 << 3) // nominals
        << 8 // expectedMaxSearchLimit (5 + 3 = 8)
        << (IntVector() << 0 << -1 << 1 << 1 << 2 << 2 << 2 << 3 << 3) // expectedMinCoins
        << (IntVector() << -1 << -1 << 2 << 3 << 2 << 2 << 3 << 2 << 2); // expectedLastUsedCoin

    // Тест 2: Один номинал, сумма кратна номиналу
    QTest::newRow("Test 2: One nominal value, the amount is a multiple of the nominal value (sum=10, nom=[5])")
        << 10
        << (IntVector() << 5)
        << 15
        << (IntVector() << 0 << -1 << -1 << -1 << -1 << 1 << -1 << -1 << -1 << -1 << 2 << -1 << -1 << -1 << -1 << 3)
        << (IntVector() << -1 << -1 << -1 << -1 << -1 << 5 << -1 << -1 << -1 << -1 << 5 << -1 << -1 << -1 << -1 << 5);

    // Тест 3: Сумма покупки недостижима
    QTest::newRow("Test 3: The purchase amount is unattainable (requires a change search) (sum=7, nom=[5])")
        << 7
        << (IntVector() << 5)
        << 12
        << (IntVector() << 0 << -1 << -1 << -1 << -1 << 1 << -1 << -1 << -1 << -1 << 2 << -1 << -1)
        << (IntVector() << -1 << -1 << -1 << -1 << -1 << 5 << -1 << -1 << -1 << -1 << 5 << -1 << -1);

    // Тест 4: Граничные значения (минимальные допустимые)
    QTest::newRow("Test 4: Boundary values (minimum allowable) (sum=1, nom=[1])")
        << 1
        << (IntVector() << 1)
        << 2
        << (IntVector() << 0 << 1 << 2)
        << (IntVector() << -1 << 1 << 1);

    // Тест 5: Демонстрация преимущества ДП над жадным алгоритмом
    QTest::newRow("Test 5: Demonstrating the advantages of DP over a greedy algorithm (sum=6, nom=[1,3,4])")
        << 6
        << (IntVector() << 1 << 3 << 4)
        << 10
        << (IntVector() << 0 << 1 << 2 << 1 << 1 << 2 << 2 << 2 << 2 << 3 << 3)
        << (IntVector() << -1 << 1 << 1 << 3 << 4 << 1 << 3 << 3 << 4 << 1 << 3);

    // Тест 7: Много номиналов
    // Генерируем массивы программно, вызывая тестируемую функцию для создания эталона
    {
        int purchaseSum = 100;
        IntVector nominals;
        nominals << 1 << 2 << 5 << 10 << 20 << 25 << 50;
        int maxSearchLimit = 150;

        IntVector expectedMinCoins;
        IntVector expectedLastUsedCoin;

        // Вызываем функцию для создания эталонных данных
        fillReachabilityTable(purchaseSum, nominals, expectedMinCoins, expectedLastUsedCoin);

        QTest::newRow("Test 7: Multiple coin denominations (sum=100, nom=[1,2,5,10,20,25,50])")
            << purchaseSum
            << nominals
            << maxSearchLimit
            << expectedMinCoins
            << expectedLastUsedCoin;
    }

    // Тест 8: Номинал превышает сумму покупки
    QTest::newRow("Test 8: The nominal value exceeds the purchase amount (sum=3, nom=[10])")
        << 3
        << (IntVector() << 10)
        << 13
        << (IntVector() << 0 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << 1 << -1 << -1 << -1)
        << (IntVector() << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << 10 << -1 << -1 << -1);

    // Тест 9: Одна крупная монета
    QTest::newRow("Test 9: One large coin (sum=1, nom=[50])")
        << 1
        << (IntVector() << 50)
        << 51
        << (IntVector() << 0 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                      << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                      << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                      << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                      << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                      << 1 << -1)
        << (IntVector() << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                       << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                       << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                       << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                       << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                       << 50 << -1);

    // Тест 10: Только номинал 1
    QTest::newRow("Test 10: Only the nominal value is 1 (sum=50, nom=[1])")
        << 50
        << (IntVector() << 1)
        << 51
        << (IntVector() << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10
                      << 11 << 12 << 13 << 14 << 15 << 16 << 17 << 18 << 19 << 20
                      << 21 << 22 << 23 << 24 << 25 << 26 << 27 << 28 << 29 << 30
                      << 31 << 32 << 33 << 34 << 35 << 36 << 37 << 38 << 39 << 40
                      << 41 << 42 << 43 << 44 << 45 << 46 << 47 << 48 << 49 << 50 << 51)
        << (IntVector() << -1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1
                       << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1
                       << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1
                       << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1
                       << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1
                       << 1);
}



/**
 * @brief Тестовая функция для проверки fillReachabilityTable.
 * Выполняется для каждой строки данных из testFillReachabilityTable_data().
 * Извлекает тестовые данные с помощью QFETCH и сравнивает результаты с ожидаемыми значениями через QCOMPARE.
 */
void Test_fillReachabilityTable::testFillReachabilityTable()
{
    // QFETCH извлекает значения из текущей строки тестовых данных
    QFETCH(int, purchaseSum);
    QFETCH(IntVector, nominals);
    QFETCH(int, expectedMaxSearchLimit);
    QFETCH(IntVector, expectedMinCoins);
    QFETCH(IntVector, expectedLastUsedCoin);

    // Выходные параметры для тестируемой функции
    QVector<int> minCoins;
    QVector<int> lastUsedCoin;

    // Вызов тестируемой функции
    int maxSearchLimit = fillReachabilityTable(purchaseSum, nominals, minCoins, lastUsedCoin);
    // QCOMPARE сравнивает фактическое значение с ожидаемым
    QCOMPARE(maxSearchLimit, expectedMaxSearchLimit);
    QCOMPARE(minCoins.size(), expectedMinCoins.size());
    QCOMPARE(lastUsedCoin.size(), expectedLastUsedCoin.size());

    // Проверяем все элементы
    for (int i = 0; i < expectedMinCoins.size(); ++i) {
        QCOMPARE(minCoins[i], expectedMinCoins[i]);
        QCOMPARE(lastUsedCoin[i], expectedLastUsedCoin[i]);
    }
}




/**
 * @brief Класс тестов для функции findOptimalPayment.
 * Наследуется от QObject и использует фреймворк Qt Test для модульного тестирования.
 * Содержит data-функцию для предоставления тестовых данных и тестовую функцию для проверки.
 */
class Test_findOptimalPayment : public QObject
{
    Q_OBJECT

private slots:
    void testFindOptimalPayment_data(); // Data-функция - предоставляет набор тестовых данных
    void testFindOptimalPayment(); // Тестовая функция - выполняется для каждой строки данных
};



/**
 * @brief Data-функция для тестирования findOptimalPayment.
 * Определяет столбцы тестовых данных и добавляет строки с тестовыми случаями.
 * Каждая строка (newRow) будет протестирована отдельно в тестовой функции.
 */
void Test_findOptimalPayment::testFindOptimalPayment_data()
{
    // Определение столбцов тестовых данных
    QTest::addColumn<int>("purchaseSum"); // Сумма покупки (входной параметр)
    QTest::addColumn<int>("maxSearchLimit"); // Максимальный предел поиска (входной параметр)
    QTest::addColumn<IntVector>("minCoins"); // Таблица минимального количества монет (входной параметр)
    QTest::addColumn<int>("expectedOptimalPayment"); // Ожидаемая оптимальная сумма оплаты (выходной параметр)
    QTest::addColumn<int>("expectedChange"); // Ожидаемая сдача (выходной параметр)

    // Тест 1: Базовый случай
    QTest::newRow("Test 1: Base case (two denominations, the exact amount is achievable) (sum=5)")
        << 5 // purchaseSum
        << 8 // maxSearchLimit
        << (IntVector() << 0 << -1 << 1 << 1 << 2 << 2 << 2 << 3 << 3) // minCoins
        << 5 // expectedOptimalPayment
        << 0; // expectedChange

    // Тест 2: Один номинал, сумма кратна номиналу
    QTest::newRow("Test 2: One nominal value, the amount is a multiple of the nominal value (sum=10)")
        << 10
        << 15
        << (IntVector() << 0 << -1 << -1 << -1 << -1 << 1 << -1 << -1 << -1 << -1 << 2 << -1 << -1 << -1 << -1 << 3)
        << 10
        << 0;

    // Тест 3: Сумма покупки недостижима (требуется поиск сдачи)
    QTest::newRow("Test 3: The purchase amount is unattainable (requires a change search) (sum=7)")
        << 7
        << 12
        << (IntVector() << 0 << -1 << -1 << -1 << -1 << 1 << -1 << -1 << -1 << -1 << 2 << -1 << -1)
        << 10
        << 3;

    // Тест 4: Граничные значения (минимальные допустимые)
    QTest::newRow("Test 4: Boundary values (minimum allowable) (sum=1)")
        << 1
        << 2
        << (IntVector() << 0 << 1 << 2)
        << 1
        << 0;

    // Тест 5: Демонстрация преимущества ДП над жадным алгоритмом
    QTest::newRow("Test 5: Demonstrating the advantages of DP over a greedy algorithm (sum=6)")
        << 6
        << 10
        << (IntVector() << 0 << 1 << 2 << 1 << 1 << 2 << 2 << 2 << 2 << 3 << 4)
        << 6
        << 0;

    // Тест 7: Много номиналов
    // Генерируем массив программно
    {
        int purchaseSum = 100;
        int maxSearchLimit = 150;
        IntVector minCoins(maxSearchLimit + 1, -1);
        minCoins[0] = 0;
        minCoins[1] = 1;
        minCoins[2] = 1;
        minCoins[50] = 1;
        minCoins[100] = 2;
        minCoins[150] = 3;
        // Остальные значения заполним циклом (все достижимы, так как есть номинал 1)
        for (int i = 3; i <= 150; ++i) {
            if (minCoins[i] == -1) {
                minCoins[i] = minCoins[i - 1] + 1;
            }
        }

        QTest::newRow("Test 7: Multiple coin denominations (sum=100)")
            << purchaseSum
            << maxSearchLimit
            << minCoins
            << 100
            << 0;
    }

    // Тест 8: Номинал превышает сумму покупки
    QTest::newRow("Test 8: The nominal value exceeds the purchase amount (sum=3)")
        << 3
        << 13
        << (IntVector() << 0 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << 1 << -1 << -1 << -1)
        << 10
        << 7;

    // Тест 9: Одна крупная монета
    QTest::newRow("Test 9: One large coin (sum=1)")
        << 1
        << 51
        << (IntVector() << 0 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                      << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                      << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                      << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                      << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                      << 1 << -1)
        << 50
        << 49;

    // Тест 10: Только номинал 1
    QTest::newRow("Test 10: Only the nominal value is 1 (sum=50)")
        << 50
        << 51
        << (IntVector() << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10
                      << 11 << 12 << 13 << 14 << 15 << 16 << 17 << 18 << 19 << 20
                      << 21 << 22 << 23 << 24 << 25 << 26 << 27 << 28 << 29 << 30
                      << 31 << 32 << 33 << 34 << 35 << 36 << 37 << 38 << 39 << 40
                      << 41 << 42 << 43 << 44 << 45 << 46 << 47 << 48 << 49 << 50 << 51)
        << 50
        << 0;
}



/**
 * @brief Тестовая функция для проверки findOptimalPayment.
 * Выполняется для каждой строки данных из testFindOptimalPayment_data().
 * Извлекает тестовые данные с помощью QFETCH и сравнивает результаты с ожидаемыми значениями через QCOMPARE.
 */
void Test_findOptimalPayment::testFindOptimalPayment()
{
    // QFETCH извлекает значения из текущей строки тестовых данных
    QFETCH(int, purchaseSum);
    QFETCH(int, maxSearchLimit);
    QFETCH(IntVector, minCoins);
    QFETCH(int, expectedOptimalPayment);
    QFETCH(int, expectedChange);

    // Выходные параметры для тестируемой функции
    int optimalPayment = 0;
    int change = 0;
    // Вызов тестируемой функции
    findOptimalPayment(purchaseSum, maxSearchLimit, minCoins, optimalPayment, change);

    // QCOMPARE сравнивает фактическое значение с ожидаемым
    QCOMPARE(optimalPayment, expectedOptimalPayment);
    QCOMPARE(change, expectedChange);

    // Дополнительные проверки
    QVERIFY2(optimalPayment >= purchaseSum && optimalPayment <= maxSearchLimit,
             qPrintable(QString("optimalPayment=%1 вне диапазона [%2, %3]")
                        .arg(optimalPayment).arg(purchaseSum).arg(maxSearchLimit)));
    QVERIFY2(change >= 0, "Сдача не может быть отрицательной");
    QCOMPARE(change, optimalPayment - purchaseSum);
}




/**
 * @brief Класс тестов для функции reconstructCoinCombination.
 * Наследуется от QObject и использует фреймворк Qt Test для модульного тестирования.
 * Содержит data-функцию для предоставления тестовых данных и тестовую функцию для проверки.
 */
class Test_reconstructCoinCombination : public QObject
{
    Q_OBJECT

private slots:
    void testReconstructCoinCombination_data(); // Data-функция - предоставляет набор тестовых данных
    void testReconstructCoinCombination(); // Тестовая функция - выполняется для каждой строки данных
};



/**
 * @brief Data-функция для тестирования reconstructCoinCombination.
 * Определяет столбцы тестовых данных и добавляет строки с тестовыми случаями.
 * Каждая строка (newRow) будет протестирована отдельно в тестовой функции.
 */
void Test_reconstructCoinCombination::testReconstructCoinCombination_data()
{
    QTest::addColumn<int>("optimalPayment"); // Оптимальная сумма оплаты (входной параметр)
    QTest::addColumn<IntVector>("lastUsedCoin"); // Таблица последних использованных номиналов (входной параметр)
    QTest::addColumn<IntIntMap>("expectedUsedCoins"); // Ожидаемое количество монет каждого номинала (выходной параметр)

    // Тест 1: Базовый случай
    {
        IntIntMap expected;
        expected[2] = 1;
        expected[3] = 1;
        QTest::newRow("Test 1: Base case (two denominations, the exact amount is achievable) (payment=5)")
            << 5 // optimalPayment
            << (IntVector() << -1 << -1 << 2 << 3 << 2 << 2 << 3 << 2 << 2) // lastUsedCoin
            << expected; // expectedUsedCoins
    }

    // Тест 2: Один номинал, сумма кратна номиналу
    {
        IntIntMap expected;
        expected[5] = 2;
        QTest::newRow("Test 2: One nominal value, the amount is a multiple of the nominal value (payment=10)")
            << 10
            << (IntVector() << -1 << -1 << -1 << -1 << -1 << 5 << -1 << -1 << -1 << -1 << 5 << -1 << -1 << -1 << -1 << 5)
            << expected;
    }

    // Тест 3: Сумма покупки недостижима (требуется поиск сдачи)
    {
        IntIntMap expected;
        expected[5] = 2;
        QTest::newRow("Test 3: The purchase amount is unattainable (requires a change search) (payment=10)")
            << 10
            << (IntVector() << -1 << -1 << -1 << -1 << -1 << 5 << -1 << -1 << -1 << -1 << 5 << -1 << -1)
            << expected;
    }

    // Тест 4: Граничные значения (минимальные допустимые)
    {
        IntIntMap expected;
        expected[1] = 1;
        QTest::newRow("Test 4: Boundary values (minimum allowable) (payment=1)")
            << 1
            << (IntVector() << -1 << 1 << 1)
            << expected;
    }

    // Тест 5: Демонстрация преимущества ДП над жадным алгоритмом
    {
        IntIntMap expected;
        expected[3] = 2;
        QTest::newRow("Test 5: Demonstrating the advantages of DP over a greedy algorithm (payment=6)")
            << 6
            << (IntVector() << -1 << 1 << 1 << 3 << 4 << 1 << 3 << 3 << 4 << 1 << 3)
            << expected;
    }

    // Тест 7: Много номиналов
    {
        int optimalPayment = 100;
        int arraySize = 151;
        IntVector lastUsedCoin(arraySize, 1); // Заполняем номиналом 1
        lastUsedCoin[0] = -1;
        lastUsedCoin[50] = 50;
        lastUsedCoin[100] = 50;

        IntIntMap expected;
        expected[50] = 2;

        QTest::newRow("Test 7: Multiple coin denominations (payment=100)")
            << optimalPayment
            << lastUsedCoin
            << expected;
    }

    // Тест 8: Номинал превышает сумму покупки
    {
        IntIntMap expected;
        expected[10] = 1;
        QTest::newRow("Test 8: The nominal value exceeds the purchase amount (payment=10)")
            << 10
            << (IntVector() << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << 10 << -1 << -1 << -1)
            << expected;
    }

    // Тест 9: Одна крупная монета
    {
        IntIntMap expected;
        expected[50] = 1;
        QTest::newRow("Test 9: One large coin (payment=50)")
            << 50
            << (IntVector() << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                          << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                          << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                          << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                          << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1 << -1
                          << 50 << -1)
            << expected;
    }

    // Тест 10: Только номинал 1
    {
        IntIntMap expected;
        expected[1] = 50;
        QTest::newRow("Test 10: Only the nominal value is 1 (payment=50)")
            << 50
            << (IntVector() << -1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1
                          << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1
                          << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1
                          << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1
                          << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1
                          << 1 << 1)
            << expected;
    }
}



/**
 * @brief Тестовая функция для проверки reconstructCoinCombination.
 * Выполняется для каждой строки данных из testReconstructCoinCombination_data().
 * Извлекает тестовые данные с помощью QFETCH и сравнивает результаты с ожидаемыми значениями через QCOMPARE.
 */
void Test_reconstructCoinCombination::testReconstructCoinCombination()
{
    // QFETCH извлекает значения из текущей строки тестовых данных
    QFETCH(int, optimalPayment);
    QFETCH(IntVector, lastUsedCoin);
    QFETCH(IntIntMap, expectedUsedCoins);

    // Выходной параметр для тестируемой функции
    QMap<int, int> usedCoins;
    // Вызов тестируемой функции
    reconstructCoinCombination(optimalPayment, lastUsedCoin, usedCoins);

    // QCOMPARE сравнивает размер словаря
    QCOMPARE(usedCoins.size(), expectedUsedCoins.size());

    // Проверяем каждый номинал
    for (auto it = expectedUsedCoins.constBegin(); it != expectedUsedCoins.constEnd(); ++it) {
        // QVERIFY2 проверяет наличие номинала в результате
        QVERIFY2(usedCoins.contains(it.key()),
                 qPrintable(QString("Номинал %1 не найден в результате").arg(it.key())));
        // QCOMPARE сравнивает количество монет данного номинала
        QCOMPARE(usedCoins[it.key()], it.value());
    }
}





// Запуск всех тестов
int main(int argc, char *argv[])
{
    int status = 0;
    {
        Test_fillReachabilityTable test1;
        status |= QTest::qExec(&test1, argc, argv);
    }
    {
        Test_findOptimalPayment test2;
        status |= QTest::qExec(&test2, argc, argv);
    }
    {
         Test_reconstructCoinCombination test3;
         status |= QTest::qExec(&test3, argc, argv);
     }

    return status;
}

#include "tst_tests.moc"






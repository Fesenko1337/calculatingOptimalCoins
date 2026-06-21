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
Q_DECLARE_METATYPE(errorType) // Регистрация перечисления errorType для использования в тестах


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
    // Определение столбцов тестовых данных
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




/**
 * @brief Класс тестов для функции calculateOptimalCoins.
 * Наследуется от QObject и использует фреймворк Qt Test для модульного тестирования.
 * Содержит data-функцию для предоставления тестовых данных и тестовую функцию для проверки.
 * Тестирует полную цепочку вычислений: от входных данных до финального результата.
 */
class Test_calculateOptimalCoins : public QObject
{
    Q_OBJECT

private slots:
    void testCalculateOptimalCoins_data(); // Data-функция - предоставляет набор тестовых данных
    void testCalculateOptimalCoins(); // Тестовая функция - выполняется для каждой строки данных
};



/**
 * @brief Data-функция для тестирования calculateOptimalCoins.
 * Определяет столбцы тестовых данных и добавляет строки с тестовыми случаями.
 * Каждая строка (newRow) будет протестирована отдельно в тестовой функции.
 */
void Test_calculateOptimalCoins::testCalculateOptimalCoins_data()
{
    // Определение столбцов тестовых данных
    QTest::addColumn<int>("purchaseSum"); // Сумма покупки (входной параметр)
    QTest::addColumn<IntVector>("nominals"); // Номиналы монет (входной параметр)
    QTest::addColumn<int>("expectedChange"); // Ожидаемая сдача (выходной параметр)
    QTest::addColumn<IntIntMap>("expectedUsedCoins"); // Ожидаемое количество монет каждого номинала (выходной параметр)

    // Тест 1: Типовой пример
    {
        IntIntMap expected;
        expected[6] = 3;
        expected[3] = 1;
        QTest::newRow("Test 1: Typical example (sum=20, nom=[3,6])")
            << 20 // purchaseSum
            << (IntVector() << 3 << 6) // nominals
            << 1 // expectedChange (21 - 20 = 1)
            << expected; // expectedUsedCoins
    }

    // Тест 2: Точная оплата
    {
        IntIntMap expected;
        expected[5] = 2;
        QTest::newRow("Test 2: Accurate payment (sum=10, nom=[2,5])")
            << 10
            << (IntVector() << 2 << 5)
            << 0
            << expected;
    }

    // Тест 3: Оплата со сдачей
    {
        IntIntMap expected;
        expected[3] = 1;
        expected[5] = 1;
        QTest::newRow("Test 3: Payment with change (sum=7, nom=[3,5])")
            << 7
            << (IntVector() << 3 << 5)
            << 1
            << expected;
    }

    // Тест 4: Граничный минимум
    {
        IntIntMap expected;
        expected[1] = 1;
        QTest::newRow("Test 4: Boundary Minimum (sum=1, nom=[1])")
            << 1
            << (IntVector() << 1)
            << 0
            << expected;
    }

    // Тест 5: Граничный максимум
    {
        IntIntMap expected;
        expected[100] = 10;
        QTest::newRow("Test 5: Boundary Maximum (sum=1000, nom=[100])")
            << 1000
            << (IntVector() << 100)
            << 0
            << expected;
    }

    // Тест 6: Номинал больше суммы покупки
    {
        IntIntMap expected;
        expected[5] = 1;
        QTest::newRow("Test 6: The nominal value is more than the purchase amount (sum=4, nom=[5])")
            << 4
            << (IntVector() << 5)
            << 1
            << expected;
    }

    // Тест 7: Одна крупная монета
    {
        IntIntMap expected;
        expected[50] = 1;
        QTest::newRow("Test 7: One large coin (sum=1, nom=[50])")
            << 1
            << (IntVector() << 50)
            << 49
            << expected;
    }

    // Тест 8: Предпочтение одной крупной монеты нескольким меньшим
    {
        IntIntMap expected;
        expected[10] = 1;
        QTest::newRow("Test 8: Preference for one large coin (sum=10, nom=[1,9,10])")
            << 10
            << (IntVector() << 1 << 9 << 10)
            << 0
            << expected;
    }

    // Тест 9: Преимущество над жадным алгоритмом
    {
        IntIntMap expected;
        expected[3] = 2;
        QTest::newRow("Test 9: The advantage over the greedy algorithm (sum=6, nom=[1,3,4])")
            << 6
            << (IntVector() << 1 << 3 << 4)
            << 0
            << expected;
    }

    // Тест 10: Несколько равносильных решений
    {
        IntIntMap expected;
        expected[1] = 1;
        expected[3] = 1;
        QTest::newRow("Test 10: Several equivalent solutions (sum=4, nom=[1,2,3])")
            << 4
            << (IntVector() << 1 << 2 << 3)
            << 0
            << expected;
    }

    // Тест 11: Плотный набор номиналов
    {
        IntIntMap expected;
        expected[25] = 2;
        QTest::newRow("Test 11: A dense set of denominations (sum=50, nom=[1,2,5,10,20,25])")
            << 50
            << (IntVector() << 1 << 2 << 5 << 10 << 20 << 25)
            << 0
            << expected;
    }

    // Тест 12: Только номинал 1
    {
        IntIntMap expected;
        expected[1] = 50;
        QTest::newRow("Test 12: Only the nominal value of 1 (sum=50, nom=[1])")
            << 50
            << (IntVector() << 1)
            << 0
            << expected;
    }

    // Тест 13: Приоритет меньшей сдачи при равном количестве монет
    {
        IntIntMap expected;
        expected[6] = 2;
        QTest::newRow("Test 13: The priority of a lower pass (sum=10, nom=[6,9])")
            << 10
            << (IntVector() << 6 << 9)
            << 2
            << expected;
    }

    // Тест 14: Нулевая сдача при нескольких вариантах
    {
        IntIntMap expected;
        expected[21] = 2;
        QTest::newRow("Test 14: Zero change for multiple solutions (sum=42, nom=[7,14,21])")
            << 42
            << (IntVector() << 7 << 14 << 21)
            << 0
            << expected;
    }

    // Тест 15: Все номиналы больше суммы покупки
    {
        IntIntMap expected;
        expected[25] = 1;
        QTest::newRow("Test 15: All denominations are greater than the purchase amount (sum=10, nom=[25,50,100])")
            << 10
            << (IntVector() << 25 << 50 << 100)
            << 15
            << expected;
    }
}



/**
 * @brief Тестовая функция для проверки calculateOptimalCoins.
 * Выполняется для каждой строки данных из testCalculateOptimalCoins_data().
 * Извлекает тестовые данные с помощью QFETCH и сравнивает результаты с ожидаемыми значениями через QCOMPARE.
 */
void Test_calculateOptimalCoins::testCalculateOptimalCoins()
{
    // QFETCH извлекает значения из текущей строки тестовых данных
    QFETCH(int, purchaseSum);
    QFETCH(IntVector, nominals);
    QFETCH(int, expectedChange);
    QFETCH(IntIntMap, expectedUsedCoins);

    // Выходные параметры для тестируемой функции
    int change = 0;
    QMap<int, int> usedCoins;
    // Вызов тестируемой функции
    calculateOptimalCoins(purchaseSum, nominals, change, usedCoins);

    // QCOMPARE сравнивает фактическую сдачу с ожидаемой
    QCOMPARE(change, expectedChange);

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

    int totalSum = 0;
    for (auto it = usedCoins.constBegin(); it != usedCoins.constEnd(); ++it) {
        totalSum += it.key() * it.value();
    }
    // QCOMPARE проверяет, что общая сумма монет равна purchaseSum + change
    QCOMPARE(totalSum, purchaseSum + change);
}




/**
 * @brief Вспомогательная функция для проверки наличия конкретной ошибки в множестве.
 * Ищет в множестве ошибок ошибку с указанным типом, номером строки и значением.
 * Для номера строки допускается погрешность ±1 из-за особенностей QXmlStreamReader.
 * @param errors - множество ошибок для поиска.
 * @param type - тип искомой ошибки.
 * @param line - ожидаемый номер строки (-1, если не проверяем).
 * @param value - ожидаемое некорректное значение ("" если не проверяем).
 * @return true, если ошибка найдена, иначе false.
 */
bool hasError(const QSet<Error>& errors, errorType type, int line = -1, const QString& value = "")
{
    for (const Error& err : errors) {
        if (err.type == type) {
            // Проверяем номер строки только если он указан и не равен -1
            // Допускаем погрешность ±1 из-за особенностей QXmlStreamReader
            if (line != -1 && qAbs(err.lineNumber - line) > 1) continue;

            // Проверяем значение только если оно не пустое
            if (!value.isEmpty() && err.invalidValue != value) continue;
            return true;
        }
    }
    return false;
}



/**
 * @brief Класс тестов для функции parseAndValidateData.
 * Наследуется от QObject и использует фреймворк Qt Test для модульного тестирования.
 * Содержит data-функцию для предоставления тестовых данных и тестовую функцию для проверки.
 * Также содержит отдельные методы для тестов с несколькими ошибками, которые не помещаются
 * в стандартную data-driven структуру QTest.
 */
class Test_parseAndValidateData : public QObject
{
    Q_OBJECT

private slots:
    void testParseAndValidateData_data(); // Data-функция - предоставляет набор тестовых данных для одиночных ошибок
    void testParseAndValidateData(); // Тестовая функция - выполняется для каждой строки данных

    // Отдельные методы для тестов с несколькими ошибками
    void testParseAndValidateData_Test11(); // Тест 11: Номиналы монет не являются целыми числами (2 ошибки)
    void testParseAndValidateData_Test12(); // Тест 12: Номиналы монет меньше минимально допустимого (2 ошибки)
    void testParseAndValidateData_Test14(); // Тест 14: Количество номиналов больше максимума (2 ошибки)
    void testParseAndValidateData_Test23(); // Тест 23: Смешанные валидные и некорректные номиналы (2 ошибки)
    void testParseAndValidateData_Test24(); // Тест 24: Комбинация ошибок данных сумма + номиналы (3 ошибки)
    void testParseAndValidateData_Test25(); // Тест 25: Логические ошибки номиналов дубликат + диапазон (2 ошибки)
    void testParseAndValidateData_Test26(); // Тест 26: Комбинация структурных и ошибок номиналов (3 ошибки)
};



/**
 * @brief Data-функция для тестирования parseAndValidateData.
 * Определяет столбцы тестовых данных и добавляет строки с тестовыми случаями.
 * Каждая строка (newRow) будет протестирована отдельно в тестовой функции.
 * Тесты охватывают валидные данные, одиночные ошибки валидации и структурные ошибки XML.
 */
void Test_parseAndValidateData::testParseAndValidateData_data()
{
    QTest::addColumn<QString>("xmlContent"); // Содержимое XML-файла (входной параметр)
    QTest::addColumn<bool>("expectedResult"); // Ожидаемый результат парсинга (true - успешно, false - ошибка)
    QTest::addColumn<int>("expectedPurchaseSum"); // Ожидаемая сумма покупки (проверяется только при успехе)
    QTest::addColumn<IntVector>("expectedNominals"); // Ожидаемые номиналы монет (проверяются только при успехе)
    QTest::addColumn<int>("expectedErrorCount"); // Ожидаемое количество ошибок
    QTest::addColumn<errorType>("expectedErrorType"); // Ожидаемый тип первой ошибки (noError если нет ошибок)
    QTest::addColumn<int>("expectedErrorLine"); // Ожидаемый номер строки ошибки (-1 если не проверяем)
    QTest::addColumn<QString>("expectedErrorValue"); // Ожидаемое некорректное значение ("" если не проверяем)

    // Тест 1: Валидные данные: базовый случай
    QTest::newRow("Test 1: Valid data: the base case")
        << QString("<config>\n<sum>10</sum>\n<nominals>\n<value>2</value>\n<value>5</value>\n</nominals>\n</config>")
        << true << 10 << (IntVector() << 2 << 5) << 0 << noError << -1 << QString("");

    // Тест 2: Валидные данные: граничные значения
    QTest::newRow("Test 2: Valid data: boundary values (minimum)")
        << QString("<config>\n<sum>1</sum>\n<nominals>\n<value>1</value>\n</nominals>\n</config>")
        << true << 1 << (IntVector() << 1) << 0 << noError << -1 << QString("");

    // Тест 3: Валидные данные: граничные значения
    QTest::newRow("Test 3: Valid data: boundary values (maximum)")
        << QString("<config>\n<sum>1000</sum>\n<nominals>\n<value>100</value>\n</nominals>\n</config>")
        << true << 1000 << (IntVector() << 100) << 0 << noError << -1 << QString("");

    // Тест 4: Валидные данные: несколько номиналов
    QTest::newRow("Test 4: Valid data: multiple denominations")
        << QString("<config>\n<sum>100</sum>\n<nominals>\n<value>10</value>\n<value>20</value>\n<value>50</value>\n<value>70</value>\n<value>100</value>\n</nominals>\n</config>")
        << true << 100 << (IntVector() << 10 << 20 << 50 << 70 << 100) << 0 << noError << -1 << QString("");

    // Тест 5: Валидные данные: номинал больше суммы покупки
    QTest::newRow("Test 5: Valid data: the nominal value is more than the purchase amount")
        << QString("<config>\n<sum>1</sum>\n<nominals>\n<value>50</value>\n</nominals>\n</config>")
        << true << 1 << (IntVector() << 50) << 0 << noError << -1 << QString("");

    // Тест 6: Сумма покупки больше максимально допустимой
    QTest::newRow("Test 6: The purchase amount is more than the maximum allowed")
        << QString("<config>\n<sum>15000</sum>\n<nominals>\n<value>1</value>\n<value>7</value>\n</nominals>\n</config>")
        << false << 0 << IntVector() << 1 << purchaseSumExceedsMax << 2 << QString("15000");

    // Тест 7: Сумма покупки меньше минимально допустимой
    QTest::newRow("Test 7: The purchase amount is less than the minimum allowed")
        << QString("<config>\n<sum>-500</sum>\n<nominals>\n<value>19</value>\n<value>4</value>\n</nominals>\n</config>")
        << false << 0 << IntVector() << 1 << purchaseSumLessMin << 2 << QString("-500");

    // Тест 8: Сумма покупки не является целым числом
    QTest::newRow("Test 8: The purchase amount is not an integer")
        << QString("<config>\n<sum>14.03</sum>\n<nominals>\n<value>3</value>\n<value>9</value>\n</nominals>\n</config>")
        << false << 0 << IntVector() << 1 << purchaseSumNotInteger << 2 << QString("14.03");

    // Тест 9: Сумма покупки не является числом
    QTest::newRow("Test 9: The purchase amount is not a number")
        << QString("<config>\n<sum>A</sum>\n<nominals>\n<value>21</value>\n<value>5</value>\n</nominals>\n</config>")
        << false << 0 << IntVector() << 1 << purchaseSumNotNumber << 2 << QString("A");

    // Тест 10: Номиналы монет не являются числами
    QTest::newRow("Test 10: Coin denominations are not numbers")
        << QString("<config>\n<sum>150</sum>\n<nominals>\n<value>$</value>\n<value>16</value>\n</nominals>\n</config>")
        << false << 0 << IntVector() << 1 << nominalNotNumber << 4 << QString("$");

    // Тест 13: Номиналы монет повторяются
    QTest::newRow("Test 13: Coin denominations are repeated")
        << QString("<config>\n<sum>500</sum>\n<nominals>\n<value>5</value>\n<value>5</value>\n</nominals>\n</config>")
        << false << 0 << IntVector() << 1 << nominalDuplicate << 5 << QString("5");

    // Тест 15: Номинал монеты превышает максимально допустимый
    QTest::newRow("Test 15: The coin's face value exceeds the maximum allowed value")
        << QString("<config>\n<sum>100</sum>\n<nominals>\n<value>10</value>\n<value>150</value>\n</nominals>\n</config>")
        << false << 0 << IntVector() << 1 << nominalExceedsMax << 5 << QString("150");

    // Тест 16: Неопознанный тег
    QTest::newRow("Test 16: Unknown tag")
        << QString("<config>\n<rub>300</rub>\n<sum>935</sum>\n<nominals>\n<value>12</value>\n<value>210</value>\n<value>89</value>\n</nominals>\n</config>")
        << false << 0 << IntVector() << 2 << unknownTag << 2 << QString("rub");

    // Тест 17: Отсутствует обязательный тег
    QTest::newRow("Test 17: Missing required tag")
        << QString("<config>\n<nominals>\n<value>27</value>\n<value>90</value>\n</nominals>\n</config>")
        << false << 0 << IntVector() << 1 << missingTag << -1 << QString("sum");

    // Тест 18: Некорректное расположение тега
    QTest::newRow("Test 18: Incorrect tag location")
        << QString("<config>\n<value>2</value>\n<sum>10</sum>\n<nominals>\n<value>1</value>\n<value>3</value>\n</nominals>\n</config>")
        << false << 0 << IntVector() << 1 << incorrectTagLocation << 2 << QString("value");

    // Тест 19: Некорректный синтаксис XML
    QTest::newRow("Test 19: Incorrect XML syntax")
        << QString("<config\n<sum>670</sum>\n<nominals>\n<value>1</value>\n<value>2</value>\n<value>39</value>\n</nominals>\n</config>")
        << false << 0 << IntVector() << 1 << invalidXmlSyntax << 1 << QString("");

    // Тест 20: Пустое содержимое тега суммы
    QTest::newRow("Test 20: Empty amount tag content")
        << QString("<config>\n<sum></sum>\n<nominals>\n<value>5</value>\n</nominals>\n</config>")
        << false << 0 << IntVector() << 1 << purchaseSumNotNumber << 2 << QString("");

    // Тест 21: Зависимость от регистра
    QTest::newRow("Test 21: Case dependence")
        << QString("<config>\n<SUM>10</SUM>\n<nominals>\n<value>5</value>\n</nominals>\n</config>")
        << true << 10 << (IntVector() << 5) << 0 << noError << -1 << QString("");

    // Тест 22: Пробелы и переносы строк
    QTest::newRow("Test 22: Spaces and line breaks")
        << QString("<config>\n <sum> 42 </sum>\n <nominals>\n <value> 5 </value>\n </nominals>\n</config>")
        << true << 42 << (IntVector() << 5) << 0 << noError << -1 << QString("");
}



/**
 * @brief Тестовая функция для проверки parseAndValidateData.
 * Выполняется для каждой строки данных из testParseAndValidateData_data().
 * Извлекает тестовые данные с помощью QFETCH и сравнивает результаты с ожидаемыми значениями через QCOMPARE.
 */
void Test_parseAndValidateData::testParseAndValidateData()
{
    // QFETCH извлекает значения из текущей строки тестовых данных
    QFETCH(QString, xmlContent);
    QFETCH(bool, expectedResult);
    QFETCH(int, expectedPurchaseSum);
    QFETCH(IntVector, expectedNominals);
    QFETCH(int, expectedErrorCount);
    QFETCH(errorType, expectedErrorType);
    QFETCH(int, expectedErrorLine);
    QFETCH(QString, expectedErrorValue);

    // Выходные параметры для тестируемой функции
    int purchaseSum = 0;
    QVector<int> nominals;
    QSet<Error> errors;
    // Вызов тестируемой функции
    bool result = parseAndValidateData(xmlContent, purchaseSum, nominals, errors);
    // QCOMPARE сравнивает фактический результат с ожидаемым
    QCOMPARE(result, expectedResult);
    // QCOMPARE сравнивает количество ошибок
    QCOMPARE(errors.size(), expectedErrorCount);

    // Если парсинг успешен - проверяем извлечённые данные
    if (expectedResult) {
        // QCOMPARE сравнивает сумму покупки
        QCOMPARE(purchaseSum, expectedPurchaseSum);
        // QCOMPARE сравнивает размер вектора номиналов
        QCOMPARE(nominals.size(), expectedNominals.size());
        // Проверяем каждый номинал
        for (int i = 0; i < expectedNominals.size(); ++i) {
            QCOMPARE(nominals[i], expectedNominals[i]);
        }
    }
    // Если парсинг неуспешен - проверяем наличие конкретной ошибки
    else if (expectedErrorType != noError) {
        // QVERIFY2 проверяет наличие ошибки с указанными параметрами
        QVERIFY2(hasError(errors, expectedErrorType, expectedErrorLine, expectedErrorValue),
                 qPrintable(QString("Ожидаемая ошибка type=%1, line=%2, value='%3' не найдена")
                            .arg(expectedErrorType).arg(expectedErrorLine).arg(expectedErrorValue)));
    }
}

/**
 * @brief Тест 11: Номиналы монет не являются целыми числами (2 ошибки).
 * Отдельный тест, так как проверяет наличие двух ошибок типа nominalNotInteger одновременно.
 */
void Test_parseAndValidateData::testParseAndValidateData_Test11()
{
    qDebug() << "Test 11: Coin denominations are not integers (2 errors)";
    QString xml = "<config>\n<sum>112</sum>\n<nominals>\n<value>3.14</value>\n<value>9.67</value>\n</nominals>\n</config>";
    int purchaseSum = 0;
    QVector<int> nominals;
    QSet<Error> errors;

    bool result = parseAndValidateData(xml, purchaseSum, nominals, errors);

    QVERIFY(!result); // QVERIFY проверяет, что парсинг завершился с ошибкой
    QCOMPARE(errors.size(), 2); // QCOMPARE проверяет количество ошибок
    // QVERIFY проверяет наличие каждой конкретной ошибки
    QVERIFY(hasError(errors, nominalNotInteger, 4, "3.14"));
    QVERIFY(hasError(errors, nominalNotInteger, 5, "9.67"));
}

/**
 * @brief Тест 12: Номиналы монет меньше минимально допустимого (2 ошибки).
 * Отдельный тест, так как проверяет наличие двух ошибок типа nominalLessMin одновременно.
 */
void Test_parseAndValidateData::testParseAndValidateData_Test12()
{
    qDebug() << "Test 12: Coin denominations are less than the minimum allowed (2 errors)";
    QString xml = "<config>\n<sum>1000</sum>\n<nominals>\n<value>-6</value>\n<value>-491</value>\n</nominals>\n</config>";
    int purchaseSum = 0;
    QVector<int> nominals;
    QSet<Error> errors;

    bool result = parseAndValidateData(xml, purchaseSum, nominals, errors);

    QVERIFY(!result);
    QCOMPARE(errors.size(), 2);
    QVERIFY(hasError(errors, nominalLessMin, 4, "-6"));
    QVERIFY(hasError(errors, nominalLessMin, 5, "-491"));
}

/**
 * @brief Тест 14: Количество номиналов больше максимума (2 ошибки).
 * Отдельный тест, так как проверяет наличие двух разных типов ошибок одновременно:
 * nominalExceedsMax и nominalsCountExceedsMax.
 */
void Test_parseAndValidateData::testParseAndValidateData_Test14()
{
    qDebug() << "Test 14: The number of denominations is greater than the maximum and the last denomination exceeds the maximum value (2 errors)";
    QString xml = "<config>\n<sum>670</sum>\n<nominals>\n";
    // Генерируем 101 номинал для проверки ограничения на количество
    for (int i = 1; i <= 101; ++i) {
        xml += QString("<value>%1</value>\n").arg(i);
    }
    xml += "</nominals>\n</config>";

    int purchaseSum = 0;
    QVector<int> nominals;
    QSet<Error> errors;

    bool result = parseAndValidateData(xml, purchaseSum, nominals, errors);

    QVERIFY(!result);
    QCOMPARE(errors.size(), 2);

    // Проверяем обе ошибки
    QVERIFY(hasError(errors, nominalExceedsMax, 104, "101"));
    QVERIFY(hasError(errors, nominalsCountExceedsMax, 104, "101"));
}

/**
 * @brief Тест 23: Смешанные валидные и некорректные номиналы (2 ошибки).
 * Отдельный тест, так как проверяет наличие двух разных типов ошибок одновременно:
 * nominalLessMin и nominalNotNumber.
 */
void Test_parseAndValidateData::testParseAndValidateData_Test23()
{
    qDebug() << "Test 23: Mixed valid and incorrect denominations (2 errors)";
    QString xml = "<config>\n<sum>50</sum>\n<nominals>\n<value>5</value>\n<value>-1</value>\n<value>10</value>\n<value>a</value>\n</nominals>\n</config>";
    int purchaseSum = 0;
    QVector<int> nominals;
    QSet<Error> errors;

    bool result = parseAndValidateData(xml, purchaseSum, nominals, errors);

    QVERIFY(!result);
    QCOMPARE(errors.size(), 2);
    QVERIFY(hasError(errors, nominalLessMin, 5, "-1"));
    QVERIFY(hasError(errors, nominalNotNumber, 7, "a"));
}

/**
 * @brief Тест 24: Комбинация ошибок данных сумма + номиналы (3 ошибки).
 * Отдельный тест, так как проверяет наличие трёх разных типов ошибок одновременно:
 * purchaseSumLessMin, nominalLessMin и nominalNotNumber.
 */
void Test_parseAndValidateData::testParseAndValidateData_Test24()
{
    qDebug() << "Test 24: Combination of data errors sum + denominations (3 errors)";
    QString xml = "<config>\n<sum>0</sum>\n<nominals>\n<value>-5</value>\n<value>abc</value>\n</nominals>\n</config>";
    int purchaseSum = 0;
    QVector<int> nominals;
    QSet<Error> errors;

    bool result = parseAndValidateData(xml, purchaseSum, nominals, errors);

    QVERIFY(!result);
    QCOMPARE(errors.size(), 3);
    QVERIFY(hasError(errors, purchaseSumLessMin, 2, "0"));
    QVERIFY(hasError(errors, nominalLessMin, 4, "-5"));
    QVERIFY(hasError(errors, nominalNotNumber, 5, "abc"));
}

/**
 * @brief Тест 25: Логические ошибки номиналов дубликат + диапазон (2 ошибки)
 * Отдельный тест, так как проверяет наличие двух разных типов ошибок одновременно:
 * nominalDuplicate и nominalExceedsMax.
 */
void Test_parseAndValidateData::testParseAndValidateData_Test25()
{
    qDebug() << "Test 25: Logical errors of denominations duplicate + range (2 errors)";
    QString xml = "<config>\n<sum>50</sum>\n<nominals>\n<value>5</value>\n<value>5</value>\n<value>150</value>\n</nominals>\n</config>";
    int purchaseSum = 0;
    QVector<int> nominals;
    QSet<Error> errors;

    bool result = parseAndValidateData(xml, purchaseSum, nominals, errors);

    QVERIFY(!result);
    QCOMPARE(errors.size(), 2);
    QVERIFY(hasError(errors, nominalDuplicate, 5, "5"));
    QVERIFY(hasError(errors, nominalExceedsMax, 6, "150"));
}

/**
 * @brief Тест 26: Комбинация структурных и ошибок номиналов (3 ошибки)
 * Отдельный тест, так как проверяет наличие трёх разных типов ошибок одновременно:
 * unknownTag, missingTag и nominalExceedsMax.
 */
void Test_parseAndValidateData::testParseAndValidateData_Test26()
{
    qDebug() << "Test 26: Combination of structural and nominal errors (3 errors)";
    QString xml = "<config>\n<unknown>333</unknown>\n<nominals>\n<value>200</value>\n</nominals>\n</config>";
    int purchaseSum = 0;
    QVector<int> nominals;
    QSet<Error> errors;

    bool result = parseAndValidateData(xml, purchaseSum, nominals, errors);

    QVERIFY(!result);
    QCOMPARE(errors.size(), 3);
    QVERIFY(hasError(errors, unknownTag, 2, "unknown"));
    QVERIFY(hasError(errors, missingTag, -1, "sum"));
    QVERIFY(hasError(errors, nominalExceedsMax, 4, "200"));
}




/**
 * @brief Главная функция тестового приложения.
 * Последовательно запускает все тестовые классы и накапливает статус выполнения.
 * @param argc - количество аргументов командной строки.
 * @param argv - массив аргументов командной строки.
 * @return 0, если все тесты пройдены успешно, иначе ненулевое значение.
 */
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
    {
        Test_calculateOptimalCoins test4;
        status |= QTest::qExec(&test4, argc, argv);
    }
    {
        Test_parseAndValidateData test5;
        status |= QTest::qExec(&test5, argc, argv);
    }
    return status;
}

#include "tst_tests.moc"






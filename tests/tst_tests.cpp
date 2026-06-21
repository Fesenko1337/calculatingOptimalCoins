#include <QtTest>
#include <QVector>
#include "coinSolver.h"
#include "xmlParser.h"
#include "error.h"
#include <QSet>
#include <QString>

typedef QVector<int> IntVector;
Q_DECLARE_METATYPE(IntVector) // Регистрация типа QVector<int> для использования в тестах



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

QTEST_MAIN(Test_fillReachabilityTable)
#include "tst_tests.moc"






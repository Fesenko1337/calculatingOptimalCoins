#include "coinSolver.h"




int fillReachabilityTable(int purchaseSum, const QVector<int>& nominals, QVector<int>& minCoins, QVector<int>& lastUsedCoin)
{
    // Найти максимальный номинал в списке номиналов монет
    int maxNominal = 0;
    if (!nominals.isEmpty())
    {
        maxNominal = *std::max_element(nominals.begin(), nominals.end());
    }
    // Вычислить максимально возможную сумму оплаты как сумму суммы покупки и максимального номинала
    int maxSearchLimit = purchaseSum + maxNominal;

    // Установить размер массива равным максимально возможной сумме оплаты + 1
    minCoins.resize(maxSearchLimit + 1);
    for (int sum = 0; sum <= maxSearchLimit; ++sum)
    {
        minCoins[sum] = -1; // Заполнить все элементы таблицы значением, обозначающим недостижимость суммы имеющимися номиналами монет “-1”
    }
    // Установить количество столбцов таблицы равным максимально возможной сумме оплаты + 1
    lastUsedCoin.resize(maxSearchLimit + 1);
    for (int sum = 0; sum <= maxSearchLimit; ++sum)
    {
        lastUsedCoin[sum] = -1; // Заполнить все элементы таблицы значением -1
    }

    minCoins[0] = 0; // Считать, что сумма 0 достижима с количеством монет 0

    // Для каждой суммы от 1 до максимально возможной суммы оплаты
    for (int currentSum = 1; currentSum <= maxSearchLimit; ++currentSum)
    {
        // Для каждого номинала из списка номиналов монет
        for (int coin : nominals)
        {
            // Вычислить остаточную сумму как разность текущей суммы и номинала монеты
            int remainder = currentSum - coin;

            // Если существует способ набрать остаточную сумму имеющимися монетами
            if (remainder >= 0 && minCoins[remainder] != -1)
            {
                // Рассчитать кандидатное количество монет как количество монет для остаточной суммы + 1
                int candidateNumberOfCoins = minCoins[remainder] + 1;

                // Если кандидатное количество монет меньше текущего записанного для этой суммы
                if (minCoins[currentSum] == -1 || candidateNumberOfCoins < minCoins[currentSum])
                {
                    minCoins[currentSum] = candidateNumberOfCoins; // Обновить минимальное количество монет для этой суммы
                    lastUsedCoin[currentSum] = coin; // Запомнить текущий номинал как последний использованный для этой суммы
                }
            }
        }
    }
    return maxSearchLimit; // Вернуть максимально возможную сумму оплаты
}




void findOptimalPayment(int purchaseSum, int maxSearchLimit, const QVector<int>& minCoins, int& optimalPayment, int& change)
{
    optimalPayment = -1; // Считать, что оптимальная сумма оплаты ещё не найдена
    int minCoinsCount = -1; // Считать, что минимальное количество монет не определено
    // Для каждой суммы от суммы покупки до максимально возможной суммы оплаты
    for (int currentSum = purchaseSum; currentSum <= maxSearchLimit; ++currentSum)
    {
        // Если для суммы определено минимальное количество монет
        if (minCoins[currentSum] != -1)
        {
            int currentCoinsCount = minCoins[currentSum];
            /*
             * Если оптимальная сумма оплаты ещё не найдена
             * ИЛИ количество монет меньше уже найденного
            */
            if (optimalPayment == -1 || currentCoinsCount < minCoinsCount)
            {
                optimalPayment = currentSum; // Зафиксировать текущую сумму как оптимальную сумму оплаты
                minCoinsCount = currentCoinsCount; // Зафиксировать соответствующее текущей сумме количество монет как минимальное
            }
        }
    }
    if (optimalPayment != -1)
    {
        // Вычислить сдачу как разность между оптимальной суммой оплаты и суммой покупки
        change = optimalPayment - purchaseSum;
    }
    else
    {
        change = 0;
    }
}




void reconstructCoinCombination(int optimalPayment, const QVector<int>& lastUsedCoin, QMap<int, int>& usedCoins)
{
    usedCoins.clear();
    // Считать, что текущая сумма равна оптимальной сумме оплаты
    int currentSum = optimalPayment;

    // Пока текущая сумма больше 0
    while (currentSum > 0)
    {
        // Извлечь номинал, записанный как последний использованный для текущей суммы
        int coin = lastUsedCoin[currentSum];

        // Увеличить количество данного номинала в usedCoins на 1
        usedCoins[coin]++;

        // Уменьшить текущую сумму на значение этого номинала
        currentSum -= coin;

    }
}




void calculateOptimalCoins(int purchaseSum, const QVector<int>& nominals, int& change, QMap<int, int>& usedCoins)
{
    QVector<int> minCoins;
    QVector<int> lastUsedCoin;
    int optimalPayment = -1;

    // Для каждой возможной суммы оплаты вычислить минимальное количество монет и записать последний использованный номинал
    int maxSearchLimit = fillReachabilityTable(purchaseSum, nominals, minCoins, lastUsedCoin);

    // Найти оптимальную сумму оплаты и вычислить сдачу
    findOptimalPayment(purchaseSum, maxSearchLimit, minCoins, optimalPayment, change);

    if (optimalPayment > 0)
    {
        // Восстановить состав всех используемых номиналов монет для оптимальной суммы оплаты
        reconstructCoinCombination(optimalPayment, lastUsedCoin, usedCoins);
    }
    else
    {
        usedCoins.clear();
    }

}


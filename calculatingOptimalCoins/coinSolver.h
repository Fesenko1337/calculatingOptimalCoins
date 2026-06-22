#ifndef COINSOLVER_H
#define COINSOLVER_H
#include <QVector>
#include <QMap>


/**
 * @brief
 * Вычисляет минимальное количество монет для всех сумм от 0 до максимально возможной суммы,
 * заполняет таблицу последнего использованного номинала для каждой суммы.
 * @param purchaseSum - сумма покупки.
 * @param nominals - список доступных номиналов монет.
 * @param minCoins - заполненная таблица минимального количества монет для каждой суммы.
 * @param lastUsedCoin - заполненная таблица последнего использованного номинала для каждой суммы.
 * @return maxSearchLimit - максимально возможная сумма оплаты (purchaseSum + maxNominal).
 */
int fillReachabilityTable(int purchaseSum, const QVector<int>& nominals, QVector<int>& minCoins, QVector<int>& lastUsedCoin);

/**
 * @brief
 * Находит оптимальную сумму оплаты по критериям минимального количества монет
 * и минимальной сдачи, вычисляет сдачу как разность найденной оптимальной суммы и суммы покупки.
 * @param purchaseSum - сумма покупки.
 * @param maxSearchLimit - верхняя граница сумм для поиска.
 * @param minCoins - заполненная таблица минимального количества монет.
 * @param optimalPayment - найденная оптимальная сумма оплаты.
 * @param change - величина сдачи.
 * @return void.
 */
void findOptimalPayment(int purchaseSum, int maxSearchLimit, const QVector<int>& minCoins, int& optimalPayment, int& change);

/**
 * @brief
 * Восстанавливает использованные монеты и подсчитывает количество каждого номинала
 * путём обратного прохода по таблице последнего использованного номинала.
 * @param optimalPayment - оптимальная сумма оплаты.
 * @param lastUsedCoin - таблица последнего использованного номинала.
 * @param usedCoins - словарь, где ключ - номинал монеты, значение - количество использованных монет данного номинала.
 * @return void.
 */
void reconstructCoinCombination(int optimalPayment, const QVector<int>& lastUsedCoin, QMap<int, int>& usedCoins);

/**
 * @brief
 * Рассчитывает оптимальное количество монет каждого номинала для оплаты данной покупки
 * и сдачу, полученную после покупки.
 * @param purchaseSum - сумма покупки.
 * @param nominals - список доступных номиналов монет.
 * @param change - величина сдачи.
 * @param usedCoins - словарь, где ключ - номинал монеты, значение - количество использованных монет данного номинала.
 * @return void.
 */
void calculateOptimalCoins(int purchaseSum, const QVector<int>& nominals, int& change, QMap<int, int>& usedCoins);


#endif // COINSOLVER_H

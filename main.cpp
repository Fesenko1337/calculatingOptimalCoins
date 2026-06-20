#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QVector>
#include <QMap>

#include "coinSolver.h"
#include "fileHandler.h"
#include "xmlParser.h"


/**
 * @brief
 * Выводит все ошибки из множества в консоль.
 * @param errors - множество ошибок для вывода.
 */
void printErrors(const QSet<Error>& errors)
{
    for (const Error& err : errors)
    {
        qCritical().noquote() << err.generateErrorMessage();
    }
}




/**
 * @brief
 * Главная функция программы.
 * Выполняет чтение входных данных, валидацию, расчёт оптимального количества монет и сдачи, запись результата.
 * @param argc Количество аргументов командной строки.
 * @param argv Массив аргументов: argv[1] - путь к входному файлу, argv[2] - путь к выходному файлу.
 * @return 0 при успешном завершении, 1 при ошибке.
 */
int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    // Проверить количество аргументов командной строки
    if (argc < 3)
    {
        // Если аргументов меньше трёх – вывести сообщение об ошибке в консоль и завершить работу программы
        qCritical() << "Ошибка: недостаточно аргументов командной строки.";
        qCritical() << "Использование: CoinCalculator <input_file> <output_file>";
        return 1;
    }

    QString inputFilePath = QString::fromLocal8Bit(argv[1]);
    QString outputFilePath = QString::fromLocal8Bit(argv[2]);

    // Вызвать readInputFile для чтения содержимого входного файла
    QString fileContent;
    QSet<Error> errors;
    if (!readInputFile(inputFilePath, fileContent, errors))
    {
        // Вывести сообщение об ошибке в консоль и завершить работу программы
        printErrors(errors);
        return 1;
    }

    // Вызвать parseAndValidateData для проверки и разбора данных
    int purchaseSum = 0;
    QVector<int> nominals;
    if (!parseAndValidateData(fileContent, purchaseSum, nominals, errors))
    {
        // Вывести сообщение об ошибке в консоль и завершить работу программы
        printErrors(errors);
        return 1;
    }

    // Рассчитать оптимальное количество монет каждого номинала для оплаты данной покупки и сдачу, полученную после покупки
    int change = 0;
    QMap<int, int> usedCoins;
    calculateOptimalCoins(purchaseSum, nominals, change, usedCoins);

    // Вызвать writeResultToFile для записи результата в выходной файл
    if (!writeResultToFile(outputFilePath, change, usedCoins, errors))
    {
        // Вывести сообщение об ошибке в консоль и завершить работу программы
        printErrors(errors);
        return 1;
    }

    qDebug() << "Расчёт успешно завершён. Результат записан в файл:" << outputFilePath;
    return 0;
}



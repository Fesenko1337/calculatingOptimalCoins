#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include "error.h"
#include <QString>
#include <QMap>
#include <QSet>


/**
 * @brief
 * Проверяет существование файла, открывает его и считывает содержимое.
 * @param filePath - путь к входному файлу.
 * @param fileContent - строка с содержимым файла.
 * @param errors - множество ошибок (если обнаружены).
 * @return true - файл успешно прочитан, false - произошла ошибка.
 */
bool readInputFile(const QString& filePath, QString& fileContent, QSet<Error>& errors);


/**
 * @brief
 * Формирует и записывает результат расчёта (величину сдачи и состав монет) в выходной XML-файл.
 * @param filePath - путь к выходному файлу.
 * @param change - величина сдачи.
 * @param usedCoins - словарь, где ключ - номинал монеты, значение - количество использованных монет данного номинала.
 * @param errors - множество ошибок (если обнаружены).
 * @return true - файл успешно записан, false - произошла ошибка записи.
 */
bool writeResultToFile(const QString& filePath, int change, const QMap<int, int>& usedCoins, QSet<Error>& errors);


#endif // FILEHANDLER_H

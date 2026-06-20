#ifndef XMLPARSER_H
#define XMLPARSER_H
#include <QString>
#include "error.h"

/**
 * @brief
 * Выполняет разбор содержимого XML-файла и проверку корректности входных данных.
 * @param fileContent - содержимое входного XML-файла в виде строки.
 * @param purchaseSum - сумма покупки (выходной параметр, заполняется при успешной валидации).
 * @param nominals - список номиналов монет (выходной параметр, заполняется при успешной валидации).
 * @param errors - множество ошибок (если обнаружены).
 * @return true - данные успешно проверены и разобраны, false - обнаружены ошибки валидации.
 */
bool parseAndValidateData(const QString& fileContent, int& purchaseSum, QVector<int>& nominals, QSet<Error>& errors);


#endif // XMLPARSER_H

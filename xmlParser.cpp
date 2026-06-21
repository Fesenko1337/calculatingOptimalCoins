#include "xmlParser.h"
#include <QXmlStreamReader>
#include <QSet>
#include "error.h"

// Имена XML-тегов, используемых в программе
const QString TAG_CONFIG = "config";
const QString TAG_SUM = "sum";
const QString TAG_NOMINALS = "nominals";
const QString TAG_VALUE = "value";

bool parseAndValidateData(const QString& fileContent, int& purchaseSum, QVector<int>& nominals, QSet<Error>& errors)
{
    purchaseSum = 0;
    nominals.clear();
    errors.clear();
    // Проверить, что входная строка не пустая
    if (fileContent.trimmed().isEmpty())
    {
        errors.insert(Error(inputFileEmpty));
        return false;
    }
    // Создать XML-парсер на основе содержимого файла
    QXmlStreamReader xml(fileContent);
    // Флаги для отслеживания структуры XML
    bool configFound = false; // Найден ли тег <config>
    bool sumFound = false; // Найден ли тег <sum>
    bool nominalsFound = false; // Найден ли тег <nominals>
    bool insideNominals = false; // Находимся ли мы внутри тега <nominals>
    QSet<int> uniqueNominals; // Множество для проверки уникальности номиналов
    int totalValueTagsCount = 0; // Общее количество тегов <value>
    int lastValueLineNumber = 0; // Номер строки последнего тега <value>

    // Проверить корректность синтаксиса XML и извлечь данные
    while (!xml.atEnd())
    {
        xml.readNext();

        // Если синтаксис некорректен
        if (xml.hasError())
        {
            // Записать ошибку invalidXmlSyntax
            errors.insert(Error(invalidXmlSyntax, xml.errorString(), xml.lineNumber()));
            return false;
        }

        // Обработка открывающих тегов
        if (xml.isStartElement())
        {
            QString tagName = xml.name().toString().trimmed().toLower();

            // Проверить структуру XML и наличие обязательных тегов
            if (tagName == TAG_CONFIG)
            {
                configFound = true; // Обнаружен корневой тег <config>
            }
            else if (tagName == TAG_SUM)
            {
                // Проверка расположения тега sum (должен быть внутри config)
                if (!configFound)
                {
                    errors.insert(Error(incorrectTagLocation, tagName, xml.lineNumber()));
                }
                else
                {
                    sumFound = true; // Обнаружен тег <sum>

                    // Извлечь и проверить сумму покупки
                    QString text = xml.readElementText().trimmed();
                    bool isNumber = false;
                    text.toDouble(&isNumber);
                    if (!isNumber) // Если сумма покупки не число
                    {
                        // Записать ошибку purchaseSumNotNumber
                        errors.insert(Error(purchaseSumNotNumber, text, xml.lineNumber()));
                    }
                    else
                    {
                        bool isInteger = false;
                        int value = text.toInt(&isInteger);
                        if (!isInteger) // Если сумма покупки не целая
                        {
                            // Записать ошибку purchaseSumNotInteger
                            errors.insert(Error(purchaseSumNotInteger, text, xml.lineNumber()));
                        }
                        else if (value < 1) // Сумма покупки меньше 1
                        {
                            // Записать ошибку purchaseSumLessMin
                            errors.insert(Error(purchaseSumLessMin, text, xml.lineNumber()));
                        }
                        else if (value > 1000) // Сумма покупки больше 1000
                        {
                            // Записать ошибку purchaseSumExceedsMax
                            errors.insert(Error(purchaseSumExceedsMax, text, xml.lineNumber()));
                        }
                        else
                        {
                            purchaseSum = value;
                        }
                    }
                }
            }
            else if (tagName == TAG_NOMINALS)
            {
                // Проверка расположения тега nominals (должен быть внутри config)
                if (!configFound)
                {
                    errors.insert(Error(incorrectTagLocation, tagName, xml.lineNumber()));
                }
                else
                {
                    // Обнаружен тег <nominals>, начинаем обработку номиналов
                    nominalsFound = true;
                    insideNominals = true;
                }
            }
            else if (tagName == TAG_VALUE)
            {
                totalValueTagsCount++;
                lastValueLineNumber = xml.lineNumber();
                // Проверка расположения тега value (должен быть внутри nominals)
                if (!insideNominals)
                {
                    errors.insert(Error(incorrectTagLocation, tagName, xml.lineNumber()));
                }

                // Извлечь и проверить список номиналов монет
                QString text = xml.readElementText().trimmed();
                bool isNumber = false;
                text.toDouble(&isNumber);
                if (!isNumber) // Если номинал не число
                {
                    // Записать ошибку nominalNotNumber
                    errors.insert(Error(nominalNotNumber, text, xml.lineNumber()));
                }
                else
                {
                    bool isInteger = false;
                    int value = text.toInt(&isInteger);
                    if (!isInteger) // Если номинал не целый
                    {
                        // Записать ошибку nominalNotInteger
                        errors.insert(Error(nominalNotInteger, text, xml.lineNumber()));
                    }
                    else if (value < 1) // Если номинал меньше 1
                    {
                        // Записать ошибку nominalLessMin
                        errors.insert(Error(nominalLessMin, text, xml.lineNumber()));
                    }
                    else if (value > 100) // Если номинал больше 100
                    {
                        // Записать ошибку nominalExceedsMax
                        errors.insert(Error(nominalExceedsMax, text, xml.lineNumber()));
                    }
                    else if (uniqueNominals.contains(value)) // Если номинал повторяется
                    {
                        // Записать ошибку nominalDuplicate
                        errors.insert(Error(nominalDuplicate, text, xml.lineNumber()));
                    }
                    else
                    {
                        uniqueNominals.insert(value);
                        nominals.append(value); // Номинал валиден, добавить его в список
                    }
                }
            }
            else {
                // Если обнаружен неизвестный тег
                // Записать ошибку unknownTag
                errors.insert(Error(unknownTag, tagName, xml.lineNumber()));
            }
        }
        // Обработка закрывающих тегов
        else if (xml.isEndElement())
        {
            QString tagName = xml.name().toString().toLower();
            if (tagName == TAG_NOMINALS)
            {
                insideNominals = false;
            }
        }
    }
    // Проверка наличия обязательных тегов
    if (!configFound)
    {
        errors.insert(Error(missingTag, "config", xml.lineNumber()));
    }
    if (!sumFound)
    {
        errors.insert(Error(missingTag, "sum", xml.lineNumber()));
    }
    if (!nominalsFound)
    {
        errors.insert(Error(missingTag, "nominals", xml.lineNumber()));
    }
    // Проверить количество номиналов
    // Если количество номиналов превышает максимально допустимое
    if (totalValueTagsCount > 100)
    {
        // Записать ошибку nominalsCountExceedsMax
        // Номер строки = строка последнего <value> тега
        errors.insert(Error(nominalsCountExceedsMax, QString::number(totalValueTagsCount), lastValueLineNumber));
    }

    // Если errors не пуст – вернуть false
    if (!errors.isEmpty())
    {
        return false;
    }
    // Иначе вернуть true
    return true;
}

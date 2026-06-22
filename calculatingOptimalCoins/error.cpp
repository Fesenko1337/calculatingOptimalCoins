#include "error.h"

/**
 * @brief
 * Конструктор по умолчанию.
 */
Error::Error()
    : type(inputFileNotAccessible), lineNumber(-1)
{
}


/**
 * @brief
 * Параметризованный конструктор класса Error.
 * @param errorType - тип ошибки из перечисления errorType.
 * @param invalidValue - некорректное значение, вызвавшее ошибку.
 * @param lineNumber - номер строки, где обнаружена ошибка.
 * @param fileName - имя файла, в котором обнаружена ошибка.
 */
Error::Error(errorType errorType, const QString& invalidValue, int lineNumber, const QString& fileName)
    : type(errorType), invalidValue(invalidValue), fileName(fileName), lineNumber(lineNumber)
{
}


bool Error::operator==(const Error& other) const
{
    return type == other.type
        && invalidValue == other.invalidValue
        && lineNumber == other.lineNumber
        && fileName == other.fileName;
}


QString Error::generateErrorMessage() const
{
    QString message;

    switch (type)
    {
        // Ошибки входного файла
        case inputFileNotAccessible:
            message = QString("Ошибка. Неверно указан файл с входными данными. "
                              "Возможно, файл не существует.\n"
                              "Файл: \"%1\".").arg(fileName);
            break;

        case inputFileEmpty:
            message = QString("Ошибка. Файл с входными данными не заполнен. "
                              "Заполните файл.\n"
                              "Файл: \"%1\".").arg(fileName);
            break;

        // Ошибки выходного файла
        case outputFileNotAccessible:
            message = QString("Ошибка. Неверно указан файл для выходных данных. "
                              "Возможно, указанного расположения не существует "
                              "или нет прав на запись.\n"
                              "Файл: \"%1\".").arg(fileName);
            break;

        // Ошибки суммы покупки
        case purchaseSumExceedsMax:
            message = QString("Ошибка. Сумма покупки не может быть больше 1000.\n"
                              "Найдена некорректная сумма покупки: \"%1\".").arg(invalidValue);
            break;

        case purchaseSumLessMin:
            message = QString("Ошибка. Сумма покупки не может быть меньше 1.\n"
                              "Найдена некорректная сумма покупки: \"%1\".").arg(invalidValue);
            break;

        case purchaseSumNotInteger:
            message = QString("Ошибка. Сумма покупки не может быть не целым числом.\n"
                              "Найдена некорректная сумма покупки: \"%1\".").arg(invalidValue);
            break;

        case purchaseSumNotNumber:
            message = QString("Ошибка. Сумма покупки не является числом.\n"
                              "Найдена некорректная сумма покупки: \"%1\".").arg(invalidValue);
            break;

        // Ошибки номиналов монет
        case nominalNotNumber:
            message = QString("Ошибка. Номиналы монет не являются числами.\n"
                              "Найден некорректный номинал монеты: \"%1\".").arg(invalidValue);
            break;

        case nominalNotInteger:
            message = QString("Ошибка. Номиналы монет не могут быть не целыми числами.\n"
                              "Найден некорректный номинал монеты: \"%1\".").arg(invalidValue);
            break;

        case nominalLessMin:
            message = QString("Ошибка. Номинал монеты не может быть меньше 1.\n"
                              "Найден некорректный номинал монеты: \"%1\".").arg(invalidValue);
            break;

        case nominalDuplicate:
            message = QString("Ошибка. Номиналы монет не могут повторяться.\n"
                              "Номинал монеты: \"%1\" повторяется.").arg(invalidValue);
            break;

        case nominalExceedsMax:
            message = QString("Ошибка. Номинал монеты превышает максимально допустимое значение 100.\n"
                              "Найден некорректный номинал монеты: \"%1\".").arg(invalidValue);
            break;

        case nominalsCountExceedsMax:
            message = QString("Ошибка. Количество номиналов монет превышает максимально допустимое 100 чисел.");
            break;

        // Ошибки структуры XML
        case unknownTag:
            message = QString("Ошибка. Неопознанный тег: \"%1\".").arg(invalidValue);
            break;

        case missingTag:
            message = QString("Ошибка. Отсутствует тег: \"%1\".").arg(invalidValue);
            break;

        case incorrectTagLocation:
            if (invalidValue == TAG_VALUE)
            {
                message = QString("Ошибка. Некорректное местоположение тега: \"%1\". "
                                  "Тег \"%1\" должен находиться внутри тега \"nominals\".")
                                  .arg(invalidValue);
            }
            else if (invalidValue == TAG_SUM || invalidValue == TAG_NOMINALS)
            {
                message = QString("Ошибка. Некорректное местоположение тега: \"%1\". "
                                  "Тег \"%1\" должен находиться внутри тега \"config\".")
                                  .arg(invalidValue);
            }
            else
            {
                message = QString("Ошибка. Некорректное местоположение тега: \"%1\".")
                                  .arg(invalidValue);
            }
            break;

        case invalidXmlSyntax:
            if (invalidValue.isEmpty())
            {
                message = QString("Ошибка. Некорректный синтаксис XML.");
            }
            else
            {
                message = QString("Ошибка. Некорректный синтаксис XML. %1").arg(invalidValue);
            }
            break;

        default:
            message = QString("Неизвестная ошибка.");
            break;
    }

    // Добавление номера строки, если он был передан
    if (lineNumber > 0)
    {
        message += QString("\nСтрока: %1").arg(lineNumber);
    }

    return message;
}

uint qHash(const Error& key, uint seed)
{
    uint h = seed;
    h ^= static_cast<uint>(key.type);
    h ^= static_cast<uint>(key.lineNumber);
    h ^= static_cast<uint>(key.invalidValue.length());
    h ^= static_cast<uint>(key.fileName.length());
    return h;
}

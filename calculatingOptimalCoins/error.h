#ifndef ERROR_H
#define ERROR_H

#include <QString>

// Имена XML-тегов, используемых в программе
static const QString TAG_CONFIG = "config";
static const QString TAG_SUM = "sum";
static const QString TAG_NOMINALS = "nominals";
static const QString TAG_VALUE = "value";

/**
 * @brief
 * Перечисление возможных типов ошибок в программе.
 */
enum errorType
{
    // Ошибки входного файла
    inputFileNotAccessible, // Входной файл недоступен
    inputFileEmpty, // Входной файл пустой

    // Ошибки выходного файла
    outputFileNotAccessible, // Выходной файл недоступен

    // Ошибки валидации данных
    purchaseSumExceedsMax, // Сумма покупки больше максимально возможной
    purchaseSumLessMin, // Сумма покупки меньше минимально возможной
    purchaseSumNotInteger, // Сумма покупки не является целым числом
    purchaseSumNotNumber, // Сумма покупки не является числом
    nominalNotNumber, // Номинал не является числом
    nominalNotInteger, // Номинал не является целым числом
    nominalLessMin, // Номинал меньше минимально возможного
    nominalDuplicate, // Номинал повторяется
    nominalExceedsMax, // Номинал больше максимально возможного
    nominalsCountExceedsMax, // Количество номиналов превышает максимально возможное
    unknownTag, // Неопознанный тег
    missingTag, // Отсутствует тег
    incorrectTagLocation, // Некорректное местоположение тега
    invalidXmlSyntax, // Некорректный синтаксис XML
    noError
};

/**
 * @brief
 * Класс, описывающий ошибку с дополнительными деталями
 * (тип, некорректное значение, имя файла, номер строки).
 */
class Error
{
public:
    errorType type; // Тип ошибки
    QString invalidValue; // Некорректное значение (если применимо)
    QString fileName; // Имя файла (если применимо)
    int lineNumber; // Номер строки в файле (если применимо)

    /**
     * @brief
     * Конструктор по умолчанию. Инициализирует ошибку типом inputFileNotAccessible.
     */
    Error();

    /**
     * @brief
     * Основной конструктор с параметрами.
     * @param errorType - тип ошибки.
     * @param invalidValue - некорректное значение (по умолчанию пустая строка).
     * @param lineNumber - номер строки (по умолчанию -1).
     * @param fileName - имя файла (по умолчанию пустая строка).
     */
    Error(errorType errorType, const QString& invalidValue = "",
          int lineNumber = -1, const QString& fileName = "");
    /**
     * @brief
     * Оператор сравнения двух объектов Error.
     * @param other - другой объект Error для сравнения.
     * @return true, если оба объекта идентичны (совпадают тип, некорректное значение, номер строки и имя файла), иначе false.
     */
    bool operator==(const Error& other) const;

    /**
     * @brief
     * Генерирует сообщение об ошибке.
     * @return строка с описанием ошибки.
     */
    QString generateErrorMessage() const;
};



/**
 * @brief
 * Хэш-функция для класса Error.
 * @param key - ошибка, для которой вычисляется хэш.
 * @param seed - начальное значение хэша (по умолчанию 0).
 * @return хэш-значение ошибки.
 */
 uint qHash(const Error& key, uint seed = 0);

#endif // ERROR_H

#include "fileHandler.h"
#include <QFile>
#include <QFileInfo>
#include <QTextStream>




bool readInputFile(const QString& filePath, QString& fileContent, QSet<Error>& errors)
{
    fileContent.clear();

    // Проверить существование файла по указанному пути
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile())
    {
        // Если файл отсутствует или не открылся
        // Записать ошибку inputFileNotAccessible в errors
        errors.insert(Error(inputFileNotAccessible, "", -1, filePath));
        // Вернуть false
        return false;
    }

    // Открыть файл для чтения
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // Если файл не удалось открыть
        // Записать ошибку inputFileNotFound в errors
        errors.insert(Error(inputFileNotAccessible, "", -1, filePath));
        // Вернуть false
        return false;
    }

    // Считать содержимое файла в fileContent
    QTextStream inStream(&file);
    inStream.setCodec("UTF-8");
    fileContent = inStream.readAll();

    // Закрыть файл
    file.close();
    return true;
}




bool writeResultToFile(const QString& filePath, int change, const QMap<int, int>& usedCoins, QSet<Error>& errors)
{
    QFile file(filePath);

    // Открыть выходной файл по указанному пути на запись
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // Если файл не удалось открыть
        // Записать соответствующую ошибку в errors
        errors.insert(Error(outputFileNotAccessible , "", -1, filePath));
        // Вернуть false
        return false;
    }

    QTextStream outStream(&file);
    outStream.setCodec("UTF-8");

    // Записать открывающий тег <config>
    outStream << "<config>\n";
    // Записать элемент <change> с величиной сдачи
    outStream << "  <change>" << change << "</change>\n";
    // Записать открывающий тег <nominals>
    outStream << "  <nominals>\n";

    // Для каждого номинала в usedCoins в порядке убывания номинала
    for (auto it = usedCoins.constEnd(); it != usedCoins.constBegin(); )
    {
        --it; // Переходим к предыдущему элементу (от конца к началу)
        int nominal = it.key();
        int count = it.value();

        if (count > 0)
        {
            // Записать элемент <nominal>
            outStream << "    <nominal>\n";
            // Внутри него записать элемент <value> с номиналом монеты
            outStream << "      <value>" << nominal << "</value>\n";
            // Внутри него записать элемент <count> с количеством монет
            outStream << "      <count>" << count << "</count>\n";
            // Закрыть элемент <nominal>
            outStream << "    </nominal>\n";
         }
    }

    // Закрыть тег <nominals>
    outStream << "  </nominals>\n";
    // Закрыть тег <config>
    outStream << "</config>\n";

    // Завершить запись в файл
    outStream.flush();
    file.close();
    return true;
}

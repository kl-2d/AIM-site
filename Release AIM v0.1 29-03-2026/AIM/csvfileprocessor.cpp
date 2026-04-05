#include "csvfileprocessor.h"

#include <QFile>

CsvFileProcessor::CsvFileProcessor(
    const FileColumnsModel* fileColumnsModel,
    const DataColumnsModel* dataColumnsModel,
    DataModel* dataModel)
{
    this->fileColumnsModel = fileColumnsModel;
    this->dataColumnsModel = dataColumnsModel;
    this->dataModel = dataModel;
}

void CsvFileProcessor::importFromFile(const QString& fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    // dbController->clear();
    QByteArray row = file.readLine();
    while (!row.isEmpty())
    {
        QString rowString = QString::fromLocal8Bit(row);
        while (rowString.at(rowString.size() - 1) == '\n' || rowString.at(rowString.size() - 1) == '\r')
        {
            rowString.chop(1);
        }
        auto fileRow = rowString.split(",", Qt::KeepEmptyParts);
        auto dataRow = dataColumnsModel->convertFileRow(fileRow);
        dataModel->insertRow(dataRow);
        row = file.readLine();
    }
    file.close();
}

void CsvFileProcessor::exportToFile(const QString& fileName, bool useDataFormat)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QList<QString> headerData;
    for (int column = 0; column < dataColumnsModel->count(); column++)
    {
        headerData.append(dataColumnsModel->at(column).name);
    }
    file.write(headerData.join(",").append("\n").toLocal8Bit());
    for (int row = 0; row < dataModel->rowCount(); row++)
    {
        QList<QString> rowData;
        for (int column = 0; column < dataColumnsModel->count(); column++)
        {
            rowData.append(dataModel->data(row, column));
        }
        auto fileRow =
            useDataFormat ? rowData : dataColumnsModel->convertDataRow(rowData);
        file.write(fileRow.join(",").append("\n").toLocal8Bit());
    }
    file.close();
}

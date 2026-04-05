#include "datamodel.h"

DataModel::DataModel(const DataColumnsModel *columns,
                     DatabaseController *databaseController,
                     QObject *parent)
    : QObject(parent)
{
    columnsModel = columns;
    dbController = databaseController;
    rowIds = dbController->getIds();

    QObject::connect(
        columnsModel,
        &DataColumnsModel::modelChanged,
        this,
        [this](bool fullChange) {
            if (!fullChange)
            {
                emit changed();
            }
        });

    QObject::connect(
        dbController,
        &DatabaseController::tableRecreated,
        this,
        [this](){
            rowIds.clear();
            emit changed();
        });
}

int DataModel::rowCount() const
{
    return rowIds.size();
}

QVector<QVector<QString>> DataModel::allData() const
{
    return dbController->allData();
}

QString DataModel::data(int row, int column) const
{
    return dbController->data(rowIds.at(row), column);
}

void DataModel::setData(int row, int column, const QString &data)
{
    dbController->setData(rowIds.at(row), column, data);
    emit changed();
}

void DataModel::insertRow(const QList<QString>& data)
{
    int newRowId = dbController->insertRow(data);
    if (newRowId != -1)
    {
        rowIds.append(newRowId);
        emit changed();
    }
}

void DataModel::deleteRow(int row)
{
    dbController->deleteRow(rowIds.at(row));
    rowIds.removeAt(row);
    emit changed();
}

#ifndef DATAMODEL_H
#define DATAMODEL_H

#include "databasecontroller.h"
#include "datacolumnsmodel.h"

class DataModel : public QObject
{
    Q_OBJECT

public:
    DataModel(
        const DataColumnsModel* columns,
        DatabaseController *databaseController,
        QObject *parent = nullptr);

    int rowCount() const;
    QVector<QVector<QString>> allData() const;
    QString data(int row, int column) const;
    void setData(int row, int column, const QString& data);
    void insertRow(const QList<QString> &data);
    void deleteRow(int row);

signals:
    void changed();

private:
    const DataColumnsModel* columnsModel;
    DatabaseController* dbController;

    QVector<int> rowIds;
};

#endif // DATAMODEL_H

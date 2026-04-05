#ifndef DATABASECONTROLLER_H
#define DATABASECONTROLLER_H

#include "datacolumnsmodel.h"

#include <QObject>
#include <QSqlDatabase>

class DatabaseController : public QObject
{
    Q_OBJECT

public:
    DatabaseController(
        const DataColumnsModel* columnsModel, QObject* parent = nullptr);

    int recordsCount() const;
    QVector<QVector<QString>> allData() const;
    QVector<int> getIds() const;
    QString data(int id, int column) const;
    void clear();
    void setData(int id, int column, const QString& data);
    int insertRow(const QList<QString>& data);
    void deleteRow(int id);

signals:
    void tableRecreated();

private:
    QString createTableQuery() const;

private slots:
    void recreateTable();

private:
    const DataColumnsModel* model;

    QSqlDatabase database;
};

#endif // DATABASECONTROLLER_H

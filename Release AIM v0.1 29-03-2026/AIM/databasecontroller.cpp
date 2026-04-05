#include "databasecontroller.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <iostream>

DatabaseController::DatabaseController(
    const DataColumnsModel* columnsModel, QObject* parent) :
  QObject(parent)
{
    model = columnsModel;
    QObject::connect(
        model,
        &DataColumnsModel::modelChanged,
        this,
        [this](bool fullChange) {
            if (fullChange)
            {
                recreateTable();
            }
        });

    auto* dbPath = std::getenv("DB_PATH");
    if (dbPath == nullptr)
    {
        dbPath = "db.sqlite";
        qDebug() << "Missing DB_PATH variable. Using default";
    }

    // TODO: Change to PostgreSQL
    database = QSqlDatabase::addDatabase("QSQLITE", "db");
    database.setDatabaseName(QString::fromUtf8(dbPath));
    database.open();

    QSqlQuery query(database);
    query.prepare(createTableQuery());
    query.exec();
}

int DatabaseController::recordsCount() const
{
    QSqlQuery query(database);
    query.prepare("SELECT COUNT(*) FROM data;");
    query.exec();
    query.next();
    return query.value(0).value<int>();
}

QVector<QVector<QString>> DatabaseController::allData() const
{
    QString columnLabels = model->labelsList().join(",");
    QSqlQuery query(database);
    query.prepare(QString("SELECT %1 FROM data;").arg(columnLabels));
    query.exec();

    QVector<QVector<QString>> result;
    while (query.next())
    {
        QVector<QString> row;
        for (int i = 0; i < model->count(); i++)
        {
            row.append(query.value(i).value<QString>());
        }
        result.append(row);
    }
    return result;
}

QVector<int> DatabaseController::getIds() const
{
    QVector<int> ids;
    QSqlQuery query(database);
    query.prepare("SELECT id FROM data;");
    query.exec();
    while(query.next())
    {
        ids.append(query.value(0).value<int>());
    }
    return ids;
}

QString DatabaseController::data(int id, int column) const
{
    QSqlQuery query(database);
    query.prepare(QString("SELECT %1 FROM data WHERE id=%2;")
                      .arg(model->at(column).label, QString::number(id)));
    query.exec();
    query.next();
    return query.value(0).value<QString>();
}

void DatabaseController::clear()
{
    QSqlQuery query(database);
    query.prepare(QString("DELETE FROM data;"));
    query.exec();

    query.prepare(QString("DELETE FROM sqlite_sequence WHERE name='data';"));
    query.exec();
}

void DatabaseController::setData(int id, int column, const QString &data)
{
    QSqlQuery query(database);
    query.prepare(QString("UPDATE data SET %1=\"%2\" WHERE id=%3;")
                      .arg(model->at(column).label, data, QString::number(id)));
    qDebug() << QString("UPDATE data SET %1=\"%2\" WHERE id=%3;")
                    .arg(model->at(column).label, data, QString::number(id));
    query.exec();
}

int DatabaseController::insertRow(const QList<QString> &data)
{
    QString columnLabels = model->labelsList().join(",");
    QList<QString> valuesList = data;
    for (int i = 0; i < valuesList.size(); i++)
    {
        /*if (model->at(i).type == ColumnType::DATETIME
            || model->at(i).type == ColumnType::STRING)*/
        {
            valuesList[i].prepend("'");
            valuesList[i].append("'");
        }
    }
    QString values = valuesList.join(",");

    QSqlQuery query(database);
    query.prepare(
        QString("INSERT INTO data (%1) VALUES (%2);").arg(columnLabels, values));
    qDebug() << QString("INSERT INTO data (%1) VALUES (%2);")
                    .arg(columnLabels, values);
    if(!query.exec())
    {
        qDebug() << query.lastError().text();
        return -1;
    }

    query.prepare("SELECT MAX(id) FROM data;");
    query.exec();
    query.next();
    return query.value(0).value<int>();
}

void DatabaseController::deleteRow(int id)
{
    QSqlQuery query(database);
    query.prepare(QString("DELETE FROM data WHERE id=%1;").arg(QString::number(id)));
    query.exec();
    query.next();
}

QString DatabaseController::createTableQuery() const
{
    QString query = "CREATE TABLE IF NOT EXISTS \"data\" (";
    query.append("\"id\" INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE");
    for (int i = 0; i < model->count(); i++)
    {
        auto columnParams = model->at(i);
        QString type;
        // TODO: Update types after switching to PostgreSQL
        switch (columnParams.type)
        {
            case ColumnType::INT: type = "INTEGER"; break;
            case ColumnType::DOUBLE: type = "REAL"; break;
            default: type = "TEXT"; break;
        }
        query.append(QString(", \"%1\" %2").arg(columnParams.label, type));
    }
    query.append(")");
    return query;
}

void DatabaseController::recreateTable()
{
    QSqlQuery query(database);
    query.prepare("DROP TABLE \"data\"");
    query.exec();

    query.prepare(createTableQuery());
    query.exec();

    emit tableRecreated();
}

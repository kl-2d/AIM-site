#ifndef DATACOLUMNSMODEL_H
#define DATACOLUMNSMODEL_H

#include "filecolumnsmodel.h"

struct DataColumnParams
{
    QString label;
    QString name;
    ColumnType::Value type;
    std::optional<ColumnType::Value> elementsType;
    QList<QPair<double, double>> intervals;
    QList<StringValueParams> values;
    QList<int> fileColumnModelIndexes;

    QList<QString> valuesStringList() const;
    QString valueByCode(const QString& code) const;
    QString codeByValue(const QString& text) const;
    QString intervalsString() const;
};

class DataColumnsModel : public QObject
{
    Q_OBJECT

public:
    DataColumnsModel(
        const FileColumnsModel* columnsModel, QObject* parent = nullptr);

    int count() const;
    DataColumnParams at(int index) const;
    QList<QString> labelsList() const;
    QList<QString> convertFileRow(const QList<QString>& fileRow) const;
    QList<QString> convertDataRow(const QList<QString>& dataRow) const;

signals:
    void modelChanged(bool fullChange);

private slots:
    void updateModel(bool fullChange);

private:
    const FileColumnsModel* columnsModel;
    QVector<DataColumnParams> params;
};

#endif // DATACOLUMNSMODEL_H

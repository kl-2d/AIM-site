#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include "datacolumnsmodel.h"
#include "datamodel.h"

#include <QAbstractTableModel>

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableModel(
        const DataColumnsModel* columns,
        DataModel* data,
        QObject* parent = nullptr);

    int rowCount(const QModelIndex& index = QModelIndex()) const override;
    int columnCount(const QModelIndex& index = QModelIndex()) const override;

    QVariant headerData(
        int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(
        const QModelIndex& index,
        const QVariant& value,
        int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    int rowsWithErrors() const;

    bool isValid(int row) const;

    void deleteSelectedData(const QModelIndexList& selected);

private:
    void appendRow();
    void validate();

private:
    struct ValidationResult
    {
        bool isValid;
        QString errorDescription;
    };
    QVector<QVector<ValidationResult>> validationResults;

    const DataColumnsModel* columnsModel;
    DataModel* dataModel;
};

#endif // TABLEMODEL_H

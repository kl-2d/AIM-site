#include "tablemodel.h"
#include "utils.h"

#include <QColor>
#include <QCoreApplication>

TableModel::TableModel(
    const DataColumnsModel *columns,
    DataModel *data,
    QObject *parent) :
  QAbstractTableModel(parent)
{
    columnsModel = columns;
    dataModel = data;

    connect(dataModel, &DataModel::changed, this, [this]() {
        validate();
        beginResetModel();
        endResetModel();
    });

    validate();
}

int TableModel::rowCount(const QModelIndex& index) const
{
    Q_UNUSED(index)

    return dataModel->rowCount() + 1;
}

int TableModel::columnCount(const QModelIndex& index) const
{
    Q_UNUSED(index)

    return columnsModel->count();
}

QVariant TableModel::headerData(
    int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return {};
    }
    if (orientation == Qt::Horizontal)
    {
        return columnsModel->at(section).name;
    }
    else
    {
        return QString::number(section + 1);
    }
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.row() == rowCount() - 1)
        {
            return {};
        }
        return dataModel->data(index.row(), index.column());
    }
    if (role == Qt::BackgroundRole && index.row() < validationResults.size()
        && index.column() < validationResults.at(index.row()).size())
    {
        return validationResults.at(index.row()).at(index.column()).isValid ?
                   QColor(255, 255, 255) :
                   QColor(255, 0, 0);
    }
    if (role == Qt::ToolTipRole && index.row() < validationResults.size()
        && index.column() < validationResults.at(index.row()).size())
    {
        return validationResults.at(index.row())
            .at(index.column())
            .errorDescription;
    }
    return {};
}

bool TableModel::setData(
    const QModelIndex& index, const QVariant& value, int role)
{
    if (role == Qt::EditRole)
    {
        if (index.row() == dataModel->rowCount())
        {
            appendRow();
        }

        dataModel->setData(index.row(), index.column(), value.toString());
        return true;
    }
    return false;
}

Qt::ItemFlags TableModel::flags(const QModelIndex& index) const
{
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

int TableModel::rowsWithErrors() const
{
    int rowsWithErrorsCount = 0;
    for (const auto& rowValidationResult: validationResults)
    {
        if (!std::all_of(
                rowValidationResult.begin(),
                rowValidationResult.end(),
                [](const auto& validationResult) {
                    return validationResult.isValid == true;
                }))
        {
            rowsWithErrorsCount++;
        }
    }
    return rowsWithErrorsCount;
}

bool TableModel::isValid(int row) const
{
    if (row >= validationResults.size())
    {
        return true;
    }
    auto rowValidationResult = validationResults.at(row);
    return std::all_of(
        rowValidationResult.begin(),
        rowValidationResult.end(),
        [](const auto& validationResult) {
            return validationResult.isValid == true;
        });
}

void TableModel::deleteSelectedData(const QModelIndexList& selected)
{
    QModelIndexList selectedSorted = selected;
    std::sort(
        selectedSorted.begin(),
        selectedSorted.end(),
        [](const QModelIndex& index1, const QModelIndex& index2)
        {
            return index2.row() < index1.row();
        });
    for (int i = 0; i < selectedSorted.size(); i++)
    {
        int row = selectedSorted.at(i).row();
        if (row == rowCount() - 1)
        {
            continue;
        }
        if (std::count_if(
                selectedSorted.begin(),
                selectedSorted.end(),
                [row](const QModelIndex& index){ return index.row() == row; })
            == columnsModel->count())
        {
            dataModel->deleteRow(row);
            i += columnsModel->count() - 1;
        }
        else
        {
            int column = selectedSorted.at(i).column();
            dataModel->setData(row, column, QString());
        }
    }
}

void TableModel::appendRow()
{
    QList<QString> newRow;
    for (int i = 0; i < columnsModel->count(); i++)
    {
        newRow.append(QString());
    }
    dataModel->insertRow(newRow);
}

void TableModel::validate()
{
    validationResults.clear();
    auto allData = dataModel->allData();
    for (int row = 0; row < allData.size(); row++)
    {
        QVector<ValidationResult> rowValidationResults;
        auto rowData = allData.at(row);
        for (int column = 0; column < rowData.size(); column++)
        {
            QString cellData = rowData.at(column);
            DataColumnParams cellParams = columnsModel->at(column);
            bool isValid = true;
            QString errorDescription;
            if (cellParams.type == ColumnType::INT)
            {
                bool ok = true;
                cellData.toInt(&ok);
                if (cellData.isEmpty() || !ok)
                {
                    isValid = false;
                    errorDescription = QCoreApplication::translate(
                        "Validation errors", "Cannot convert value to integer");
                }
            }
            if (cellParams.type == ColumnType::DOUBLE)
            {
                bool ok = true;
                cellData.toDouble(&ok);
                if (!ok)
                {
                    isValid = false;
                    errorDescription = QCoreApplication::translate(
                        "Validation errors", "Cannot convert value to double");
                }
            }
            if (cellParams.type == ColumnType::BOOL)
            {
                if (!Utils::strToBool(cellData))
                {
                    isValid = false;
                    errorDescription = QCoreApplication::translate(
                        "Validation errors", "Cannot convert value to bool. Allowed values: 'Yes', '1', '+' or 'No', '0', '-'");
                }
            }
            if (cellParams.type == ColumnType::DATETIME)
            {
                if (!QDate::fromString(cellData, "dd.MM.yyyy").isValid())
                {
                    isValid = false;
                    errorDescription = QCoreApplication::translate(
                        "Validation errors", "Cannot convert value to date. Allowed date format: dd.MM.yyyy");
                }
            }
            if (!cellParams.intervals.isEmpty())
            {
                double value = cellData.toDouble();
                bool found = false;
                for (const auto& interval: qAsConst(cellParams.intervals))
                {
                    if (value >= interval.first && value <= interval.second)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    isValid = false;
                    errorDescription =
                        QCoreApplication::translate(
                            "Validation errors",
                            "Unacceptable value. Allowed intervals: %1")
                            .arg(cellParams.intervalsString());
                }
            }
            if (!cellParams.values.isEmpty())
            {
                bool found = false;
                auto valuesList = cellParams.valuesStringList();
                for (const auto& value: valuesList)
                {
                    if (cellData == value)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    isValid = false;
                    errorDescription =
                        QCoreApplication::translate(
                            "Validation errors",
                            "Unacceptable value. Allowed values: %1")
                            .arg(valuesList.join(", "));
                }
            }
            rowValidationResults.append(
                ValidationResult{isValid, errorDescription});
        }
        validationResults.append(rowValidationResults);
    }
}

#include "datacolumnsmodel.h"

DataColumnsModel::DataColumnsModel(
    const FileColumnsModel* columnsModel, QObject* parent) :
  QObject(parent)
{
    this->columnsModel = columnsModel;
    QObject::connect(
        columnsModel,
        &FileColumnsModel::modelChanged,
        this,
        &DataColumnsModel::updateModel);
    updateModel(true);
}

int DataColumnsModel::count() const
{
    return params.size();
}

DataColumnParams DataColumnsModel::at(int index) const
{
    return params.at(index);
}

QList<QString> DataColumnsModel::labelsList() const
{
    QList<QString> result;
    for (const auto& columnParams: params)
    {
        result.append(columnParams.label);
    }
    return result;
}

QList<QString> DataColumnsModel::convertFileRow(
    const QList<QString>& fileRow) const
{
    QList<QString> result;
    for (int i = 0; i < params.count(); i++)
    {
        const auto& columnParams = params.at(i);
        auto fileRowIndexes = columnParams.fileColumnModelIndexes;
        if (fileRowIndexes.size() == 1)
        {
            if (columnParams.type == ColumnType::STRING
                && !columnParams.values.isEmpty()
                && !columnParams.values.first().code.isEmpty())
            {
                result.append(columnParams.valueByCode(
                    fileRow.at(fileRowIndexes.first())));
            }
            else
            {
                result.append(fileRow.at(fileRowIndexes.first()));
            }
        }
        else
        {
            for (auto fileRowIndex: fileRowIndexes)
            {
                // TODO: Check for group
                if (!fileRow.at(fileRowIndex).isEmpty())
                {
                    result.append(columnsModel->at(fileRowIndex).name);
                    break;
                }
            }
        }
    }
    return result;
}

QList<QString> DataColumnsModel::convertDataRow(
    const QList<QString>& dataRow) const
{
    QList<QString> result;
    for (int i = 0; i < columnsModel->count(); i++)
    {
        auto it = std::find_if(
            params.begin(), params.end(), [i](const auto& columnParams) {
                return std::find(
                           columnParams.fileColumnModelIndexes.begin(),
                           columnParams.fileColumnModelIndexes.end(),
                           i)
                       != columnParams.fileColumnModelIndexes.end();
            });
        auto index = std::distance(params.begin(), it);
        const auto& fileColumnParams = columnsModel->at(i);
        if (!fileColumnParams.isInGroup)
        {
            if (fileColumnParams.type == ColumnType::STRING
                && !fileColumnParams.values.isEmpty()
                && !fileColumnParams.values.first().code.isEmpty())
            {
                result.append(fileColumnParams.codeByValue(dataRow.at(index)));
            }
            else
            {
                result.append(dataRow.at(index));
            }
        }
        else
        {
            if (columnsModel->at(i).name == dataRow.at(index))
            {
                result.append("1");
            }
            else
            {
                result.append("");
            }
        }
    }
    return result;
}

void DataColumnsModel::updateModel(bool fullChange)
{
    params.clear();
    for (int i = 0; i < columnsModel->count(); i++)
    {
        auto fileColumnParams = columnsModel->at(i);
        if (fileColumnParams.isInGroup)
        {
            QString groupLabel = *fileColumnParams.groupLabel;
            auto it = std::find_if(
                params.begin(),
                params.end(),
                [&groupLabel](const auto& columnParams) {
                    return columnParams.label == groupLabel;
                });
            if (it == params.end())
            {
                params.append(DataColumnParams{
                    .label = groupLabel,
                    // TODO: Add group name
                    .name = "",
                    .type = ColumnType::STRING,
                    .elementsType = std::nullopt,
                    .intervals = {},
                    .values = columnsModel->valuesForGroup(groupLabel),
                    .fileColumnModelIndexes = {i}});
            }
            else
            {
                it->fileColumnModelIndexes.append(i);
            }
        }
        else
        {
            params.append(DataColumnParams{
                .label = fileColumnParams.label,
                .name = fileColumnParams.name,
                .type = fileColumnParams.type,
                .elementsType = fileColumnParams.elementsType,
                .intervals = fileColumnParams.intervals,
                .values = fileColumnParams.values,
                .fileColumnModelIndexes = {i}});
        }
    }
    emit modelChanged(fullChange);
}

QList<QString> DataColumnParams::valuesStringList() const
{
    QList<QString> result;
    for (const auto& value: values)
    {
        result.append(value.text);
    }
    return result;
}

QString DataColumnParams::valueByCode(const QString& code) const
{
    for (const auto& value: values)
    {
        if (value.code == code)
        {
            return value.text;
        }
    }
    return code;
}

QString DataColumnParams::codeByValue(const QString& text) const
{
    for (const auto& value: values)
    {
        if (value.text == text)
        {
            return value.code;
        }
    }
    return text;
}

QString DataColumnParams::intervalsString() const
{
    QList<QString> intervalsStrList;
    for (const auto& interval: intervals)
    {
        intervalsStrList.append(QString("[%1; %2]")
                                    .arg(
                                        QString::number(interval.first),
                                        QString::number(interval.second)));
    }
    return intervalsStrList.join(", ");
}

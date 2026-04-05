#include "filecolumnsmodel.h"

#include <QJsonArray>
#include <QVariant>

namespace
{

const QString labelTag = "label";
const QString nameTag = "name";
const QString typeTag = "type";
const QString elementsTypeTag = "elements_type";
const QString intervalsTag = "intervals";
const QString minTag = "min";
const QString maxTag = "max";
const QString groupTag = "group";
const QString valuesTag = "values";
const QString textTag = "text";
const QString codeTag = "code";
const QString descriptionTag = "description";

} // namespace

FileColumnsModel::FileColumnsModel(
    const QList<QVariant>& config, QObject* parent) :
  QObject(parent)
{
    setModel(config, true);
}

void FileColumnsModel::setModel(const QList<QVariant>& config, bool fullChange)
{
    this->config = config;
    params.clear();

    // TODO: Template functions for conversion from/to JSON
    for (const auto& columnConfig: config)
    {
        QJsonObject columnConfigObj = columnConfig.toJsonObject();
        ColumnParams columnParams;

        columnParams.name = columnConfigObj.value(nameTag).toString();
        QString typeStr = columnConfigObj.value(typeTag).toString();
        columnParams.type = ColumnType::fromString(typeStr);

        if (columnParams.type == ColumnType::LIST)
        {
            columnParams.elementsType = ColumnType::fromString(
                columnConfigObj.value(elementsTypeTag).toString());
        }

        if (columnParams.type == ColumnType::INT
            || columnParams.type == ColumnType::DOUBLE
            || (columnParams.type == ColumnType::LIST
                && (*columnParams.elementsType == ColumnType::INT
                    || *columnParams.elementsType == ColumnType::DOUBLE)))
        {
            if (columnConfigObj.contains(intervalsTag))
            {
                QJsonArray intervals =
                    columnConfigObj.value(intervalsTag).toArray();
                for (int i = 0; i < intervals.size(); i++)
                {
                    QJsonObject interval = intervals.at(i).toObject();
                    columnParams.intervals.append(QPair<double, double>(
                        interval.value(minTag).toDouble(),
                        interval.value(maxTag).toDouble()));
                }
            }
        }

        if (columnParams.type == ColumnType::STRING)
        {
            if (columnConfigObj.contains(valuesTag))
            {
                QJsonArray values = columnConfigObj.value(valuesTag).toArray();
                for (int i = 0; i < values.size(); i++)
                {
                    QJsonObject valueObj = values.at(i).toObject();
                    columnParams.values.append(StringValueParams{
                        .text = valueObj.value(textTag).toString(),
                        .code = valueObj.value(codeTag).toString(),
                        .description =
                            valueObj.value(descriptionTag).toString()});
                }
            }
        }

        if (columnParams.type == ColumnType::BOOL
            && columnConfigObj.contains(groupTag))
        {
            columnParams.isInGroup = true;
            columnParams.groupLabel =
                columnConfigObj.value(groupTag).toString();
            if (columnConfigObj.contains(descriptionTag))
            {
                columnParams.values.append(StringValueParams{
                    .text = QString(),
                    .code = QString(),
                    .description =
                        columnConfigObj.value(descriptionTag).toString()});
            }
        }
        else
        {
            columnParams.label = columnConfigObj.value(labelTag).toString();
        }
        params.append(columnParams);
    }

    emit modelChanged(fullChange);
}

QList<QVariant> FileColumnsModel::getConfig() const
{
    return config;
}

int FileColumnsModel::count() const
{
    return params.size();
}

ColumnParams FileColumnsModel::at(int index) const
{
    return params.at(index);
}

QList<StringValueParams> FileColumnsModel::valuesForGroup(
    const QString& groupLabel) const
{
    QList<StringValueParams> result;
    for (const auto& columnParams: params)
    {
        if (columnParams.isInGroup && *columnParams.groupLabel == groupLabel)
        {
            StringValueParams valueParams;
            valueParams.text = columnParams.name;
            if (!columnParams.values.isEmpty())
            {
                valueParams.code = columnParams.values.first().code;
                valueParams.description =
                    columnParams.values.first().description;
            }
            result.append(valueParams);
        }
    }
    return result;
}

QString ColumnParams::valueByCode(const QString& code) const
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

QString ColumnParams::codeByValue(const QString& text) const
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

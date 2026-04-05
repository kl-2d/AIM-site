#include "tableproxymodel.h"

#include "tablemodel.h"
#include "utils.h"

#include <QDate>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

namespace
{

const QString modelFiltersTag = "model_filters";
const QString programFiltersTag = "program_filters";
const QString recordsTag = "records";
const QString intervalsTag = "intervals";
const QString minTag = "min";
const QString maxTag = "max";
const QString valuesTag = "values";
const QString textTag = "text";
const QString datesTag = "dates";

} // namespace

TableProxyModel::TableProxyModel(
    const DataColumnsModel* columnsModel, QObject* parent) :
  QSortFilterProxyModel(parent)
{
    model = columnsModel;
}

QVariant TableProxyModel::headerData(
    int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return {};
    }
    if (orientation == Qt::Vertical)
    {
        return QString::number(section + 1);
    }
    else
    {
        return sourceModel()->headerData(section, orientation, role);
    }
}

void TableProxyModel::setFiltersFromFile(const QString& fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    QJsonParseError error{};
    QJsonDocument filtersFile(QJsonDocument::fromJson(file.readAll(), &error));
    QJsonObject filtersObject = filtersFile.object();
    file.close();

    setFilters(filtersObject);
}

const DataColumnsModel* TableProxyModel::getModel() const
{
    return model;
}

int TableProxyModel::totalDataRows() const
{
    return filterAcceptsRow(sourceModel()->rowCount() - 1, {}) ?
               rowCount() - 1 :
               rowCount();
}

int TableProxyModel::rowsWithErrors() const
{
    int result = 0;
    for (int i = 0; i < totalDataRows(); i++)
    {
        auto sourceIndex = mapToSource(index(i, 0));
        auto tableModel = qobject_cast<TableModel*>(sourceModel());
        if (!tableModel->isValid(sourceIndex.row()))
        {
            result++;
        }
    }
    return result;
}

void TableProxyModel::setFilters(const QJsonObject& filtersObject)
{
    filters.clear();
    auto modelFilters = filtersObject.value(modelFiltersTag).toObject();
    auto keys = modelFilters.keys();
    for (const auto& key: keys)
    {
        QJsonObject filter = modelFilters.value(key).toObject();
        {
            if (filter.contains(intervalsTag))
            {
                QJsonArray intervals = filter.value(intervalsTag).toArray();
                for (int i = 0; i < intervals.size(); i++)
                {
                    QJsonObject interval = intervals.at(i).toObject();
                    filters[key].intervals.append(QPair<double, double>(
                        interval.value(minTag).toDouble(),
                        interval.value(maxTag).toDouble()));
                }
            }
            if (filter.contains(valuesTag))
            {
                QJsonArray values = filter.value(valuesTag).toArray();
                for (int i = 0; i < values.size(); i++)
                {
                    filters[key].values.append(values.at(i).toString());
                }
            }
            if (filter.contains(textTag))
            {
                filters[key].text = filter.value(textTag).toString();
            }
            if (filter.contains(datesTag))
            {
                QJsonObject dates = filter.value(datesTag).toObject();
                filters[key].dates = QPair<QString, QString>(
                    dates.value(minTag).toString(),
                    dates.value(maxTag).toString());
            }
        }
    }
    auto programFilters = filtersObject.value(programFiltersTag).toObject();
    recordsStatus =
        BoolStatus::fromString(programFilters.value(recordsTag).toString());
    invalidateFilter();
    emit filtersUpdated(filters);
}

void TableProxyModel::deleteSelectedData(const QModelIndexList& selected)
{
    QModelIndexList sourceSelection;
    for (const auto& modelIndex: selected)
    {
        sourceSelection.append(mapToSource(modelIndex));
    }
    auto tableModel = qobject_cast<TableModel*>(sourceModel());
    tableModel->deleteSelectedData(sourceSelection);
}

bool TableProxyModel::filterAcceptsRow(
    int sourceRow, const QModelIndex& sourceParent) const
{
    Q_UNUSED(sourceParent)
    if (sourceRow == sourceModel()->rowCount() - 1
        && (!filters.isEmpty() || recordsStatus != BoolStatus::ALL))
    {
        return false;
    }
    for (int i = 0; i < model->count(); i++)
    {
        auto label = model->at(i).label;
        auto type = model->at(i).type;
        if (filters.contains(label))
        {
            auto value =
                sourceModel()->data(sourceModel()->index(sourceRow, i));
            auto filter = filters.value(label);
            if (!filter.intervals.isEmpty())
            {
                bool found = false;
                for (const auto& interval: filter.intervals)
                {
                    if (value.toDouble() >= interval.first
                        && value.toDouble() <= interval.second)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    return false;
                }
            }
            if (!filter.values.isEmpty())
            {
                if (type == ColumnType::BOOL)
                {
                    auto status = BoolStatus::fromString(filter.values.first());
                    auto boolValue = Utils::strToBool(value.toString());
                    if (!boolValue
                        || (status == BoolStatus::YES && *boolValue == false)
                        || (status == BoolStatus::NO && *boolValue == true))
                    {
                        return false;
                    }
                }
                else if (!filter.values.contains(value.toString()))
                {
                    return false;
                }
            }
            if (!filter.text.isEmpty())
            {
                if (!value.toString().contains(filter.text))
                {
                    return false;
                }
            }
            if (filter.dates.has_value())
            {
                QDate valueDate =
                    QDate::fromString(value.toString(), "dd.MM.yyyy");
                QDate minDate =
                    QDate::fromString(filter.dates->first, "yyyy-MM-dd");
                QDate maxDate =
                    QDate::fromString(filter.dates->second, "yyyy-MM-dd");
                if (!valueDate.isValid() || valueDate < minDate || valueDate > maxDate)
                {
                    return false;
                }
            }
        }
    }
    auto tableModel = qobject_cast<TableModel*>(sourceModel());
    if (recordsStatus == BoolStatus::YES)
    {
        return tableModel->isValid(sourceRow);
    }
    else if (recordsStatus == BoolStatus::NO)
    {
        return tableModel->rowCount() - 1 == sourceRow
               || !tableModel->isValid(sourceRow);
    }
    return true;
}

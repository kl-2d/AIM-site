#ifndef TABLEPROXYMODEL_H
#define TABLEPROXYMODEL_H

#include "boolstatus.h"
#include "datacolumnsmodel.h"
#include "filterparams.h"

#include <QList>
#include <QMap>
#include <QSortFilterProxyModel>
#include <QString>

#include <optional>

class TableProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    TableProxyModel(
        const DataColumnsModel* columnsModel, QObject* parent = nullptr);

    QVariant headerData(
        int section, Qt::Orientation orientation, int role) const override;

    void setFiltersFromFile(const QString& fileName);
    const DataColumnsModel* getModel() const;
    int totalDataRows() const;
    int rowsWithErrors() const;

public slots:
    void setFilters(const QJsonObject& filtersObject);
    void deleteSelectedData(const QModelIndexList& selected);

signals:
    void filtersUpdated(QMap<QString, FilterParams> filters);

protected:
    [[nodiscard]] bool filterAcceptsRow(
        int sourceRow, const QModelIndex& sourceParent) const override;

private:
    const DataColumnsModel* model;
    QMap<QString, FilterParams> filters;
    BoolStatus::Value recordsStatus = BoolStatus::ALL;
};

#endif // TABLEPROXYMODEL_H

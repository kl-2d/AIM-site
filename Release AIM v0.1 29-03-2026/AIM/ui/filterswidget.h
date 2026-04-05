#ifndef FILTERSWIDGET_H
#define FILTERSWIDGET_H

#include "collapsiblewidget.h"
#include "datacolumnsmodel.h"
#include "filternamesstorage.h"
#include "filterparams.h"
#include "filterwidget.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QWidget>

class FiltersWidget : public QWidget
{
    Q_OBJECT

public:
    FiltersWidget(FilterNamesStorage* namesStorage, QWidget* parent = nullptr);

    void setModel(const DataColumnsModel* model);

    QJsonObject getFilters(bool updateModelFilters);

    void updateStatus(int totalRows, int rowsWithErrors);

    void apply();
    void reset();
    void save();

public slots:
    void setFilters(const QMap<QString, FilterParams>& newFilters);

signals:
    void applyFiltersRequested(QJsonObject filters);
    void saveFiltersRequested(QJsonObject filters);
    void importFiltersRequested();
    void loadFiltersRequested(QString fileName);
    void removeFiltersRequested(QString fileName);

private:
    void createLayout();
    void createFilterWidgets();
    QString getFilterParametersString() const;

private:
    const DataColumnsModel* columnsModel;

    QList<QPair<QString, FilterWidget*>> filters;
    QList<CollapsibleWidget*> collapsibleWidgets;

    QPushButton* hideAllButton;

    FilterNamesStorage* filterNamesStorage;

    QComboBox* recordsComboBox;
    QButtonGroup* recordsButtonGroup;
    QRadioButton* allRadioButton;
    QRadioButton* correctRadioButton;
    QRadioButton* withErrorsRadioButton;
    QCheckBox* extraMeasuresCheckBox = nullptr;

    QLabel* statusLabel;

    QComboBox* filtersComboBox;
    QPushButton* removeFilterButton;
    QPushButton* importButton;

    QPushButton* resetButton;
    QPushButton* applyButton;
    QPushButton* saveButton;

    QJsonObject lastModelFilters;
};

#endif // FILTERSWIDGET_H

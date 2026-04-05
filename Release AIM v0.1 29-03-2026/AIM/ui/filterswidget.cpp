#include "filterswidget.h"

#include "boolfilterwidget.h"
#include "boolstatus.h"
#include "datetimefilterwidget.h"
#include "numberfilterwidget.h"
#include "stringfilterwidget.h"
#include "stringvaluesfilterwidget.h"
#include "verticalseparator.h"

#include <QCoreApplication>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

const QString modelFiltersTag = "model_filters";
const QString programFiltersTag = "program_filters";
const QString recordsTag = "records";

void clearLayout(QLayout* layout)
{
    while (layout->count() > 0)
    {
        QLayoutItem* item = layout->takeAt(0);

        QWidget* widget = item->widget();
        if (widget)
        {
            delete widget;
        }
        else
        {
            QLayout* layout = item->layout();
            if (layout)
            {
                clearLayout(layout);
            }
            else
            {
                QSpacerItem* si = item->spacerItem();
                if (si)
                {
                    delete si;
                }
            }
        }
        delete item;
    }
}

FiltersWidget::FiltersWidget(FilterNamesStorage* namesStorage, QWidget* parent) :
  QWidget(parent)
{
    filterNamesStorage = namesStorage;
}

void FiltersWidget::setModel(const DataColumnsModel* model)
{
    columnsModel = model;
    QObject::connect(
        columnsModel, &DataColumnsModel::modelChanged, this, [this] {
            clearLayout(layout());
            delete layout();
            filters.clear();
            collapsibleWidgets.clear();
            createFilterWidgets();
            createLayout();
        });
    createFilterWidgets();
    createLayout();
}

QJsonObject FiltersWidget::getFilters(bool updateModelFilters)
{
    QJsonObject result;
    QJsonObject modelFilters;
    if (updateModelFilters)
    {
        for (const auto& namefilterPair: qAsConst(filters))
        {
            QJsonObject filter = namefilterPair.second->getFilter();
            if (!filter.isEmpty())
            {
                modelFilters.insert(namefilterPair.second->getTag(), filter);
            }
        }
        lastModelFilters = modelFilters;
    }
    else
    {
        modelFilters = lastModelFilters;
    }
    result.insert(modelFiltersTag, modelFilters);

    QJsonObject programFilters;
    auto checkedButton = recordsButtonGroup->checkedButton();
    BoolStatus::Value recordsStatus;
    if (checkedButton == allRadioButton)
    {
        recordsStatus = BoolStatus::ALL;
    }
    else if (checkedButton == correctRadioButton)
    {
        recordsStatus = BoolStatus::YES;
    }
    else if (checkedButton == withErrorsRadioButton)
    {
        recordsStatus = BoolStatus::NO;
    }
    programFilters.insert(recordsTag, BoolStatus::toString(recordsStatus));

    result.insert(programFiltersTag, programFilters);

    return result;
}

void FiltersWidget::updateStatus(int totalRows, int rowsWithErrors)
{
    statusLabel->setText(
        QCoreApplication::translate(
            "Filters",
            "Filter parameters:\n%1\n\nRecords count: %2, with errors: %3")
            .arg(
                getFilterParametersString(),
                QString::number(totalRows),
                QString::number(rowsWithErrors)));
}

void FiltersWidget::apply()
{
    emit applyFiltersRequested(getFilters(true));
}

void FiltersWidget::reset()
{
    for (const auto& namefilterPair: filters)
    {
        namefilterPair.second->reset();
    }
    allRadioButton->setChecked(true);
    emit applyFiltersRequested(getFilters(true));
}

void FiltersWidget::save()
{
    emit saveFiltersRequested(getFilters(true));
}

void FiltersWidget::setFilters(const QMap<QString, FilterParams>& newFilters)
{
    QJsonObject newModelFilters;
    for (const auto& nameFilterPair: filters)
    {
        QString label = nameFilterPair.second->getTag();
        if (newFilters.contains(label))
        {
            nameFilterPair.second->setFilter(newFilters.value(label));
        }
        else
        {
            nameFilterPair.second->reset();
        }

        QJsonObject filter = nameFilterPair.second->getFilter();
        if (!filter.isEmpty())
        {
            newModelFilters.insert(nameFilterPair.second->getTag(), filter);
        }
    }
    lastModelFilters = newModelFilters;

    if (newFilters.isEmpty())
    {
        filtersComboBox->blockSignals(true);
        filtersComboBox->setCurrentIndex(0);
        filtersComboBox->blockSignals(false);
    }
}

void FiltersWidget::createLayout()
{
    const int bottomButtonsHeight = 40;

    QLabel* filtersLabel =
        new QLabel(QCoreApplication::translate("Filters", "Current filter:"));

    filtersComboBox = new QComboBox();
    filtersComboBox->addItem(QCoreApplication::translate("Filters", "None"));
    filtersComboBox->addItems(filterNamesStorage->getNames());
    QObject::connect(
        filtersComboBox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        [this](int index) {
            if (index == 0)
            {
                removeFilterButton->setEnabled(false);
                emit applyFiltersRequested({});
            }
            else
            {
                removeFilterButton->setEnabled(true);
                auto filePath = filterNamesStorage->getFilePath(
                    filtersComboBox->itemText(index));
                emit loadFiltersRequested(filePath);
            }
        });
    QObject::connect(
        filterNamesStorage, &FilterNamesStorage::updated, this, [this]() {
            filtersComboBox->blockSignals(true);
            filtersComboBox->clear();
            filtersComboBox->addItem(
                QCoreApplication::translate("Filters", "None"));
            filtersComboBox->addItems(filterNamesStorage->getNames());
            filtersComboBox->blockSignals(false);
        });

    QObject::connect(
        filterNamesStorage,
        &FilterNamesStorage::added,
        this,
        [this](const QString& name) {
            filtersComboBox->blockSignals(true);
            int index = filtersComboBox->findText(name);
            filtersComboBox->setCurrentIndex(index);
            filtersComboBox->blockSignals(false);
        });

    removeFilterButton = new QPushButton();
    removeFilterButton->setEnabled(false);
    removeFilterButton->setText(
        QCoreApplication::translate("Filters", "Remove filter"));
    QObject::connect(removeFilterButton, &QPushButton::clicked, this, [this]() {
        if (filtersComboBox->currentIndex() != 0)
        {
            emit removeFiltersRequested(filtersComboBox->currentText());
            removeFilterButton->setEnabled(false);
            emit applyFiltersRequested({});
        }
    });

    importButton = new QPushButton();
    importButton->setText(
        QCoreApplication::translate("Filters", "Load filter"));
    QObject::connect(importButton, &QPushButton::clicked, this, [this]() {
        emit importFiltersRequested();
    });

    resetButton = new QPushButton();
    resetButton->setText(QCoreApplication::translate("Filters", "Reset"));
    resetButton->setFixedHeight(bottomButtonsHeight);

    applyButton = new QPushButton();
    applyButton->setText(QCoreApplication::translate("Filters", "Apply"));
    applyButton->setFixedHeight(bottomButtonsHeight);
    QObject::connect(applyButton, &QPushButton::clicked, this, [this]() {
        apply();
    });

    saveButton = new QPushButton();
    saveButton->setText(QCoreApplication::translate("Filters", "Save"));
    saveButton->setFixedHeight(bottomButtonsHeight);
    QObject::connect(saveButton, &QPushButton::clicked, this, [this]() {
        save();
    });

    QHBoxLayout* topControlsLayout = new QHBoxLayout();
    topControlsLayout->addWidget(filtersComboBox);
    topControlsLayout->addWidget(removeFilterButton);
    topControlsLayout->addWidget(importButton);

    QHBoxLayout* bottomButtonsLayout = new QHBoxLayout();
    bottomButtonsLayout->addWidget(resetButton);
    bottomButtonsLayout->addWidget(applyButton);
    bottomButtonsLayout->addWidget(saveButton);

    QVBoxLayout* leftFiltersScrollLayout = new QVBoxLayout();
    for (const auto& nameFilterPair: filters)
    {
        CollapsibleWidget* collapsibleWidget =
            new CollapsibleWidget(nameFilterPair.first, nameFilterPair.second);
        collapsibleWidget->setSizePolicy(
            QSizePolicy::Preferred, QSizePolicy::Maximum);
        collapsibleWidgets.append(collapsibleWidget);
        leftFiltersScrollLayout->addWidget(collapsibleWidget);
    }
    leftFiltersScrollLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* leftFiltersWidget = new QWidget();
    leftFiltersWidget->setContentsMargins(0, 0, 0, 0);
    leftFiltersWidget->setLayout(leftFiltersScrollLayout);
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidget(leftFiltersWidget);
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->setMinimumWidth(200);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    scrollArea->setStyleSheet(
        "QScrollArea { background: transparent; } \
         QScrollArea > QWidget > QWidget { background: transparent; } \
         QScrollArea > QWidget > QScrollBar { background: palette(base); }");

    QVBoxLayout* leftFiltersLayout = new QVBoxLayout();
    leftFiltersLayout->addWidget(scrollArea);

    hideAllButton = new QPushButton();
    hideAllButton->setText(QCoreApplication::translate("Filters", "Hide all"));
    QObject::connect(hideAllButton, &QPushButton::clicked, this, [this]() {
        for (auto& widget: collapsibleWidgets)
        {
            widget->setCollapsed(true);
        }
    });
    leftFiltersLayout->addWidget(hideAllButton);

    QLabel* recordsLabel = new QLabel();
    recordsLabel->setText(QCoreApplication::translate("Filters", "Records"));

    allRadioButton =
        new QRadioButton(QCoreApplication::translate("Filters", "All"));
    allRadioButton->setStyleSheet(
        "QRadioButton::checked { background-color: #FFFF00 }");
    correctRadioButton =
        new QRadioButton(QCoreApplication::translate("Filters", "Correct"));
    correctRadioButton->setStyleSheet(
        "QRadioButton::checked { background-color: #00FF00 }");
    withErrorsRadioButton =
        new QRadioButton(QCoreApplication::translate("Filters", "With errors"));
    withErrorsRadioButton->setStyleSheet(
        "QRadioButton::checked { background-color: #FF0000 }");

    recordsButtonGroup = new QButtonGroup(this);
    recordsButtonGroup->addButton(allRadioButton);
    recordsButtonGroup->addButton(correctRadioButton);
    recordsButtonGroup->addButton(withErrorsRadioButton);

    QObject::connect(
        recordsButtonGroup,
        QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
        this,
        [this]() { emit applyFiltersRequested(getFilters(false)); });

    allRadioButton->setChecked(true);

    QObject::connect(resetButton, &QPushButton::clicked, this, [this]() {
        reset();
    });

    /*QLabel* extraMeasuresLabel = new QLabel();
    extraMeasuresLabel->setText(
        QCoreApplication::translate("Filters", "Extra measures"));

    extraMeasuresCheckBox = new QCheckBox();
    extraMeasuresCheckBox->setText(
        QCoreApplication::translate("Filters", "Show"));*/

    QLabel* spacer2 = new QLabel();
    spacer2->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    statusLabel = new QLabel();
    statusLabel->setWordWrap(true);

    QVBoxLayout* rightFiltersLayout = new QVBoxLayout();
    rightFiltersLayout->addWidget(recordsLabel);
    rightFiltersLayout->addWidget(allRadioButton);
    rightFiltersLayout->addWidget(correctRadioButton);
    rightFiltersLayout->addWidget(withErrorsRadioButton);
    /*rightFiltersLayout->addWidget(extraMeasuresLabel);
    rightFiltersLayout->addWidget(extraMeasuresCheckBox);*/
    rightFiltersLayout->addWidget(spacer2);
    rightFiltersLayout->addWidget(statusLabel);

    VerticalSeparator* separator = new VerticalSeparator();

    QHBoxLayout* allFiltersLayout = new QHBoxLayout();
    allFiltersLayout->addLayout(leftFiltersLayout);
    allFiltersLayout->addWidget(separator);
    allFiltersLayout->addLayout(rightFiltersLayout);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(filtersLabel);
    layout->addLayout(topControlsLayout);
    layout->addLayout(allFiltersLayout);
    layout->addLayout(bottomButtonsLayout);

    setLayout(layout);
}

void FiltersWidget::createFilterWidgets()
{
    for (int i = 0; i < columnsModel->count(); i++)
    {
        DataColumnParams columnParams = columnsModel->at(i);
        if (columnParams.type == ColumnType::STRING)
        {
            if (columnParams.values.isEmpty())
            {
                StringFilterWidget* stringFilter =
                    new StringFilterWidget(columnParams.label);
                filters.append(QPair<QString, FilterWidget*>(
                    columnParams.name, stringFilter));
            }
            else
            {
                StringValuesFilterWidget* stringFilter =
                    new StringValuesFilterWidget(
                        columnParams.values, columnParams.label);
                filters.append(QPair<QString, FilterWidget*>(
                    columnParams.name, stringFilter));
            }
        }
        else if (
            columnParams.type == ColumnType::INT
            || columnParams.type == ColumnType::DOUBLE)
        {
            NumberFilterWidget* numberFilter =
                new NumberFilterWidget(columnParams.label);
            filters.append(
                QPair<QString, FilterWidget*>(columnParams.name, numberFilter));
        }
        else if (columnParams.type == ColumnType::DATETIME)
        {
            DatetimeFilterWidget* datetimeFilter =
                new DatetimeFilterWidget(columnParams.label);
            filters.append(QPair<QString, FilterWidget*>(
                columnParams.name, datetimeFilter));
        }
        else if (columnParams.type == ColumnType::BOOL)
        {
            BoolFilterWidget* boolFilter =
                new BoolFilterWidget(columnParams.label);
            filters.append(
                QPair<QString, FilterWidget*>(columnParams.name, boolFilter));
        }
    }
}

QString FiltersWidget::getFilterParametersString() const
{
    QStringList filterParameters;
    for (const auto& nameFilterPair: filters)
    {
        if (!nameFilterPair.second->isEmpty())
        {
            filterParameters.append(QString("%1: %2").arg(
                nameFilterPair.first,
                nameFilterPair.second->getFilterString()));
        }
    }
    return filterParameters.isEmpty() ?
               QCoreApplication::translate("Filters", "None") :
               filterParameters.join("\n");
}

#include "tablesettingswidget.h"

#include "horizontalseparator.h"

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSet>
#include <QVBoxLayout>

namespace
{

const QString labelTag = "label";
const QString nameTag = "name";
const QString columnsTag = "columns";

} // namespace

void _clearLayout(QLayout* layout)
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
                _clearLayout(layout);
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

inline void insertWidgetWithSeparator(
    int index, QVBoxLayout* layout, QWidget* widget)
{
    QVBoxLayout* newlayout = new QVBoxLayout();
    newlayout->addWidget(widget);
    newlayout->addWidget(new HorizontalSeparator());
    layout->insertItem(index, newlayout);
}

TableSettingsWidget::TableSettingsWidget(bool createMode,
                                         const QList<QVariant> &settings,
                                         QWidget *parent)
    : QDialog(parent)
{
    setMinimumWidth(1000);
    setMinimumHeight(650);
    setStyleSheet(
        "QDialog { background-image: url(:/images/background_settings.jpg) }");

    tableHeaderPrewiewWidget = new TableHeaderPreviewWidget();

    columnSettingsLayout = new QVBoxLayout();

    errorLabel = new QLabel();
    errorLabel->setStyleSheet("QLabel { color: red; }");

    if (createMode)
    {
        addColumnButton = new QPushButton(
            QCoreApplication::translate("TableSettings", "Add column"));
        selectColumnButton = new QPushButton(
            QCoreApplication::translate("TableSettings", "Select column"));
        QObject::connect(
            addColumnButton,
            &QPushButton::clicked,
            this,
            [this]() { addColumn(); });
        QObject::connect(
            selectColumnButton,
            &QPushButton::clicked,
            this,
            [this]() { QMessageBox::information(this, QCoreApplication::translate("TableSettings", "Not implemented"), QCoreApplication::translate("TableSettings", "Not yet implemented")); });
    }

    buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(
        buttons->button(QDialogButtonBox::Ok),
        &QPushButton::clicked,
        this,
        [this, createMode]() {
            if (!validate())
            {
                return;
            }
            if (createMode)
            {
                QMessageBox confirmation(
                    QMessageBox::Warning,
                    QCoreApplication::translate(
                        "TableSettings", "Changing table settings"),
                    QCoreApplication::translate(
                        "TableSettings",
                        "Changing the table settings will result in the loss of all current data. Do you want to continue?"),
                    QMessageBox::Yes | QMessageBox::No,
                    this);
                if (confirmation.exec() == QMessageBox::Yes)
                {
                    accept();
                }
            }
            else
            {
                accept();
            }
        });
    connect(
        buttons->button(QDialogButtonBox::Cancel),
        &QPushButton::clicked,
        this,
        &QDialog::reject);

    QHBoxLayout* controlButtonsLayout = new QHBoxLayout();
    if (createMode)
    {
        controlButtonsLayout->addWidget(addColumnButton);
        controlButtonsLayout->addWidget(selectColumnButton);
    }

    QWidget* columnSettingsWidget = new QWidget();
    columnSettingsWidget->setContentsMargins(0, 0, 0, 0);
    columnSettingsWidget->setLayout(columnSettingsLayout);
    auto* scrollArea = new QScrollArea();
    scrollArea->setWidget(columnSettingsWidget);
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    scrollArea->setStyleSheet(
        "QScrollArea { background: transparent; } \
         QScrollArea > QWidget > QWidget { background: transparent; } \
         QScrollArea > QWidget > QScrollBar { background: palette(base); }");

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(tableHeaderPrewiewWidget);
    layout->addWidget(scrollArea, 0);
    layout->addWidget(errorLabel);
    layout->addLayout(controlButtonsLayout, 0);
    layout->addWidget(buttons, 0, Qt::AlignRight | Qt::AlignBottom);

    setLayout(layout);

    if (!createMode)
    {
        for (const auto& columnSettings: settings)
        {
            addColumn(columnSettings.toJsonObject(), false);
        }
    }
}

QJsonObject TableSettingsWidget::getSettings() const
{
    QJsonObject result;
    QJsonArray columnsArray;
    for (const auto& columnSettingsWidget: columnSettingsWidgets)
    {
        columnsArray.append(columnSettingsWidget->getSettings());
    }
    result.insert(columnsTag, columnsArray);
    return result;
}

void TableSettingsWidget::processUpButtonClicked(const QUuid& widgetId)
{
    for (int i = 1; i < columnSettingsWidgets.size(); i++)
    {
        ColumnSettingsWidget* widget = columnSettingsWidgets.at(i);
        if (widget->id() == widgetId)
        {
            columnSettingsWidgets.swapItemsAt(i, i - 1);

            auto* layout = columnSettingsLayout->itemAt(i - 1);
            QVBoxLayout* vBoxLayout =
                qobject_cast<QVBoxLayout*>(layout->layout());
            vBoxLayout->takeAt(0);
            vBoxLayout->insertWidget(0, columnSettingsWidgets.at(i - 1));

            layout = columnSettingsLayout->itemAt(i);
            vBoxLayout = qobject_cast<QVBoxLayout*>(layout->layout());
            vBoxLayout->takeAt(0);
            vBoxLayout->insertWidget(0, columnSettingsWidgets.at(i));

            tableHeaderPrewiewWidget->setHeaders(getColumnNames());
            return;
        }
    }
}

void TableSettingsWidget::processDownButtonClicked(const QUuid& widgetId)
{
    for (int i = 0; i < columnSettingsWidgets.size() - 1; i++)
    {
        ColumnSettingsWidget* widget = columnSettingsWidgets.at(i);
        if (widget->id() == widgetId)
        {
            columnSettingsWidgets.swapItemsAt(i, i + 1);

            auto* layout = columnSettingsLayout->itemAt(i);
            QVBoxLayout* vBoxLayout =
                qobject_cast<QVBoxLayout*>(layout->layout());
            vBoxLayout->takeAt(0);
            vBoxLayout->insertWidget(0, columnSettingsWidgets.at(i));

            layout = columnSettingsLayout->itemAt(i + 1);
            vBoxLayout = qobject_cast<QVBoxLayout*>(layout->layout());
            vBoxLayout->takeAt(0);
            vBoxLayout->insertWidget(0, columnSettingsWidgets.at(i + 1));

            tableHeaderPrewiewWidget->setHeaders(getColumnNames());
            return;
        }
    }
}

void TableSettingsWidget::processRemoveButtonClicked(const QUuid& widgetId)
{
    for (int i = 0; i < columnSettingsWidgets.size(); i++)
    {
        ColumnSettingsWidget* widget = columnSettingsWidgets.at(i);
        if (widget->id() == widgetId)
        {
            auto* layout = columnSettingsLayout->takeAt(i);
            _clearLayout(layout->layout());
            columnSettingsWidgets.removeAt(i);

            tableHeaderPrewiewWidget->setHeaders(getColumnNames());
            return;
        }
    }
}

void TableSettingsWidget::processDuplicateButtonClicked(const QUuid& widgetId)
{
    for (int i = 0; i < columnSettingsWidgets.size(); i++)
    {
        ColumnSettingsWidget* widget = columnSettingsWidgets.at(i);
        if (widget->id() == widgetId)
        {
            auto settings = widget->getSettings();
            addColumn(settings, i + 1);
        }
    }
}

void TableSettingsWidget::processSaveButtonClicked(const QUuid& widgetId)
{
    Q_UNUSED(widgetId)
    QMessageBox::information(this, QCoreApplication::translate("TableSettings", "Not implemented"), QCoreApplication::translate("TableSettings", "Not yet implemented"));
}

void TableSettingsWidget::addColumn(const QJsonObject &settings, bool createMode, int position)
{
    ColumnSettingsWidget* settingsWidget = new ColumnSettingsWidget(createMode);
    settingsWidget->setSizePolicy(
        QSizePolicy::Preferred, QSizePolicy::Maximum);
    QObject::connect(
        settingsWidget,
        &ColumnSettingsWidget::upButtonClicked,
        this,
        &TableSettingsWidget::processUpButtonClicked);
    QObject::connect(
        settingsWidget,
        &ColumnSettingsWidget::downButtonClicked,
        this,
        &TableSettingsWidget::processDownButtonClicked);
    if (createMode)
    {
        QObject::connect(
            settingsWidget,
            &ColumnSettingsWidget::removeButtonClicked,
            this,
            &TableSettingsWidget::processRemoveButtonClicked);
        QObject::connect(
            settingsWidget,
            &ColumnSettingsWidget::duplicateButtonClicked,
            this,
            &TableSettingsWidget::processDuplicateButtonClicked);
        QObject::connect(
            settingsWidget, &ColumnSettingsWidget::nameChanged, this, [this]() {
                tableHeaderPrewiewWidget->setHeaders(getColumnNames());
            });
    }
    QObject::connect(
        settingsWidget,
        &ColumnSettingsWidget::saveButtonClicked,
        this,
        &TableSettingsWidget::processSaveButtonClicked);
    HorizontalSeparator* separator = new HorizontalSeparator();
    /*QObject::connect(
            removeButton,
            &QPushButton::clicked,
            this,
            [this, settingsWidget, removeButton, separator]() {
                for (int i = 0; i < columnSettingsWidgets.size(); i++)
                {
                    if (columnSettingsWidgets.at(i) == settingsWidget)
                    {
                        columnSettingsLayout->removeItem(
                            columnSettingsLayout->itemAt(i));
                        columnSettingsWidgets.removeAt(i);
                        delete settingsWidget;
                        delete removeButton;
                        delete separator;
                        return;
                    }
                }
            });*/

    if (position >= 0)
    {
        columnSettingsWidgets.insert(position, settingsWidget);
    }
    else
    {
        columnSettingsWidgets.append(settingsWidget);
    }

    if (!settings.isEmpty())
    {
        settingsWidget->setSettings(settings);
    }

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(settingsWidget, 0);
    layout->addWidget(separator);

    if (position >= 0)
    {
        columnSettingsLayout->insertLayout(position, layout);
    }
    else
    {
        columnSettingsLayout->addLayout(layout);
    }

    tableHeaderPrewiewWidget->setHeaders(getColumnNames());
}

QList<QString> TableSettingsWidget::getColumnNames() const
{
    QList<QString> result;
    for (const auto* columnSettingsWidget: columnSettingsWidgets)
    {
        result.append(columnSettingsWidget->name());
    }
    return result;
}

bool TableSettingsWidget::validate()
{
    errorLabel->clear();
    bool isValid = validateLabels();
    for (auto* columnSettingsWidget: columnSettingsWidgets)
    {
        QString errorStr = columnSettingsWidget->validate();
        if (!errorStr.isEmpty())
        {
            errorLabel->setText(errorLabel->text() + errorStr + "\n");
            isValid = false;
        }
    }
    return isValid;
}

bool TableSettingsWidget::validateLabels()
{
    bool isValid = true;
    QList<QJsonObject> columns;
    for (auto& columnSettingsWidget: columnSettingsWidgets)
    {
        columnSettingsWidget->highlightName(false);
        columns.append(columnSettingsWidget->getSettings());
    }
    QSet<QString> labelsChecked;
    for (int i = 0; i < columns.size(); i++)
    {
        const QJsonObject& column = columns.at(i);
        QString label = column.value(labelTag).toString();
        QString name = column.value(nameTag).toString();
        if (name.isEmpty())
        {
            errorLabel->setText(
                errorLabel->text() +
                QCoreApplication::translate("TableSettings", "Empty name for column %1").arg(QString::number(i + 1)) +
                "\n");
            isValid = false;
            columnSettingsWidgets.at(i)->highlightName(true);
            continue;
        }
        else
        {
            if (name.at(0).isDigit())
            {
                errorLabel->setText(
                    errorLabel->text() +
                    QCoreApplication::translate("TableSettings", "Column name starts with number: %1").arg(name) +
                    "\n");
                isValid = false;
                columnSettingsWidgets.at(i)->highlightName(true);
            }

            if (!std::all_of(name.begin(), name.end(), [](const QChar& symbol) { return symbol.isLetterOrNumber(); }))
            {
                errorLabel->setText(
                    errorLabel->text() +
                    QCoreApplication::translate("TableSettings", "Column name contains unacceptable symbols: %1").arg(name) +
                    "\n");
                isValid = false;
                columnSettingsWidgets.at(i)->highlightName(true);
            }

            if (labelsChecked.contains(label))
            {
                continue;
            }
            if (std::count_if(
                    columns.begin(),
                    columns.end(),
                    [label](const QJsonObject& col)
                    { return col.value(labelTag).toString() == label;})
                > 1)
            {
                errorLabel->setText(
                    errorLabel->text() +
                    QCoreApplication::translate("TableSettings", "Duplicated column name: %1").arg(name) +
                    "\n");
                isValid = false;
                labelsChecked.insert(label);
                for (auto& columnSettingsWidget: columnSettingsWidgets)
                {
                    if (columnSettingsWidget->name() == name)
                    {
                        columnSettingsWidget->highlightName(true);
                    }
                }
            }
        }
    }
    return isValid;
}

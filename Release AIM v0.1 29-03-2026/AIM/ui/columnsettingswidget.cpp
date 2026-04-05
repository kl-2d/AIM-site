#include "columnsettingswidget.h"

#include "behaviortype.h"
#include "columntype.h"
#include "intcolumnsettingswidget.h"
#include "stringcolumnsettingswidget.h"
#include "utils.h"

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace
{

const QString labelTag = "label";
const QString nameTag = "name";
const QString typeTag = "type";

} // namespace

ColumnSettingsWidget::ColumnSettingsWidget(bool createMode, QWidget *parent)
    : QWidget(parent)
{
    widgetId = QUuid::createUuid();

    upButton = new QToolButton();
    QIcon moveUpIcon(":/icons/move_up.svg");
    upButton->setIcon(moveUpIcon);
    QObject::connect(upButton, &QToolButton::clicked, this, [this]() {
        emit upButtonClicked(id());
    });

    downButton = new QToolButton();
    QIcon moveDownIcon(":/icons/move_down.svg");
    downButton->setIcon(moveDownIcon);
    QObject::connect(downButton, &QToolButton::clicked, this, [this]() {
        emit downButtonClicked(id());
    });

    QLabel* typeLabel =
        new QLabel(QCoreApplication::translate("TableSettings", "Data type"));
    typeComboBox = new QComboBox();
    typeComboBox->addItem(
        ColumnType::toDisplayString(ColumnType::STRING), ColumnType::STRING);
    typeComboBox->addItem(
        ColumnType::toDisplayString(ColumnType::BOOL), ColumnType::BOOL);
    typeComboBox->addItem(
        ColumnType::toDisplayString(ColumnType::INT), ColumnType::INT);
    typeComboBox->addItem(
        ColumnType::toDisplayString(ColumnType::DOUBLE), ColumnType::DOUBLE);
    typeComboBox->addItem(
        ColumnType::toDisplayString(ColumnType::DATETIME),
        ColumnType::DATETIME);
    typeComboBox->setEnabled(createMode);

    QObject::connect(
        typeComboBox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &ColumnSettingsWidget::processTypeChange);

    QLabel* nameLabel =
        new QLabel(QCoreApplication::translate("TableSettings", "Column name"));
    nameEdit = new QLineEdit();
    nameEdit->setEnabled(createMode);
    QObject::connect(
        nameEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
            emit nameChanged(id(), text);
        });

    behaviorLabel =
        new QLabel(QCoreApplication::translate("TableSettings", "Behavior"));
    behaviorComboBox = new QComboBox();
    behaviorComboBox->addItem(
        BehaviorType::toDisplayString(BehaviorType::ANY_VALUES),
        BehaviorType::ANY_VALUES);
    behaviorComboBox->addItem(
        BehaviorType::toDisplayString(BehaviorType::VALUES_LIST),
        BehaviorType::VALUES_LIST);
    behaviorComboBox->addItem(
        BehaviorType::toDisplayString(BehaviorType::ENCODED_VALUES_LIST),
        BehaviorType::ENCODED_VALUES_LIST);
    QObject::connect(
        behaviorComboBox,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &ColumnSettingsWidget::processBehaviorChange);

    specificEditorsLayout = new QVBoxLayout();

    if (createMode)
    {
        removeButton = new QPushButton(
            QCoreApplication::translate("TableSettings", "Remove column"));
        QObject::connect(removeButton, &QPushButton::clicked, this, [this]() {
            emit removeButtonClicked(id());
        });

        duplicateButton = new QPushButton(
            QCoreApplication::translate("TableSettings", "Duplicate column"));
        QObject::connect(duplicateButton, &QPushButton::clicked, this, [this]() {
            emit duplicateButtonClicked(id());
        });
    }

    saveButton = new QPushButton(
        QCoreApplication::translate("TableSettings", "Save to catalog"));
    QObject::connect(saveButton, &QPushButton::clicked, this, [this]() {
        emit saveButtonClicked(id());
    });

    QVBoxLayout* upDownButtonsLayout = new QVBoxLayout();
    upDownButtonsLayout->addStretch();
    upDownButtonsLayout->addWidget(upButton);
    upDownButtonsLayout->addWidget(downButton);
    upDownButtonsLayout->addStretch();

    QVBoxLayout* typeLayout = new QVBoxLayout();
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(typeComboBox);

    QVBoxLayout* nameLayout = new QVBoxLayout();
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);

    QVBoxLayout* behaviorLayout = new QVBoxLayout();
    behaviorLayout->addWidget(behaviorLabel);
    behaviorLayout->addWidget(behaviorComboBox);

    QHBoxLayout* generalSettingsLayout = new QHBoxLayout();
    generalSettingsLayout->addLayout(typeLayout);
    generalSettingsLayout->addLayout(nameLayout);
    generalSettingsLayout->addLayout(behaviorLayout);

    QVBoxLayout* mainSettingsLayout = new QVBoxLayout();
    mainSettingsLayout->addLayout(generalSettingsLayout);
    mainSettingsLayout->addLayout(specificEditorsLayout);

    QVBoxLayout* controlButtonsLayout = new QVBoxLayout();
    controlButtonsLayout->addStretch();
    if (createMode)
    {
        controlButtonsLayout->addWidget(removeButton);
        controlButtonsLayout->addWidget(duplicateButton);
    }
    controlButtonsLayout->addWidget(saveButton);
    controlButtonsLayout->addStretch();

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addLayout(upDownButtonsLayout);
    layout->addLayout(mainSettingsLayout);
    layout->addLayout(controlButtonsLayout);

    setLayout(layout);

    processTypeChange();
    processBehaviorChange();
}

QUuid ColumnSettingsWidget::id() const
{
    return widgetId;
}

QString ColumnSettingsWidget::name() const
{
    return nameEdit->text();
}

QJsonObject ColumnSettingsWidget::getSettings() const
{
    QJsonObject result;
    result.insert(labelTag, Utils::transliterate(nameEdit->text()));
    result.insert(nameTag, nameEdit->text());
    ColumnType::Value type =
        ColumnType::Value(typeComboBox->currentData().toInt());
    result.insert(typeTag, ColumnType::toString(type));
    if (specificSettingsEditors.contains(type))
    {
        QJsonObject specificSettings =
            specificSettingsEditors.value(type)->getSettings();
        auto keys = specificSettings.keys();
        for (const auto& key: keys)
        {
            result.insert(key, specificSettings.value(key));
        }
    }
    return result;
}

void ColumnSettingsWidget::setSettings(const QJsonObject& settings)
{
    if (settings.contains(nameTag))
    {
        nameEdit->setText(settings.value(nameTag).toString());
    }
    if (settings.contains(typeTag))
    {
        auto type = ColumnType::fromString(settings.value(typeTag).toString());
        typeComboBox->setCurrentIndex(typeComboBox->findData(type));
        if (specificSettingsEditors.contains(type))
        {
            specificSettingsEditors.value(type)->setSettings(settings);
        }
    }
}

void ColumnSettingsWidget::highlightName(bool highlight)
{
    nameEdit->setStyleSheet(
        QString("QLineEdit { background-color: %1; }").arg(highlight ? "red": "white"));
}

QString ColumnSettingsWidget::validate()
{
    ColumnType::Value type =
        ColumnType::Value(typeComboBox->currentData().toInt());
    if (specificSettingsEditors.contains(type))
    {
        return specificSettingsEditors[type]->validate();
    }
    return QString();
}

void ColumnSettingsWidget::processTypeChange()
{
    ColumnType::Value type =
        ColumnType::Value(typeComboBox->currentData().toInt());
    if (type == ColumnType::INT || type == ColumnType::DOUBLE)
    {
        if (!specificSettingsEditors.contains(type))
        {
            IntColumnSettingsWidget* intColumnSettings =
                new IntColumnSettingsWidget();
            specificSettingsEditors[type] = intColumnSettings;
            specificEditorsLayout->addWidget(intColumnSettings);
        }
    }
    else if (type == ColumnType::STRING)
    {
        if (!specificSettingsEditors.contains(type))
        {
            StringColumnSettingsWidget* stringColumnSettings =
                new StringColumnSettingsWidget();
            specificSettingsEditors[type] = stringColumnSettings;
            specificEditorsLayout->addWidget(stringColumnSettings);
        }
    }

    auto types = specificSettingsEditors.keys();
    for (const auto& key: types)
    {
        specificSettingsEditors[key]->setVisible(key == type);
    }

    behaviorLabel->setVisible(type == ColumnType::STRING);
    behaviorComboBox->setVisible(type == ColumnType::STRING);
}

void ColumnSettingsWidget::processBehaviorChange()
{
    if (specificSettingsEditors.contains(ColumnType::STRING))
    {
        StringColumnSettingsWidget* widget =
            qobject_cast<StringColumnSettingsWidget*>(
                specificSettingsEditors.value(ColumnType::STRING));
        widget->setBehaviorType(
            BehaviorType::Value(behaviorComboBox->currentData().toInt()));
    }
}

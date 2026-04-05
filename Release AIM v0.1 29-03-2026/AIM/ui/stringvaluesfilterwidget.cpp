#include "stringvaluesfilterwidget.h"

#include <QJsonArray>
#include <QToolTip>
#include <QVBoxLayout>

namespace
{

const QString valuesTag = "values";

} // namespace

StringValuesFilterWidget::StringValuesFilterWidget(
    const QList<StringValueParams>& values,
    const QString& tag,
    QWidget* parent) :
  FilterWidget(tag, parent)
{
    for (const auto& value: values)
    {
        QCheckBox* valueCheckBox = new QCheckBox(value.text);
        QToolButton* helpButton = nullptr;
        if (!value.description.isEmpty())
        {
            helpButton = new QToolButton();
            QIcon helpIcon(":/icons/help.svg");
            helpButton->setIcon(helpIcon);
            helpButton->setIconSize(helpButton->sizeHint());
            helpButton->setStyleSheet(
                "QToolButton { border: none; } "
                "QToolButton:hover { background-color: #aaaaaa; }");
            QObject::connect(
                helpButton,
                &QToolButton::clicked,
                helpButton,
                [helpButton, description = value.description]() {
                    auto globalPos = helpButton->mapToGlobal(QPoint(0, 0));
                    QToolTip::showText(
                        QPoint(globalPos.x(), globalPos.y() + 10), description);
                });
        }
        valuesCheckBoxes.append({valueCheckBox, helpButton});
    }

    QVBoxLayout* layout = new QVBoxLayout();
    for (const auto& valueCheckBox: qAsConst(valuesCheckBoxes))
    {
        if (valueCheckBox.second != nullptr)
        {
            QHBoxLayout* valueLayout = new QHBoxLayout();
            valueLayout->addWidget(valueCheckBox.first);
            valueLayout->addWidget(valueCheckBox.second);
            layout->addLayout(valueLayout);
        }
        else
        {
            layout->addWidget(valueCheckBox.first);
        }
    }
    setLayout(layout);
}

QJsonObject StringValuesFilterWidget::getFilter() const
{
    QJsonArray values;
    for (auto& valueCheckBox: valuesCheckBoxes)
    {
        if (valueCheckBox.first->isChecked())
        {
            values.append(valueCheckBox.first->text());
        }
    }
    if (!values.isEmpty())
    {
        return QJsonObject{{valuesTag, values}};
    }
    else
    {
        return QJsonObject{};
    }
}

QString StringValuesFilterWidget::getFilterString() const
{
    QStringList values;
    for (auto& valueCheckBox: valuesCheckBoxes)
    {
        if (valueCheckBox.first->isChecked())
        {
            values.append(valueCheckBox.first->text());
        }
    }
    return values.join(", ");
}

void StringValuesFilterWidget::setFilter(const FilterParams& params)
{
    for (auto& valueCheckBox: valuesCheckBoxes)
    {
        valueCheckBox.first->setChecked(
            params.values.contains(valueCheckBox.first->text()));
    }
    emit updated();
}

void StringValuesFilterWidget::reset()
{
    for (auto& valueCheckBox: valuesCheckBoxes)
    {
        valueCheckBox.first->setChecked(false);
    }
    emit updated();
}

bool StringValuesFilterWidget::isEmpty() const
{
    return std::all_of(
        valuesCheckBoxes.begin(),
        valuesCheckBoxes.end(),
        [](const auto& valueCheckBox) {
            return !valueCheckBox.first->isChecked();
        });
}

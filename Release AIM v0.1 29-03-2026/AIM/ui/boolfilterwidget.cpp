#include "boolfilterwidget.h"

#include "boolstatus.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QVBoxLayout>

namespace
{

const QString valuesTag = "values";

} // namespace

BoolFilterWidget::BoolFilterWidget(const QString& tag, QWidget* parent) :
  FilterWidget(tag, parent)
{
    yesCheckBox = new QCheckBox(QCoreApplication::translate("Filters", "Yes"));
    noCheckBox = new QCheckBox(QCoreApplication::translate("Filters", "No"));

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(yesCheckBox);
    layout->addWidget(noCheckBox);
    setLayout(layout);
}

QJsonObject BoolFilterWidget::getFilter() const
{
    if (!isEmpty())
    {
        BoolStatus::Value status =
            (yesCheckBox->isChecked() && noCheckBox->isChecked()) ?
                BoolStatus::ALL :
                (yesCheckBox->isChecked() ? BoolStatus::YES : BoolStatus::NO);
        return QJsonObject{
            {valuesTag, QJsonArray() << BoolStatus::toString(status)}};
    }
    else
    {
        return QJsonObject{};
    }
}

QString BoolFilterWidget::getFilterString() const
{
    QStringList values;
    if (yesCheckBox->isChecked())
    {
        values.append(yesCheckBox->text());
    }
    if (noCheckBox->isChecked())
    {
        values.append(noCheckBox->text());
    }
    return values.join(", ");
}

void BoolFilterWidget::setFilter(const FilterParams& params)
{
    if (!params.values.isEmpty())
    {
        BoolStatus::Value status =
            BoolStatus::fromString(params.values.first());
        yesCheckBox->setChecked(
            status == BoolStatus::YES || status == BoolStatus::ALL);
        noCheckBox->setChecked(
            status == BoolStatus::NO || status == BoolStatus::ALL);
    }
    else
    {
        reset();
    }
}

void BoolFilterWidget::reset()
{
    yesCheckBox->setChecked(false);
    noCheckBox->setChecked(false);
}

bool BoolFilterWidget::isEmpty() const
{
    return !yesCheckBox->isChecked() && !noCheckBox->isChecked();
}

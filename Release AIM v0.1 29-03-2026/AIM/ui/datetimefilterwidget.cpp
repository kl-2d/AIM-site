#include "datetimefilterwidget.h"

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace
{

const QString datesTag = "dates";
const QString minTag = "min";
const QString maxTag = "max";

} // namespace

DatetimeFilterWidget::DatetimeFilterWidget(const QString& tag, QWidget* parent) :
  FilterWidget(tag, parent)
{
    minEditor = new QDateEdit();
    maxEditor = new QDateEdit();
    minEditor->setEnabled(false);
    maxEditor->setEnabled(false);
    minEditor->setCalendarPopup(true);
    maxEditor->setCalendarPopup(true);
    minEditor->setDate(QDate::currentDate());
    maxEditor->setDate(QDate::currentDate());

    minEnabledCheckBox = new QCheckBox();
    maxEnabledCheckBox = new QCheckBox();
    QObject::connect(
        minEnabledCheckBox,
        &QCheckBox::stateChanged,
        this,
        [this](const auto& checkState) {
            minEditor->setEnabled(checkState == Qt::Checked);
        });
    QObject::connect(
        maxEnabledCheckBox,
        &QCheckBox::stateChanged,
        this,
        [this](const auto& checkState) {
            maxEditor->setEnabled(checkState == Qt::Checked);
        });
    minEnabledCheckBox->setChecked(false);
    maxEnabledCheckBox->setChecked(false);

    QLabel* minLabel = new QLabel();
    minLabel->setText(QCoreApplication::translate("Filters", "From") + ":");
    QLabel* maxLabel = new QLabel();
    maxLabel->setText(QCoreApplication::translate("Filters", "To") + ":");

    QHBoxLayout* minlayout = new QHBoxLayout();
    minlayout->addWidget(minEnabledCheckBox);
    minlayout->addWidget(minLabel);
    minlayout->addWidget(minEditor);
    QHBoxLayout* maxlayout = new QHBoxLayout();
    maxlayout->addWidget(maxEnabledCheckBox);
    maxlayout->addWidget(maxLabel);
    maxlayout->addWidget(maxEditor);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(minlayout);
    layout->addLayout(maxlayout);
    setLayout(layout);
}

QJsonObject DatetimeFilterWidget::getFilter() const
{
    if (!minEnabledCheckBox->isChecked() && !maxEnabledCheckBox->isChecked())
    {
        return QJsonObject{};
    }
    QString min = minEnabledCheckBox->isChecked() ?
                      minEditor->date().toString("yyyy-MM-dd") :
                      QString("1900-01-01");
    QString max = maxEnabledCheckBox->isChecked() ?
                      maxEditor->date().toString("yyyy-MM-dd") :
                      QString("2100-01-01");
    QJsonObject dates = QJsonObject{{minTag, min}, {maxTag, max}};
    return QJsonObject{{datesTag, dates}};
}

QString DatetimeFilterWidget::getFilterString() const
{
    QString result;
    if (minEnabledCheckBox->isChecked())
    {
        result.append(QString("%1 %2 ").arg(
            QCoreApplication::translate("Filters", "from"),
            minEditor->date().toString("dd.MM.yyyy")));
    }
    if (maxEnabledCheckBox->isChecked())
    {
        result.append(QString("%1 %2").arg(
            QCoreApplication::translate("Filters", "to"),
            maxEditor->date().toString("dd.MM.yyyy")));
    }
    return result;
}

void DatetimeFilterWidget::setFilter(const FilterParams& params)
{
    if (params.dates.has_value())
    {
        auto min = QDate::fromString(params.dates->first, "yyyy-MM-dd");
        if (min > QDate(1900, 1, 1))
        {
            minEditor->setDate(min);
        }
        else
        {
            minEditor->setDate(QDate::currentDate());
        }
        auto max = QDate::fromString(params.dates->second, "yyyy-MM-dd");
        if (max < QDate(2100, 1, 1))
        {
            maxEditor->setDate(max);
        }
        else
        {
            maxEditor->setDate(QDate::currentDate());
        }
        emit updated();
    }
    else
    {
        reset();
    }
}

void DatetimeFilterWidget::reset()
{
    minEditor->setDate(QDate::currentDate());
    maxEditor->setDate(QDate::currentDate());
    minEnabledCheckBox->setChecked(false);
    maxEnabledCheckBox->setChecked(false);
    emit updated();
}

bool DatetimeFilterWidget::isEmpty() const
{
    return !minEnabledCheckBox->isChecked() && !maxEnabledCheckBox->isChecked();
}

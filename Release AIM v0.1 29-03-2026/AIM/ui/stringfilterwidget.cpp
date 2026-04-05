#include "stringfilterwidget.h"

#include <QCoreApplication>
#include <QVBoxLayout>

namespace
{

const QString textTag = "text";

} // namespace

StringFilterWidget::StringFilterWidget(const QString& tag, QWidget* parent) :
  FilterWidget(tag, parent)
{
    lineEdit = new QLineEdit();

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(lineEdit);
    setLayout(layout);
}

QJsonObject StringFilterWidget::getFilter() const
{
    QString text = lineEdit->text();
    if (!text.isEmpty())
    {
        return QJsonObject{{textTag, text}};
    }
    else
    {
        return QJsonObject{};
    }
}

QString StringFilterWidget::getFilterString() const
{
    QString text = lineEdit->text();
    if (!text.isEmpty())
    {
        return QCoreApplication::translate("Filters", "contains \"%1\"")
            .arg(text);
    }
    else
    {
        return QString();
    }
}

void StringFilterWidget::setFilter(const FilterParams& params)
{
    if (!params.text.isEmpty())
    {
        lineEdit->setText(params.text);
    }
    else
    {
        lineEdit->clear();
    }
    emit updated();
}

void StringFilterWidget::reset()
{
    lineEdit->clear();
    emit updated();
}

bool StringFilterWidget::isEmpty() const
{
    return lineEdit->text().isEmpty();
}

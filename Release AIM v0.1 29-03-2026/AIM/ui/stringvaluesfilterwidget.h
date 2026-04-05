#ifndef STRINGVALUESFILTERWIDGET_H
#define STRINGVALUESFILTERWIDGET_H

#include "datacolumnsmodel.h"
#include "filterwidget.h"

#include <QCheckBox>
#include <QToolButton>

class StringValuesFilterWidget : public FilterWidget
{
    Q_OBJECT

public:
    StringValuesFilterWidget(
        const QList<StringValueParams>& values,
        const QString& tag,
        QWidget* parent = nullptr);

    QJsonObject getFilter() const override;
    QString getFilterString() const override;
    void setFilter(const FilterParams& params) override;
    void reset() override;
    bool isEmpty() const override;

private:
    QList<QPair<QCheckBox*, QToolButton*>> valuesCheckBoxes;
};

#endif // STRINGVALUESFILTERWIDGET_H

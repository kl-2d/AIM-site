#ifndef DATETIMEFILTERWIDGET_H
#define DATETIMEFILTERWIDGET_H

#include "filterwidget.h"

#include <QCheckBox>
#include <QDateEdit>

class DatetimeFilterWidget : public FilterWidget
{
    Q_OBJECT

public:
    DatetimeFilterWidget(const QString& tag, QWidget* parent = nullptr);

    QJsonObject getFilter() const override;
    QString getFilterString() const override;
    void setFilter(const FilterParams& params) override;
    void reset() override;
    bool isEmpty() const override;

private:
    QCheckBox* minEnabledCheckBox;
    QCheckBox* maxEnabledCheckBox;
    QDateEdit* minEditor;
    QDateEdit* maxEditor;
};

#endif // DATETIMEFILTERWIDGET_H

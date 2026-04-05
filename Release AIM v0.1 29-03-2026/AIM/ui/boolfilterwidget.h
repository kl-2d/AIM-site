#ifndef BOOLFILTERWIDGET_H
#define BOOLFILTERWIDGET_H

#include "filterwidget.h"

#include <QCheckBox>

class BoolFilterWidget : public FilterWidget
{
public:
    BoolFilterWidget(const QString& tag, QWidget* parent = nullptr);

    QJsonObject getFilter() const override;
    QString getFilterString() const override;
    void setFilter(const FilterParams& params) override;
    void reset() override;
    bool isEmpty() const override;

private:
    QCheckBox* yesCheckBox;
    QCheckBox* noCheckBox;
};

#endif // BOOLFILTERWIDGET_H

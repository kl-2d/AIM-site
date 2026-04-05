#ifndef STRINGFILTERWIDGET_H
#define STRINGFILTERWIDGET_H

#include "filterwidget.h"

#include <QLineEdit>

class StringFilterWidget : public FilterWidget
{
    Q_OBJECT

public:
    StringFilterWidget(const QString& tag, QWidget* parent = nullptr);

    QJsonObject getFilter() const override;
    QString getFilterString() const override;
    void setFilter(const FilterParams& params) override;
    void reset() override;
    bool isEmpty() const override;

private:
    QLineEdit* lineEdit;
};

#endif // STRINGFILTERWIDGET_H

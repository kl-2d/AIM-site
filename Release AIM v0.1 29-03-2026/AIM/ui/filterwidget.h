#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include "filterparams.h"

#include <QJsonObject>
#include <QWidget>

class FilterWidget : public QWidget
{
    Q_OBJECT

public:
    FilterWidget(const QString& tag, QWidget* parent = nullptr);

    QString getTag() const;
    virtual QJsonObject getFilter() const = 0;
    virtual QString getFilterString() const = 0;
    virtual void setFilter(const FilterParams& params) = 0;
    virtual void reset() = 0;
    virtual bool isEmpty() const = 0;

signals:
    void updated();

private:
    QString tag;
};

#endif // FILTERWIDGET_H

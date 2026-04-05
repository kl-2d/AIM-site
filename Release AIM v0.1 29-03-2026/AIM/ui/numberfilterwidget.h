#ifndef NUMBERFILTERWIDGET_H
#define NUMBERFILTERWIDGET_H

#include "filtervalueedit.h"
#include "filterwidget.h"

#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>

class NumberFilterWidget : public FilterWidget
{
    Q_OBJECT

public:
    NumberFilterWidget(const QString& tag, QWidget* parent = nullptr);

    QJsonObject getFilter() const override;
    QString getFilterString() const override;
    void setFilter(const FilterParams& params) override;
    void reset() override;
    bool isEmpty() const override;

private slots:
    void addInterval();
    void removeInterval(const QString& id);

private:
    struct IntervalEditor
    {
        QString id;
        FilterValueEdit* minEditor;
        FilterValueEdit* maxEditor;
        QToolButton* removeButton;
    };
    QList<IntervalEditor> editors;

    QPushButton* addButton;

    QVBoxLayout* editorsLayout;
};

#endif // NUMBERFILTERWIDGET_H

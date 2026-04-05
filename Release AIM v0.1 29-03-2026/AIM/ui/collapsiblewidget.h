#ifndef COLLAPSIBLEWIDGET_H
#define COLLAPSIBLEWIDGET_H

#include "filterwidget.h"

#include <QLabel>
#include <QWidget>

class CollapsibleWidget : public QWidget
{
public:
    CollapsibleWidget(
        const QString& title, FilterWidget* content, QWidget* parent = nullptr);

    void mouseReleaseEvent(QMouseEvent* event) override;

    void setCollapsed(bool value);

private:
    void update();

private:
    QLabel* titleLabel;
    QLabel* collapseIconLabel;
    QLabel* highlightIconLabel;
    FilterWidget* contentWidget;

    bool collapsed = true;
};

#endif // COLLAPSIBLEWIDGET_H

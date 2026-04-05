#include "filterwidget.h"

FilterWidget::FilterWidget(const QString& tag, QWidget* parent) :
  QWidget(parent)
{
    this->tag = tag;
}

QString FilterWidget::getTag() const
{
    return tag;
}

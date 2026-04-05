#include "tableheaderpreviewwidget.h"

TableHeaderPreviewWidget::TableHeaderPreviewWidget(QWidget* parent) :
  QWidget(parent)
{
    headersLayout = new QHBoxLayout();
    headersLayout->setSpacing(0);
    setLayout(headersLayout);
}

void TableHeaderPreviewWidget::setHeaders(const QList<QString>& headers)
{
    while (headerLabels.size() > headers.size())
    {
        headersLayout->removeItem(headersLayout->itemAt(0));
        QLabel* label = headerLabels.takeAt(0);
        delete label;
    }
    while (headerLabels.size() < headers.size())
    {
        QLabel* label = new QLabel();
        label->setMinimumHeight(40);
        label->setStyleSheet(
            "QLabel { background-color: white; border: 3px solid black; }");
        headerLabels.append(label);
        headersLayout->addWidget(label);
    }
    for (int i = 0; i < headers.size(); i++)
    {
        headerLabels[i]->setText(headers.at(i));
    }
}

#ifndef TABLEHEADERPREVIEWWIDGET_H
#define TABLEHEADERPREVIEWWIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QList>

class TableHeaderPreviewWidget : public QWidget
{
    Q_OBJECT

public:
    TableHeaderPreviewWidget(QWidget* parent = nullptr);

    void setHeaders(const QList<QString>& headers);

private:
    QList<QLabel*> headerLabels;
    QHBoxLayout* headersLayout;
};

#endif // TABLEHEADERPREVIEWWIDGET_H

#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableView>

class TableWidget : public QTableView
{
    Q_OBJECT

public:
    TableWidget(QWidget* parent = nullptr);

private:
    bool eventFilter(QObject* obj, QEvent* event) override;

signals:
    void deleteRequested(QModelIndexList selected);
};

#endif // TABLEWIDGET_H

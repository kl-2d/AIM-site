#include "tablewidget.h"

#include <QEvent>
#include <QHeaderView>
#include <QKeyEvent>

TableWidget::TableWidget(QWidget* parent) : QTableView(parent)
{
    installEventFilter(this);
    /*QObject::disconnect(horizontalHeader(), nullptr, nullptr, nullptr);
    QObject::connect(
        horizontalHeader(),
        &QHeaderView::sectionClicked,
        this,
        [this](int section) {
            if (!horizontalHeader()->isSortIndicatorShown())
            {
                setSortingEnabled(true);
                horizontalHeader()->setSortIndicatorShown(true);
            }
            else if (section == horizontalHeader()->sortIndicatorSection())
            {
            }
            else
            {
                horizontalHeader()->setSortIndicatorShown(false);
                setSortingEnabled(false);
            }
        });*/
}

bool TableWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (keyEvent->modifiers().testFlag(Qt::ControlModifier))
        {
            if (keyEvent->key() == Qt::Key_C)
            {
                // TODO: Copy
                return true;
            }
            if (keyEvent->key() == Qt::Key_V)
            {
                // TODO: Paste
                return true;
            }
        }
        else if (keyEvent->key() == Qt::Key_Delete)
        {
            emit deleteRequested(selectionModel()->selectedIndexes());
            return true;
        }
        else if (
            keyEvent->key() == Qt::Key_Return
            || keyEvent->key() == Qt::Key_Enter)
        {
            // TODO: Edit
            return true;
        }
    }
    return QTableView::eventFilter(obj, event);
}

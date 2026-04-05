#include "numberfilterwidget.h"

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QLabel>
#include <QUuid>
#include <QVBoxLayout>

namespace
{

const QString intervalsTag = "intervals";
const QString minTag = "min";
const QString maxTag = "max";

} // namespace

NumberFilterWidget::NumberFilterWidget(const QString& tag, QWidget* parent) :
  FilterWidget(tag, parent)
{
    editorsLayout = new QVBoxLayout();

    addInterval();
    editors.first().removeButton->setVisible(false);

    addButton = new QPushButton(QCoreApplication::translate("Filters", "Add"));
    addButton->setMaximumWidth(150);

    QObject::connect(
        addButton,
        &QPushButton::clicked,
        this,
        &NumberFilterWidget::addInterval);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(editorsLayout);
    layout->addWidget(addButton);

    setLayout(layout);
}

QJsonObject NumberFilterWidget::getFilter() const
{
    if (isEmpty())
    {
        return QJsonObject{};
    }
    QJsonArray intervals;
    for (const auto& editor: editors)
    {
        if (editor.minEditor->text().isEmpty()
            && editor.maxEditor->text().isEmpty())
        {
            continue;
        }
        double min = editor.minEditor->text().isEmpty() ?
                         -1e12 :
                         editor.minEditor->text().toDouble();
        double max = editor.maxEditor->text().isEmpty() ?
                         1e12 :
                         editor.maxEditor->text().toDouble();
        intervals.append(QJsonObject{{minTag, min}, {maxTag, max}});
    }
    return QJsonObject{{intervalsTag, intervals}};
}

QString NumberFilterWidget::getFilterString() const
{
    QStringList intervalsString;
    QJsonArray intervals = getFilter().value(intervalsTag).toArray();
    for (const auto& interval: intervals)
    {
        intervalsString.append(QString("%1 - %2").arg(
            QString::number(interval.toObject().value(minTag).toDouble()),
            QString::number(interval.toObject().value(maxTag).toDouble())));
    }
    return intervalsString.join("; ");
}

void NumberFilterWidget::setFilter(const FilterParams& params)
{
    reset();
    if (!params.intervals.isEmpty())
    {
        for (int i = 0; i < params.intervals.size(); i++)
        {
            if (i > 0)
            {
                addInterval();
            }
            const auto& interval = params.intervals.at(i);
            if (interval.first > -1e12)
            {
                editors.last().minEditor->setText(
                    QString::number(interval.first));
            }
            else
            {
                editors.last().minEditor->clear();
            }
            if (interval.second < 1e12)
            {
                editors.last().maxEditor->setText(
                    QString::number(interval.second));
            }
            else
            {
                editors.last().maxEditor->clear();
            }
        }
        emit updated();
    }
}

void NumberFilterWidget::reset()
{
    while (editors.size() > 1)
    {
        delete editors[1].minEditor;
        delete editors[1].maxEditor;
        delete editors[1].removeButton;
        editors.removeAt(1);
        editorsLayout->removeItem(editorsLayout->itemAt(1));
    }
    editors.first().minEditor->clear();
    editors.first().maxEditor->clear();
    editors.first().removeButton->setVisible(false);
    emit updated();
}

bool NumberFilterWidget::isEmpty() const
{
    for (const auto& editor: editors)
    {
        if (!editor.minEditor->text().isEmpty() || !editor.maxEditor->text().isEmpty())
        {
            return false;
        }
    }
    return true;
}

void NumberFilterWidget::addInterval()
{
    QString id = QUuid::createUuid().toString();
    FilterValueEdit* minEditor = new FilterValueEdit(
        QCoreApplication::translate("Filters", "From"), FilterValueEdit::INT);
    FilterValueEdit* maxEditor = new FilterValueEdit(
        QCoreApplication::translate("Filters", "To"), FilterValueEdit::INT);
    QToolButton* removeButton = new QToolButton();
    QIcon removeIcon(":/icons/close.svg");
    removeButton->setIcon(removeIcon);

    QObject::connect(removeButton, &QToolButton::clicked, this, [this, id]() {
        removeInterval(id);
    });

    editors.append(IntervalEditor{id, minEditor, maxEditor, removeButton});

    QHBoxLayout* editorLayout = new QHBoxLayout();
    editorLayout->addWidget(minEditor);
    editorLayout->addWidget(maxEditor);
    editorLayout->addWidget(removeButton);

    editorsLayout->addLayout(editorLayout);

    if (editors.size() > 1)
    {
        editors[0].removeButton->setVisible(true);
    }
}

void NumberFilterWidget::removeInterval(const QString& id)
{
    for (int i = 0; i < editors.size(); i++)
    {
        if (editors.at(i).id == id)
        {
            delete editors[i].minEditor;
            delete editors[i].maxEditor;
            delete editors[i].removeButton;
            editors.removeAt(i);
            editorsLayout->removeItem(editorsLayout->itemAt(i));

            if (editors.size() == 1)
            {
                editors[0].removeButton->setVisible(false);
            }

            break;
        }
    }
}

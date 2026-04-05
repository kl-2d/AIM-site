#include "intcolumnsettingswidget.h"

#include "columntype.h"

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QLabel>
#include <QRegularExpression>
#include <QToolButton>
#include <QUuid>
#include <QVBoxLayout>

namespace
{

const QString intervalsTag = "intervals";
const QString minTag = "min";
const QString maxTag = "max";

} // namespace

IntColumnSettingsWidget::IntColumnSettingsWidget(QWidget* parent) :
  ColumnSpecificSettingsWidget(parent)
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
        &IntColumnSettingsWidget::addInterval);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(editorsLayout);
    layout->addWidget(addButton);

    setLayout(layout);
}

QJsonObject IntColumnSettingsWidget::getSettings() const
{
    QJsonArray intervals;
    bool hasIntervals = false;
    for (const auto& editor: editors)
    {
        if (!editor.minEditor->text().isEmpty()
            || !editor.maxEditor->text().isEmpty())
        {
            hasIntervals = true;
            break;
        }
    }
    if (hasIntervals)
    {
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
    }
    QJsonObject result;
    if (hasIntervals)
    {
        result.insert(intervalsTag, intervals);
    }
    return result;
}

void IntColumnSettingsWidget::setSettings(const QJsonObject& settings)
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

    if (settings.contains(intervalsTag))
    {
        QJsonArray intervals = settings.value(intervalsTag).toArray();
        for (int i = 0; i < intervals.size(); i++)
        {
            if (i > 0)
            {
                addInterval();
            }

            QJsonObject intervalObj = intervals.at(i).toObject();
            double min = intervalObj.value(minTag).toDouble();
            double max = intervalObj.value(maxTag).toDouble();
            if (min > -1e12)
            {
                editors.last().minEditor->setText(QString::number(min));
            }
            else
            {
                editors.last().minEditor->clear();
            }
            if (max < 1e12)
            {
                editors.last().maxEditor->setText(QString::number(max));
            }
            else
            {
                editors.last().maxEditor->clear();
            }
        }
    }
}

QString IntColumnSettingsWidget::validate()
{
    return QString();
}

void IntColumnSettingsWidget::addInterval()
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

void IntColumnSettingsWidget::removeInterval(const QString& id)
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

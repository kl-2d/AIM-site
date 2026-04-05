#include "stringcolumnsettingswidget.h"

#include <QCoreApplication>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QToolButton>
#include <QUuid>

namespace
{

const QString valuesTag = "values";
const QString textTag = "text";
const QString codeTag = "code";
const QString descriptionTag = "description";

} // namespace

void highlight(QLineEdit* edit, bool highlight)
{
    edit->setStyleSheet(
        QString("QLineEdit { background-color: %1; }").arg(highlight ? "red": "white"));
}

StringColumnSettingsWidget::StringColumnSettingsWidget(QWidget* parent) :
  ColumnSpecificSettingsWidget(parent)
{
    editorsLayout = new QVBoxLayout();

    addValue();
    editors.first().removeButton->setVisible(false);

    addButton = new QPushButton(QCoreApplication::translate("Filters", "Add"));
    addButton->setMaximumWidth(150);

    QObject::connect(
        addButton,
        &QPushButton::clicked,
        this,
        &StringColumnSettingsWidget::addValue);

    QLabel* valueLabel =
        new QLabel(QCoreApplication::translate("TableSettings", "Value"));
    codeLabel =
        new QLabel(QCoreApplication::translate("TableSettings", "Code"));
    QLabel* commentLabel =
        new QLabel(QCoreApplication::translate("TableSettings", "Comment"));

    QHBoxLayout* titleLayout = new QHBoxLayout;
    titleLayout->addWidget(valueLabel);
    titleLayout->addWidget(codeLabel);
    titleLayout->addWidget(commentLabel);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(titleLayout);
    layout->addLayout(editorsLayout);
    layout->addWidget(addButton);

    setLayout(layout);
}

QJsonObject StringColumnSettingsWidget::getSettings() const
{
    if (behaviorType == BehaviorType::ANY_VALUES)
    {
        return QJsonObject{};
    }

    QJsonArray values;
    bool hasValues = false;
    for (const auto& editor: editors)
    {
        if (!editor.valueEditor->text().isEmpty())
        {
            hasValues = true;
            break;
        }
    }
    if (hasValues)
    {
        for (const auto& editor: editors)
        {
            if (editor.valueEditor->text().isEmpty())
            {
                continue;
            }
            QString text = editor.valueEditor->text();
            QString code = behaviorType == BehaviorType::ENCODED_VALUES_LIST ?
                               editor.codeEditor->text() :
                               QString();
            QString description = editor.commentEditor->text();
            values.append(QJsonObject{
                {textTag, text},
                {codeTag, code},
                {descriptionTag, description}});
        }
    }
    QJsonObject result;
    if (hasValues)
    {
        result.insert(valuesTag, values);
    }
    return result;
}

void StringColumnSettingsWidget::setSettings(const QJsonObject& settings)
{
    while (editors.size() > 1)
    {
        delete editors[1].valueEditor;
        delete editors[1].codeEditor;
        delete editors[1].commentEditor;
        delete editors[1].removeButton;
        editors.removeAt(1);
        editorsLayout->removeItem(editorsLayout->itemAt(1));
    }
    editors.first().valueEditor->clear();
    editors.first().codeEditor->clear();
    editors.first().commentEditor->clear();
    editors.first().removeButton->setVisible(false);

    if (settings.contains(valuesTag))
    {
        QJsonArray values = settings.value(valuesTag).toArray();
        BehaviorType::Value type = BehaviorType::VALUES_LIST;
        for (int i = 0; i < values.size(); i++)
        {
            if (i > 0)
            {
                addValue();
            }

            QJsonObject valueObj = values.at(i).toObject();
            QString text = valueObj.value(textTag).toString();
            QString code = valueObj.value(codeTag).toString();
            QString description = valueObj.value(descriptionTag).toString();

            if (!code.isEmpty())
            {
                type = BehaviorType::ENCODED_VALUES_LIST;
                editors.last().codeEditor->setText(code);
            }
            editors.last().valueEditor->setText(text);
            editors.last().commentEditor->setText(description);
        }
        setBehaviorType(type);
    }
    else
    {
        setBehaviorType(BehaviorType::ANY_VALUES);
    }
}

QString StringColumnSettingsWidget::validate()
{
    QString errors;
    if (behaviorType == BehaviorType::ENCODED_VALUES_LIST)
    {
        for (const auto& editor: editors)
        {
            highlight(editor.valueEditor, false);
            highlight(editor.codeEditor, false);
            if (editor.valueEditor->text().isEmpty() && !editor.codeEditor->text().isEmpty())
            {
                highlight(editor.valueEditor, true);
                errors.append(QCoreApplication::translate("TableSettings", "Missing value for code %1").arg(editor.codeEditor->text()) + "\n");
            }
            if (!editor.valueEditor->text().isEmpty() && editor.codeEditor->text().isEmpty())
            {
                highlight(editor.codeEditor, true);
                errors.append(QCoreApplication::translate("TableSettings", "Missing code for value %1").arg(editor.valueEditor->text()) + "\n");
            }
        }
    }
    return errors;
}

void StringColumnSettingsWidget::setBehaviorType(BehaviorType::Value type)
{
    behaviorType = type;
    if (type == BehaviorType::ANY_VALUES)
    {
        setVisible(false);
    }
    else
    {
        setVisible(true);
        codeLabel->setVisible(type == BehaviorType::ENCODED_VALUES_LIST);
        for (auto& editor: editors)
        {
            editor.codeEditor->setVisible(
                type == BehaviorType::ENCODED_VALUES_LIST);
        }
    }
}

BehaviorType::Value StringColumnSettingsWidget::getBehaviorType() const
{
    return behaviorType;
}

void StringColumnSettingsWidget::addValue()
{
    QString id = QUuid::createUuid().toString();
    QLineEdit* valueEditor = new QLineEdit();
    QLineEdit* descriptionEditor = new QLineEdit();
    QLineEdit* commentEditor = new QLineEdit();
    QToolButton* removeButton = new QToolButton();
    QIcon removeIcon(":/icons/close.svg");
    removeButton->setIcon(removeIcon);

    QObject::connect(removeButton, &QToolButton::clicked, this, [this, id]() {
        removeValue(id);
    });

    editors.append(ValueEditor{
        id, valueEditor, descriptionEditor, commentEditor, removeButton});

    QHBoxLayout* editorLayout = new QHBoxLayout();
    editorLayout->addWidget(valueEditor);
    editorLayout->addWidget(descriptionEditor);
    editorLayout->addWidget(commentEditor);
    editorLayout->addWidget(removeButton);

    editorsLayout->addLayout(editorLayout);

    if (editors.size() > 1)
    {
        editors[0].removeButton->setVisible(true);
    }
    descriptionEditor->setVisible(behaviorType == BehaviorType::ENCODED_VALUES_LIST);
}

void StringColumnSettingsWidget::removeValue(const QString& id)
{
    for (int i = 0; i < editors.size(); i++)
    {
        if (editors.at(i).id == id)
        {
            delete editors[i].valueEditor;
            delete editors[i].codeEditor;
            delete editors[i].commentEditor;
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

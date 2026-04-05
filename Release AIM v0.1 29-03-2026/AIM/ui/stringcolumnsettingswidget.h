#ifndef STRINGCOLUMNSETTINGSWIDGET_H
#define STRINGCOLUMNSETTINGSWIDGET_H

#include "behaviortype.h"
#include "columnspecificsettingswidget.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class StringColumnSettingsWidget : public ColumnSpecificSettingsWidget
{
    Q_OBJECT

public:
    StringColumnSettingsWidget(QWidget* parent = nullptr);

    QJsonObject getSettings() const override;
    void setSettings(const QJsonObject& settings) override;
    QString validate() override;

    void setBehaviorType(BehaviorType::Value type);
    BehaviorType::Value getBehaviorType() const;

private slots:
    void addValue();
    void removeValue(const QString& id);

private:
    QLabel* codeLabel;

    struct ValueEditor
    {
        QString id;
        QLineEdit* valueEditor;
        QLineEdit* codeEditor;
        QLineEdit* commentEditor;
        QToolButton* removeButton;
    };
    QList<ValueEditor> editors;
    QPushButton* addButton;
    QVBoxLayout* editorsLayout;

    BehaviorType::Value behaviorType;
};

#endif // STRINGCOLUMNSETTINGSWIDGET_H

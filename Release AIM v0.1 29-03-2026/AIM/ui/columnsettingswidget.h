#ifndef COLUMNSETTINGSWIDGET_H
#define COLUMNSETTINGSWIDGET_H

#include "columnspecificsettingswidget.h"
#include "columntype.h"

#include <QComboBox>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QPushButton>
#include <QToolButton>
#include <QUuid>
#include <QVBoxLayout>

class ColumnSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    ColumnSettingsWidget(bool createMode = true, QWidget* parent = nullptr);

    QUuid id() const;
    QString name() const;

    QJsonObject getSettings() const;
    void setSettings(const QJsonObject& settings);

    void highlightName(bool highlight);
    QString validate();

signals:
    void upButtonClicked(QUuid id);
    void downButtonClicked(QUuid id);
    void removeButtonClicked(QUuid id);
    void duplicateButtonClicked(QUuid id);
    void saveButtonClicked(QUuid id);
    void nameChanged(QUuid id, QString name);

private slots:
    void processTypeChange();
    void processBehaviorChange();

private:
    QUuid widgetId;

    QToolButton* upButton;
    QToolButton* downButton;

    QComboBox* typeComboBox;
    QLineEdit* nameEdit;
    QLabel* behaviorLabel;
    QComboBox* behaviorComboBox;

    QPushButton* removeButton;
    QPushButton* duplicateButton;
    QPushButton* saveButton;

    QMap<ColumnType::Value, ColumnSpecificSettingsWidget*>
        specificSettingsEditors;
    QVBoxLayout* specificEditorsLayout;
};

#endif // COLUMNSETTINGSWIDGET_H

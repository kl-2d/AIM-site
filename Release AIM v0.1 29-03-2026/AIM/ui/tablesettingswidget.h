#ifndef TABLESETTINGSWIDGET_H
#define TABLESETTINGSWIDGET_H

#include "columnsettingswidget.h"
#include "tableheaderpreviewwidget.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QVBoxLayout>

class TableSettingsWidget : public QDialog
{
public:
    TableSettingsWidget(
        bool createMode,
        const QList<QVariant>& settings = {},
        QWidget* parent = nullptr);

    QJsonObject getSettings() const;

private slots:
    void processUpButtonClicked(const QUuid& widgetId);
    void processDownButtonClicked(const QUuid& widgetId);
    void processRemoveButtonClicked(const QUuid& widgetId);
    void processDuplicateButtonClicked(const QUuid& widgetId);
    void processSaveButtonClicked(const QUuid& widgetId);

private:
    void addColumn(
        const QJsonObject &settings = QJsonObject(),
        bool createMode = true,
        int position = -1);
    QList<QString> getColumnNames() const;
    bool validate();
    bool validateLabels();

private:
    TableHeaderPreviewWidget* tableHeaderPrewiewWidget;
    QList<ColumnSettingsWidget*> columnSettingsWidgets;
    QVBoxLayout* columnSettingsLayout;
    QLabel* errorLabel;
    QPushButton* addColumnButton;
    QPushButton* selectColumnButton;
    QDialogButtonBox* buttons;
};

#endif // TABLESETTINGSWIDGET_H

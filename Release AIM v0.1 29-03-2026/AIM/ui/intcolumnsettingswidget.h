#ifndef INTCOLUMNSETTINGSWIDGET_H
#define INTCOLUMNSETTINGSWIDGET_H

#include "columnspecificsettingswidget.h"
#include "filtervalueedit.h"

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

class IntColumnSettingsWidget : public ColumnSpecificSettingsWidget
{
    Q_OBJECT

public:
    IntColumnSettingsWidget(QWidget* parent = nullptr);

    QJsonObject getSettings() const override;
    void setSettings(const QJsonObject& settings) override;
    QString validate() override;

private slots:
    void addInterval();
    void removeInterval(const QString& id);

private:
    struct IntervalEditor
    {
        QString id;
        FilterValueEdit* minEditor;
        FilterValueEdit* maxEditor;
        QToolButton* removeButton;
    };
    QList<IntervalEditor> editors;
    QPushButton* addButton;
    QVBoxLayout* editorsLayout;
};

#endif // INTCOLUMNSETTINGSWIDGET_H

#ifndef COLUMNSPECIFICSETTINGSWIDGET_H
#define COLUMNSPECIFICSETTINGSWIDGET_H

#include <QJsonObject>
#include <QWidget>

class ColumnSpecificSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    ColumnSpecificSettingsWidget(QWidget* parent = nullptr) : QWidget(parent)
    {
    }

    virtual QJsonObject getSettings() const = 0;
    virtual void setSettings(const QJsonObject& settings) = 0;
    virtual QString validate() = 0;
};

#endif // COLUMNSPECIFICSETTINGSWIDGET_H

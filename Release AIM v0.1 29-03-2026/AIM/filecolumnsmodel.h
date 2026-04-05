#ifndef FILECOLUMNSMODEL_H
#define FILECOLUMNSMODEL_H

#include "columntype.h"

#include <QJsonObject>
#include <QList>
#include <QVariant>

#include <optional>

struct StringValueParams
{
    QString text;
    QString code;
    QString description;
};

struct ColumnParams
{
    QString label;
    QString name;
    ColumnType::Value type;
    std::optional<ColumnType::Value> elementsType = std::nullopt;
    QList<QPair<double, double>> intervals;
    QList<StringValueParams> values;
    bool isInGroup = false;
    std::optional<QString> groupLabel = std::nullopt;

    QString valueByCode(const QString& code) const;
    QString codeByValue(const QString& text) const;
};

class FileColumnsModel : public QObject
{
    Q_OBJECT

public:
    FileColumnsModel(const QList<QVariant>& config, QObject* parent = nullptr);

    void setModel(const QList<QVariant>& config, bool fullChange);

    QList<QVariant> getConfig() const;

    int count() const;
    ColumnParams at(int index) const;

    QList<StringValueParams> valuesForGroup(const QString& groupLabel) const;

signals:
    void modelChanged(bool fullChange);

private:
    QVector<ColumnParams> params;
    QList<QVariant> config;
};

#endif // FILECOLUMNSMODEL_H

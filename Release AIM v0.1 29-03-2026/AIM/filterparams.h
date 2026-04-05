#ifndef FILTERPARAMS_H
#define FILTERPARAMS_H

#include <QList>
#include <QMetaType>
#include <QPair>
#include <QString>

struct FilterParams
{
    QList<QPair<double, double>> intervals;
    QList<QString> values;
    QString text;
    std::optional<QPair<QString, QString>> dates;
};

Q_DECLARE_METATYPE(FilterParams)

#endif // FILTERPARAMS_H

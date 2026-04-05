#ifndef BEHAVIORTYPE_H
#define BEHAVIORTYPE_H

#include <QObject>
#include <QString>

namespace BehaviorType
{

enum Value
{
    ANY_VALUES,
    VALUES_LIST,
    ENCODED_VALUES_LIST
};

QString toString(Value type);
QString toDisplayString(Value type);
Value fromString(const QString& str);
Value fromDisplayString(const QString& str);

} // namespace ColumnType

#endif // BEHAVIORTYPE_H

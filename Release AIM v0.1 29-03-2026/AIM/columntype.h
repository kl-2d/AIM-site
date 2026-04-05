#ifndef COLUMNTYPE_H
#define COLUMNTYPE_H

#include <QObject>
#include <QString>

namespace ColumnType
{

enum Value
{
    STRING,
    BOOL,
    INT,
    INT_WITH_INDEX,
    DOUBLE,
    DATETIME,
    LIST
};

QString toString(Value type);
QString toDisplayString(Value type);
Value fromString(const QString& str);
Value fromDisplayString(const QString& str);

} // namespace ColumnType

#endif // COLUMNTYPE_H

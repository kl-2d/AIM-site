#ifndef BOOLSTATUS_H
#define BOOLSTATUS_H

#include <QString>

namespace BoolStatus
{

enum Value
{
    YES,
    NO,
    ALL
};

QString toString(Value type);
Value fromString(const QString& str);

} // namespace BoolStatus

#endif // BOOLSTATUS_H

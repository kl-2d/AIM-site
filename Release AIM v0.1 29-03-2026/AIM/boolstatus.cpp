#include "boolstatus.h"

#include <map>

namespace BoolStatus
{

static const std::map<BoolStatus::Value, QString> dictionary = {
    {YES, "yes"}, {NO, "no"}, {ALL, "all"}};

QString toString(BoolStatus::Value type)
{
    auto it = dictionary.find(type);
    return it->second;
}

BoolStatus::Value fromString(const QString& str)
{
    auto it = std::find_if(
        dictionary.begin(), dictionary.end(), [str](const auto& value) {
            return value.second == str;
        });
    return it->first;
}

} // namespace BoolStatus

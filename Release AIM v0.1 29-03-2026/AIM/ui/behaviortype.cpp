#include "behaviortype.h"

#include <QCoreApplication>

#include <map>

namespace BehaviorType
{

static const std::map<Value, QString> dictionary = {
    {ANY_VALUES, "any_values"},
    {VALUES_LIST, "values_list"},
    {ENCODED_VALUES_LIST, "encoded_values_list"}};

std::map<Value, QString> displayValues() {
    static const std::map<Value, QString> displayDictionary = {
        {ANY_VALUES, QCoreApplication::translate("BehaviorType", "Any values")},
        {VALUES_LIST, QCoreApplication::translate("BehaviorType", "Values list")},
        {ENCODED_VALUES_LIST, QCoreApplication::translate("BehaviorType", "Values list with code")}};
    return displayDictionary;
}

QString toString(BehaviorType::Value type)
{
    auto it = dictionary.find(type);
    return it->second;
}

QString toDisplayString(Value type)
{
    auto it = displayValues().find(type);
    return it->second;
}

BehaviorType::Value fromString(const QString& str)
{
    auto it = std::find_if(
        dictionary.begin(), dictionary.end(), [str](const auto& value) {
            return value.second == str;
        });
    return it->first;
}

Value fromDisplayString(const QString& str)
{
    auto it = std::find_if(
        displayValues().begin(),
        displayValues().end(),
        [str](const auto& value) { return value.second == str; });
    return it->first;
}

} // namespace BehaviorType

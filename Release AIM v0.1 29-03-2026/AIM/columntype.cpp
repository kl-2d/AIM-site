#include "columntype.h"

#include <QCoreApplication>

#include <map>

namespace ColumnType
{

static const std::map<Value, QString> dictionary = {
    {STRING, "string"},
    {BOOL, "bool"},
    {INT, "int"},
    {INT_WITH_INDEX, "int_with_index"},
    {DOUBLE, "double"},
    {DATETIME, "datetime"},
    {LIST, "list"}};

std::map<Value, QString> displayValues() {
    static const std::map<Value, QString> displayDictionary = {
        {STRING, QCoreApplication::translate("ColumnType", "Text")},
        {BOOL, QCoreApplication::translate("ColumnType", "Boolean")},
        {INT, QCoreApplication::translate("ColumnType", "Integer")},
        {INT_WITH_INDEX,
         QCoreApplication::translate("ColumnType", "Integer with index")},
        {DOUBLE, QCoreApplication::translate("ColumnType", "Double")},
        {DATETIME, QCoreApplication::translate("ColumnType", "Date and time")},
        {LIST, QCoreApplication::translate("ColumnType", "List")}};
    return displayDictionary;
}

QString toString(Value type)
{
    auto it = dictionary.find(type);
    return it->second;
}

QString toDisplayString(Value type)
{
    auto it = displayValues().find(type);
    return it->second;
}

Value fromString(const QString& str)
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

} // namespace ColumnType

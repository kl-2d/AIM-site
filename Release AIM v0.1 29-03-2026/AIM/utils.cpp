#include "utils.h"

#include <map>

namespace Utils
{

static const std::map<QString, QString> dictionary = {
    {"А", "A"},
    {"Б", "B"},
    {"В", "V"},
    {"Г", "G"},
    {"Д", "D"},
    {"Е", "E"},
    {"Ё", "Yo"},
    {"Ж", "Zh"},
    {"З", "Z"},
    {"И", "I"},
    {"Й", "Yi"},
    {"К", "K"},
    {"Л", "L"},
    {"М", "M"},
    {"Н", "N"},
    {"О", "O"},
    {"П", "P"},
    {"Р", "R"},
    {"С", "S"},
    {"Т", "T"},
    {"У", "U"},
    {"Ф", "F"},
    {"Х", "H"},
    {"Ц", "Ts"},
    {"Ч", "Ch"},
    {"Ш", "Sh"},
    {"Щ", "Sch"},
    {"Ь", ""},
    {"Ъ", ""},
    {"Ы", "Y"},
    {"Э", "E"},
    {"Ю", "Yu"},
    {"Я", "Ya"},
    {"а", "a"},
    {"б", "b"},
    {"в", "v"},
    {"г", "g"},
    {"д", "d"},
    {"е", "e"},
    {"ё", "yo"},
    {"ж", "zh"},
    {"з", "z"},
    {"и", "i"},
    {"й", "yi"},
    {"к", "k"},
    {"л", "l"},
    {"м", "m"},
    {"н", "n"},
    {"о", "o"},
    {"п", "p"},
    {"р", "r"},
    {"с", "s"},
    {"т", "t"},
    {"у", "u"},
    {"ф", "f"},
    {"х", "h"},
    {"ц", "ts"},
    {"ч", "ch"},
    {"ш", "sh"},
    {"щ", "sch"},
    {"ь", ""},
    {"ъ", ""},
    {"ы", "y"},
    {"э", "e"},
    {"ю", "yu"},
    {"я", "ya"}
};


QString transliterate(const QString& text)
{
    QString result = text;
    for (const auto& letter: dictionary)
    {
        result.replace(letter.first, letter.second);
    }
    return result;
}

std::optional<bool> strToBool(const QString& value)
{
    if (value == "Да" || value == "Yes" || value == "1" || value == "+")
    {
        return true;
    }
    else if (value == "Нет" || value == "No" || value == "0" || value == "-")
    {
        return false;
    }
    else
    {
        return std::nullopt;
    }
}

} //namespace Utils

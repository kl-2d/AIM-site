#ifndef UTILS_H
#define UTILS_H

#include <QString>

#include <optional>

namespace Utils
{

QString transliterate(const QString& text);

std::optional<bool> strToBool(const QString& value);

} // namespace Utils

#endif // UTILS_H

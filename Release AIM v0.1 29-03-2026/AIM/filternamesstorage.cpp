#include "filternamesstorage.h"

#include <QFile>
#include <QFileInfo>

FilterNamesStorage::FilterNamesStorage(QObject* parent) : QObject(parent)
{
}

void FilterNamesStorage::set(const QList<QString>& filePaths)
{
    filterNames.clear();
    for (const auto& filePath: filePaths)
    {
        checkAndAdd(filePath);
    }
    emit updated(getNames());
}

void FilterNamesStorage::add(const QString& filePath)
{
    if (checkAndAdd(filePath))
    {
        emit updated(getNames());
        emit added(QFileInfo(filePath).fileName());
    }
}

void FilterNamesStorage::remove(const QString& fileName)
{
    if (filterNames.contains(fileName))
    {
        filterNames.remove(fileName);
        emit updated(getNames());
    }
}

QList<QString> FilterNamesStorage::getNames() const
{
    return filterNames.keys();
}

QList<QString> FilterNamesStorage::getFilePaths() const
{
    return filterNames.values();
}

QString FilterNamesStorage::getFilePath(const QString& fileName) const
{
    return filterNames.value(fileName);
}

bool FilterNamesStorage::checkAndAdd(const QString& filePath)
{
    QFile file(filePath);
    if (file.exists())
    {
        filterNames[QFileInfo(filePath).fileName()] = filePath;
        return true;
    }
    return false;
}

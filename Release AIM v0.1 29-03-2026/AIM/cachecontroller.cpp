#include "cachecontroller.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

const QString filtersTag = "filters";
const QString activeFilterTag = "active_filter";

CacheController::CacheController(
    const QString& fileName,
    FilterNamesStorage* filterNamesStorage,
    QObject* parent) :
  QObject(parent)
{
    this->fileName = fileName;
    this->filterNamesStorage = filterNamesStorage;

    load();

    QObject::connect(
        this->filterNamesStorage,
        &FilterNamesStorage::updated,
        this,
        &CacheController::save);
}

CacheController::~CacheController()
{
    save();
}

void CacheController::load()
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open cache file: " << file.errorString();
        return;
    }

    QJsonParseError error{};
    QJsonDocument document(QJsonDocument::fromJson(file.readAll(), &error));
    file.close();
    if (document.isNull())
    {
        qDebug() << "Cannot parse cache file: " << error.errorString();
        return;
    }

    QJsonObject cacheData = document.object();

    if (cacheData.contains(filtersTag))
    {
        auto filtersList =
            cacheData.value(filtersTag).toVariant().toStringList();
        filterNamesStorage->set(filtersList);
    }
}

void CacheController::save()
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot open cache file: " << file.errorString();
        return;
    }

    QJsonObject cacheData;
    QJsonArray filters;
    auto filePaths = filterNamesStorage->getFilePaths();
    for (const auto& filePath: filePaths)
    {
        filters.append(filePath);
    }
    cacheData.insert(filtersTag, filters);

    auto data = QJsonDocument(cacheData).toJson();
    if (file.write(data) != data.size())
    {
        qDebug() << "Error saving cache file: " << file.errorString();
    }
    file.close();
}

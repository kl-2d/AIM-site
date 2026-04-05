#ifndef CACHECONTROLLER_H
#define CACHECONTROLLER_H

#include "filternamesstorage.h"

#include <QString>

class CacheController : public QObject
{
    Q_OBJECT

public:
    CacheController(
        const QString& fileName,
        FilterNamesStorage* filterNamesStorage,
        QObject* parent = nullptr);
    ~CacheController();

private:
    void load();

private slots:
    void save();

private:
    QString fileName;
    FilterNamesStorage* filterNamesStorage;
};

#endif // CACHECONTROLLER_H

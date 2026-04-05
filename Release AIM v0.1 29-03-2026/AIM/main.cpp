#include "cachecontroller.h"
#include "csvfileprocessor.h"
#include "databasecontroller.h"
#include "datacolumnsmodel.h"
#include "datamodel.h"
#include "filecolumnsmodel.h"
#include "filternamesstorage.h"
#include "ui/mainwindow.h"
#include "tablemodel.h"
#include "tableproxymodel.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardItemModel>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // TODO: Load translations correctly
    QTranslator translator;
    translator.load(":translations_ru.qm");
    a.installTranslator(&translator);

    auto* fileName = std::getenv("EXCEL_CONFIG_PATH");
    if (fileName == nullptr)
    {
        fileName = "config.json";
        qDebug() << "Missing EXCEL_CONFIG_PATH variable. Using default";
    }
    QString fileNameStr = QString::fromUtf8(fileName);
    QFile file(fileNameStr);
    file.open(QIODevice::ReadOnly);

    QJsonParseError error{};
    QJsonDocument configFile(QJsonDocument::fromJson(file.readAll(), &error));
    QJsonObject config = configFile.object();
    file.close();

    FileColumnsModel fileColumnsModel(
        config.value("columns").toArray().toVariantList());

    // TODO: move to ConfigController
    QObject::connect(
        &fileColumnsModel,
        &FileColumnsModel::modelChanged,
        &fileColumnsModel,
        [&fileColumnsModel, fileNameStr]() {
            QJsonObject configObj;
            configObj.insert(
                "columns",
                QJsonValue::fromVariant(fileColumnsModel.getConfig()));
            QFile file(fileNameStr);
            file.open(QIODevice::WriteOnly);
            file.write(QJsonDocument(configObj).toJson());
            file.close();
        });

    DataColumnsModel dataColumnsModel(&fileColumnsModel);

    DatabaseController dbController(&dataColumnsModel);

    DataModel dataModel(&dataColumnsModel, &dbController);

    CsvFileProcessor fileProcessor(
        &fileColumnsModel, &dataColumnsModel, &dataModel);

    FilterNamesStorage filterNamesStorage;

    TableModel tableModel(&dataColumnsModel, &dataModel);
    TableProxyModel tableProxyModel(&dataColumnsModel);
    tableProxyModel.setSourceModel(&tableModel);

    ReportGenerator reportGenerator(&dataColumnsModel, &dataModel);

    auto* cacheFileName = std::getenv("CACHE_PATH");
    if (cacheFileName == nullptr)
    {
        cacheFileName = "cache.json";
        qDebug() << "Missing CACHE_PATH variable. Using default";
    }
    CacheController cacheController(cacheFileName, &filterNamesStorage);

    MainWindow w(&fileProcessor, &filterNamesStorage, &fileColumnsModel, &reportGenerator);
    w.setModel(&tableProxyModel);
    w.show();

    // TODO добавить дату при импорте

    return a.exec();
}

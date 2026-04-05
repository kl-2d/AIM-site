#ifndef CSVFILEPROCESSOR_H
#define CSVFILEPROCESSOR_H

#include "databasecontroller.h"
#include "datamodel.h"
#include "filecolumnsmodel.h"

class CsvFileProcessor
{
public:
    CsvFileProcessor(
        const FileColumnsModel* fileColumnsModel,
        const DataColumnsModel* dataColumnsModel,
        DataModel* dataModel);

    void importFromFile(const QString& fileName);
    void exportToFile(const QString& fileName, bool useDataFormat);

private:
    const FileColumnsModel* fileColumnsModel;
    const DataColumnsModel* dataColumnsModel;
    DataModel* dataModel;
};

#endif // CSVFILEPROCESSOR_H

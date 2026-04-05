#ifndef REPORTGENERATOR_H
#define REPORTGENERATOR_H

#include "datacolumnsmodel.h"
#include "datamodel.h"

#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextTableFormat>
#include <QUrl>

struct ReportParams
{
    QString researcher;
    QString venue;
    QString date;
    QString listNumber;
};

class ReportGenerator
{
public:
    ReportGenerator(
        const DataColumnsModel* dataColumnsModel,
        const DataModel* dataModel);

    QUrl generate(const ReportParams& params);

private:
    QTextBlockFormat headerBlockFormat() const;
    QTextCharFormat headerCharFormat() const;

    QTextBlockFormat headerCommentBlockFormat() const;
    QTextCharFormat headerCommentCharFormat() const;

    QTextBlockFormat contentBlockFormat() const;
    QTextCharFormat contentCharFormat() const;

    QTextTableFormat tableFormat() const;
    QTextCharFormat tableHeaderCharFormat() const;
    QTextCharFormat tableContentCharFormat() const;


private:
    const DataColumnsModel* dataColumnsModel;
    const DataModel* dataModel;
};

#endif // REPORTGENERATOR_H

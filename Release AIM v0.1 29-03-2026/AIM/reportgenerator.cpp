#include "reportgenerator.h"

#include <QDateTime>
#include <QDir>
#include <QTextDocument>
#include <QTextDocumentWriter>
#include <QTextCursor>

ReportGenerator::ReportGenerator(
    const DataColumnsModel *dataColumnsModel,
    const DataModel *dataModel)
{
    this->dataColumnsModel = dataColumnsModel;
    this->dataModel = dataModel;
}

QUrl ReportGenerator::generate(const ReportParams &params)
{
    QTextDocument report;
    QTextCursor cursor(&report);
    cursor.setBlockFormat(headerBlockFormat());
    cursor.setBlockCharFormat(headerCharFormat());
    cursor.beginEditBlock();
    cursor.insertText("Полевая опись");
    cursor.endEditBlock();

    cursor.insertBlock(headerCommentBlockFormat(), headerCommentCharFormat());
    cursor.beginEditBlock();
    cursor.insertText(QString("(массовый материал)"));
    cursor.endEditBlock();

    cursor.insertBlock(contentBlockFormat(), contentCharFormat());
    cursor.beginEditBlock();
    cursor.insertText(QString("Исследователь: %1").arg(params.researcher));
    cursor.endEditBlock();

    cursor.insertBlock(contentBlockFormat(), contentCharFormat());
    cursor.beginEditBlock();
    cursor.insertText(QString("Наименование памятника и его местоположение: %1").arg(params.venue));
    cursor.endEditBlock();

    cursor.insertBlock(contentBlockFormat(), contentCharFormat());
    cursor.beginEditBlock();
    cursor.insertText(QString("Дата проведения экспедиции (разведки): %1").arg(params.date));
    cursor.endEditBlock();

    cursor.insertBlock(contentBlockFormat(), contentCharFormat());
    cursor.insertBlock(contentBlockFormat(), contentCharFormat());
    cursor.beginEditBlock();
    cursor.insertText(QString("Количество предметов: %1").arg(QString::number(dataModel->rowCount())));
    cursor.endEditBlock();

    cursor.insertBlock(contentBlockFormat(), contentCharFormat());
    cursor.beginEditBlock();
    cursor.insertText(QString("Номер открытого листа: %1").arg(params.listNumber));
    cursor.endEditBlock();

    cursor.insertTable(
        dataModel->rowCount() + 1,
        dataColumnsModel->count(),
        tableFormat());

    QTextBlockFormat centerAlignment;
    centerAlignment.setAlignment(Qt::AlignHCenter);

    for (int column = 0; column < dataColumnsModel->count(); column++)
    {
        cursor.setBlockFormat(centerAlignment);
        cursor.setCharFormat(tableHeaderCharFormat());
        cursor.insertText(dataColumnsModel->at(column).name);
        cursor.movePosition(QTextCursor::NextCell);
    }

    QVector<QVector<QString>> surveyResults;
    for (int row = 0; row < dataModel->rowCount(); row++)
    {
        for (int column = 0; column < dataColumnsModel->count(); column++)
        {
            cursor.setBlockFormat(centerAlignment);
            cursor.setCharFormat(tableContentCharFormat());
            cursor.insertText(dataModel->data(row, column));
            cursor.movePosition(QTextCursor::NextCell);
        }
    }
    for (const auto& surveyResult: surveyResults)
    {
        for (int col = 0; col < dataColumnsModel->count(); col++)
        {
            cursor.setBlockFormat(centerAlignment);
            cursor.setCharFormat(tableContentCharFormat());
            cursor.insertText(surveyResult.at(col));
            cursor.movePosition(QTextCursor::NextCell);
        }
    }
    cursor.movePosition(QTextCursor::End);

    cursor.insertBlock(contentBlockFormat(), contentCharFormat());
    cursor.insertBlock(contentBlockFormat(), contentCharFormat());
    cursor.beginEditBlock();
    cursor.insertText(QString("Опись составлена            ___________________________________"));
    cursor.endEditBlock();
    cursor.insertBlock(contentBlockFormat(), contentCharFormat());
    cursor.beginEditBlock();
    cursor.insertText(QString("                                          (должность, ФИО, подпись)"));
    cursor.endEditBlock();

    cursor.insertBlock(contentBlockFormat(), contentCharFormat());
    cursor.beginEditBlock();
    cursor.insertText(QString("Руководитель работ          ___________________________________"));
    cursor.endEditBlock();
    cursor.insertBlock(contentBlockFormat(), contentCharFormat());
    cursor.beginEditBlock();
    cursor.insertText(QString("                                          (должность, ФИО, подпись)"));
    cursor.endEditBlock();

    auto reportName = QString("%1/report_%2.odt")
                          .arg(QDir::currentPath(), QDateTime::currentDateTime().toString("yyMMdd_hhmmss"));
    QTextDocumentWriter documentWriter(reportName);
    documentWriter.write(&report);

    return QUrl(reportName);
}

QTextBlockFormat ReportGenerator::headerBlockFormat() const
{
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignHCenter);
    format.setBottomMargin(10);
    format.setTopMargin(15);
    return format;
}

QTextCharFormat ReportGenerator::headerCharFormat() const
{
    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    format.setFontFamily("Liberation Serif");
    format.setFontPointSize(12);
    return format;
}

QTextBlockFormat ReportGenerator::headerCommentBlockFormat() const
{
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignHCenter);
    format.setBottomMargin(15);
    format.setTopMargin(0);
    return format;
}

QTextCharFormat ReportGenerator::headerCommentCharFormat() const
{
    QTextCharFormat format;
    format.setFontItalic(true);
    format.setFontFamily("Liberation Serif");
    format.setFontPointSize(12);
    return format;
}

QTextBlockFormat ReportGenerator::contentBlockFormat() const
{
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignLeft);
    format.setBottomMargin(8);
    format.setTopMargin(8);
    format.setTextIndent(10);
    return format;
}

QTextCharFormat ReportGenerator::contentCharFormat() const
{
    QTextCharFormat format;
    format.setFontFamily("Liberation Serif");
    format.setFontPointSize(12);
    return format;
}

QTextTableFormat ReportGenerator::tableFormat() const
{
    QTextTableFormat format;
    format.setAlignment(Qt::AlignCenter);
    format.setCellPadding(5);
    format.setHeaderRowCount(1);
    return format;
}

QTextCharFormat ReportGenerator::tableHeaderCharFormat() const
{
    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    format.setFontFamily("Liberation Serif");
    format.setFontPointSize(10);
    return format;
}

QTextCharFormat ReportGenerator::tableContentCharFormat() const
{
    QTextCharFormat format;
    format.setFontFamily("Liberation Serif");
    format.setFontPointSize(10);
    return format;
}

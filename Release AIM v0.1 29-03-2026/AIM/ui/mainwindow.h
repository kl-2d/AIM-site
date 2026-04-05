#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "csvfileprocessor.h"
#include "filternamesstorage.h"
#include "filterswidget.h"
#include "reportgenerator.h"
#include "tableproxymodel.h"
#include "tablewidget.h"

#include <QLabel>
#include <QMainWindow>
#include <QSplitter>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(
        CsvFileProcessor* fileProcessor,
        FilterNamesStorage* filterNamesStorage,
        FileColumnsModel* fileColumnsModel,
        ReportGenerator* reportGenerator,
        QWidget* parent = nullptr);
    ~MainWindow();

    void setModel(TableProxyModel* model);

private:
    bool eventFilter(QObject* obj, QEvent* event) override;

    static QAction* createAction(
        QObject* parent,
        const QString& text,
        const QIcon& icon = QIcon(),
        const QString& shortcut = QString());
    void createActions();

    QString description() const;

    void updateStatus();

    void loadFilters();
    void hideOrShowFilters();

private:
    CsvFileProcessor* fileProcessor;

    FilterNamesStorage* filterNamesStorage;

    FileColumnsModel* fileColumnsModel;

    ReportGenerator* reportGenerator;

    struct FileMenuActions
    {
        QAction* createSession = nullptr;
        QAction* openSession = nullptr;
        QAction* saveSession = nullptr;
        QAction* closeSession = nullptr;
        QAction* import = nullptr;
        QAction* exportInFileFormat = nullptr;
        QAction* exportInDataFormat = nullptr;
        QAction* exportToReport = nullptr;
        QAction* exit = nullptr;
    } fileMenuActions;

    struct TableMenuActions
    {
        QAction* create = nullptr;
        QAction* open = nullptr;
        QAction* close = nullptr;
        QAction* edit = nullptr;
    } tableMenuActions;

    struct FilterMenuActions
    {
        QAction* apply = nullptr;
        QAction* reset = nullptr;
        QAction* save = nullptr;
        QAction* load = nullptr;
        QAction* remove = nullptr;
        QAction* hide = nullptr;
    } filterMenuActions;

    QAction* about = nullptr;

    QSplitter* splitter;

    FiltersWidget* filtersWidget;
    TableWidget* tableWidget;

    QLabel* statusLabel;
};

#endif // MAINWINDOW_H

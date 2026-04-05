#include "mainwindow.h"

#include "reportsettingswidget.h"
#include "tablemodel.h"
#include "tableproxymodel.h"
#include "tablesettingswidget.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QKeyEvent>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow(CsvFileProcessor *fileProcessor,
                       FilterNamesStorage *filterNamesStorage,
                       FileColumnsModel *fileColumnsModel,
                       ReportGenerator *reportGenerator,
                       QWidget *parent)
    : QMainWindow(parent)
{
    this->fileProcessor = fileProcessor;
    this->filterNamesStorage = filterNamesStorage;
    this->fileColumnsModel = fileColumnsModel;
    this->reportGenerator = reportGenerator;

    setWindowIcon(QIcon(":/icons/AIM.ico"));
    setStyleSheet(
        "QMainWindow { background-image: url(:/images/background_main.jpg) }");

    setMinimumWidth(1000);
    setMinimumHeight(700);

    installEventFilter(this);

    filtersWidget = new FiltersWidget(filterNamesStorage);
    filtersWidget->setMaximumWidth(400);
    tableWidget = new TableWidget();
    tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);

    splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(filtersWidget);
    splitter->addWidget(tableWidget);
    splitter->setCollapsible(0, true);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(splitter);

    QWidget* centralWidget = new QWidget();
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    createActions();

    QMenu* fileMenu =
        menuBar()->addMenu(QCoreApplication::translate("Menu", "File"));
    fileMenu->addAction(fileMenuActions.createSession);
    fileMenu->addAction(fileMenuActions.openSession);
    fileMenu->addAction(fileMenuActions.saveSession);
    fileMenu->addAction(fileMenuActions.closeSession);
    fileMenu->addSeparator();
    fileMenu->addAction(fileMenuActions.import);
    fileMenu->addAction(fileMenuActions.exportInDataFormat);
    fileMenu->addAction(fileMenuActions.exportToReport);
    fileMenu->addSeparator();
    fileMenu->addAction(fileMenuActions.exit);

    QMenu* tableMenu =
        menuBar()->addMenu(QCoreApplication::translate("Menu", "Table"));
    tableMenu->addAction(tableMenuActions.create);
    tableMenu->addAction(tableMenuActions.open);
    tableMenu->addAction(tableMenuActions.close);
    tableMenu->addAction(tableMenuActions.edit);

    QMenu* filterMenu =
        menuBar()->addMenu(QCoreApplication::translate("Menu", "Filter"));
    filterMenu->addAction(filterMenuActions.apply);
    filterMenu->addAction(filterMenuActions.reset);
    filterMenu->addAction(filterMenuActions.save);
    filterMenu->addAction(filterMenuActions.load);
    filterMenu->addAction(filterMenuActions.remove);
    filterMenu->addSeparator();
    filterMenu->addAction(filterMenuActions.hide);

    QMenu* helpMenu =
        menuBar()->addMenu(QCoreApplication::translate("Menu", "Help"));
    helpMenu->addAction(about);

    QStatusBar* statusBar = new QStatusBar();
    statusLabel = new QLabel();
    statusBar->addPermanentWidget(statusLabel);
    setStatusBar(statusBar);
}

MainWindow::~MainWindow()
{
    delete filtersWidget;
    delete tableWidget;
}

void MainWindow::setModel(TableProxyModel* model)
{
    filtersWidget->setModel(model->getModel());
    tableWidget->setModel(model);
    for (int i = 0; i < model->columnCount(); i++)
    {
        tableWidget->setItemDelegateForColumn(i, tableWidget->itemDelegate());
    }
    QObject::connect(
        tableWidget,
        &TableWidget::deleteRequested,
        model,
        &TableProxyModel::deleteSelectedData);
    QObject::connect(
        model,
        &TableProxyModel::filtersUpdated,
        this,
        [this, model](const QMap<QString, FilterParams>& filters) {
            filtersWidget->setFilters(filters);
            filtersWidget->updateStatus(
                model->totalDataRows(), model->rowsWithErrors());
        });
    QObject::connect(
        model, &QAbstractItemModel::modelReset, this, [this, model]() {
            updateStatus();
            filtersWidget->updateStatus(
                model->totalDataRows(), model->rowsWithErrors());
        });
    QObject::connect(
        filtersWidget, &FiltersWidget::importFiltersRequested, this, [this]() {
            loadFilters();
        });
    QObject::connect(
        filtersWidget,
        &FiltersWidget::removeFiltersRequested,
        this,
        [this](const QString& fileName) {
            filterNamesStorage->remove(fileName);
        });
    QObject::connect(
        filtersWidget,
        &FiltersWidget::loadFiltersRequested,
        this,
        [this](const QString& fileName) {
            auto tableProxyModel =
                qobject_cast<TableProxyModel*>(tableWidget->model());
            tableProxyModel->setFiltersFromFile(fileName);
        });
    QObject::connect(
        filtersWidget,
        &FiltersWidget::applyFiltersRequested,
        model,
        &TableProxyModel::setFilters);
    QObject::connect(
        filtersWidget,
        &FiltersWidget::saveFiltersRequested,
        this,
        [this](const QJsonObject& filters) {
            auto fileName = QFileDialog::getSaveFileName();
            if (!fileName.isEmpty())
            {
                QFile file(fileName);
                file.open(QIODevice::WriteOnly);
                QJsonDocument filtersJson(filters);
                file.write(filtersJson.toJson());
                file.close();
                filterNamesStorage->add(fileName);
            }
        });
    updateStatus();
    filtersWidget->updateStatus(model->totalDataRows(), model->rowsWithErrors());
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        if (keyEvent->modifiers().testFlag(Qt::ControlModifier)
            && keyEvent->key() == Qt::Key_H)
        {
            hideOrShowFilters();
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

QAction* MainWindow::createAction(
    QObject* parent,
    const QString& text,
    const QIcon& icon,
    const QString& shortcut)
{
    auto action = new QAction(text, parent);

    if (!icon.isNull())
        action->setIcon(icon);

    if (!shortcut.isEmpty())
    {
        action->setShortcut(QKeySequence(shortcut));
        action->setToolTip(QString("%1 (%2)").arg(text, shortcut));
    }

    return action;
}

void MainWindow::createActions()
{
    fileMenuActions.createSession = createAction(
        this, QCoreApplication::translate("Menu", "Create session"));
    QObject::connect(
        fileMenuActions.createSession, &QAction::triggered, this, [this]() {});
    fileMenuActions.createSession->setEnabled(false);

    fileMenuActions.openSession =
        createAction(this, QCoreApplication::translate("Menu", "Open session"));
    QObject::connect(
        fileMenuActions.openSession, &QAction::triggered, this, [this]() {});
    fileMenuActions.openSession->setEnabled(false);

    fileMenuActions.saveSession =
        createAction(this, QCoreApplication::translate("Menu", "Save session"));
    QObject::connect(
        fileMenuActions.saveSession, &QAction::triggered, this, [this]() {});
    fileMenuActions.saveSession->setEnabled(false);

    fileMenuActions.closeSession =
        createAction(this, QCoreApplication::translate("Menu", "Close session"));
    QObject::connect(
        fileMenuActions.closeSession, &QAction::triggered, this, [this]() {});
    fileMenuActions.closeSession->setEnabled(false);

    fileMenuActions.import =
        createAction(this, QCoreApplication::translate("Menu", "Import"));
    QObject::connect(
        fileMenuActions.import, &QAction::triggered, this, [this]() {
            auto fileName = QFileDialog::getOpenFileName();
            if (!fileName.isEmpty())
            {
                fileProcessor->importFromFile(fileName);
            }
        });

    fileMenuActions.exportInFileFormat = createAction(
        this, QCoreApplication::translate("Menu", "Export in file format"));
    QObject::connect(
        fileMenuActions.exportInFileFormat, &QAction::triggered, this, [this]() {
            auto fileName = QFileDialog::getSaveFileName();
            if (!fileName.isEmpty())
            {
                fileProcessor->exportToFile(fileName, false);
            }
        });

    fileMenuActions.exportInDataFormat = createAction(
        this, QCoreApplication::translate("Menu", "Export to CSV file"));
    QObject::connect(
        fileMenuActions.exportInDataFormat, &QAction::triggered, this, [this]() {
            auto fileName = QFileDialog::getSaveFileName();
            if (!fileName.isEmpty())
            {
                if (!QFileInfo(fileName).fileName().contains("."))
                {
                    fileName.append(".csv");
                }
                fileProcessor->exportToFile(fileName, true);
            }
        });

    fileMenuActions.exportToReport = createAction(
        this, QCoreApplication::translate("Menu", "Export to report"));
    QObject::connect(
        fileMenuActions.exportToReport, &QAction::triggered, this, [this]() {
            auto* reportDialog = new ReportSettingsWidget(this);
            QObject::connect(
                reportDialog,
                &ReportSettingsWidget::accepted,
                this,
                [this, reportDialog]() {
                    ReportParams params = reportDialog->getParams();
                    QUrl reportUrl = reportGenerator->generate(params);
                    bool status =
                        QDesktopServices::openUrl("file:///" + reportUrl.toString());
                    if (!status)
                    {
                        QMessageBox::warning(
                            this,
                            QCoreApplication::translate("Report", "Open report file error"),
                            QCoreApplication::translate("Report", "Cannot open report file %1")
                                .arg(reportUrl.toString()));
                    }
                });
            reportDialog->exec();
            reportDialog->deleteLater();
        });

    fileMenuActions.exit =
        createAction(this, QCoreApplication::translate("Menu", "Exit"));
    QObject::connect(fileMenuActions.exit, &QAction::triggered, this, [this]() {
        close();
    });

    tableMenuActions.create =
        createAction(this, QCoreApplication::translate("Menu", "Create"));
    QObject::connect(
        tableMenuActions.create, &QAction::triggered, this, [this]() {
            auto* tableSettingsDialog = new TableSettingsWidget(true, {}, this);
            tableSettingsDialog->setWindowTitle(
                QCoreApplication::translate("TableSettings", "Create table"));
            QObject::connect(
                tableSettingsDialog,
                &TableSettingsWidget::accepted,
                this,
                [this, tableSettingsDialog]() {
                    QJsonObject settings = tableSettingsDialog->getSettings();
                    fileColumnsModel->setModel(
                        settings.value("columns").toArray().toVariantList(), true);
                });
            tableSettingsDialog->exec();
            tableSettingsDialog->deleteLater();
        });

    tableMenuActions.open =
        createAction(this, QCoreApplication::translate("Menu", "Open"));
    QObject::connect(
        tableMenuActions.open, &QAction::triggered, this, [this]() {});
    tableMenuActions.open->setEnabled(false);

    tableMenuActions.close =
        createAction(this, QCoreApplication::translate("Menu", "Close"));
    QObject::connect(
        tableMenuActions.close, &QAction::triggered, this, [this]() {});
    tableMenuActions.close->setEnabled(false);

    tableMenuActions.edit =
        createAction(this, QCoreApplication::translate("Menu", "Edit"));
    QObject::connect(
        tableMenuActions.edit, &QAction::triggered, this, [this]() {
            QList<QVariant> config = fileColumnsModel->getConfig();
            auto* tableSettingsDialog = new TableSettingsWidget(false, config, this);
            tableSettingsDialog->setWindowTitle(
                QCoreApplication::translate("TableSettings", "Edit table"));
            QObject::connect(
                tableSettingsDialog,
                &TableSettingsWidget::accepted,
                this,
                [this, tableSettingsDialog]() {
                    QJsonObject settings = tableSettingsDialog->getSettings();
                    fileColumnsModel->setModel(
                        settings.value("columns").toArray().toVariantList(), false);
                });
            tableSettingsDialog->exec();
            tableSettingsDialog->deleteLater();
        });

    filterMenuActions.apply =
        createAction(this, QCoreApplication::translate("Menu", "Apply"));
    QObject::connect(
        filterMenuActions.apply, &QAction::triggered, this, [this]() {
            filtersWidget->apply();
        });

    filterMenuActions.reset =
        createAction(this, QCoreApplication::translate("Menu", "Reset"));
    QObject::connect(
        filterMenuActions.reset, &QAction::triggered, this, [this]() {
            filtersWidget->reset();
        });

    filterMenuActions.save =
        createAction(this, QCoreApplication::translate("Menu", "Save"));
    QObject::connect(
        filterMenuActions.save, &QAction::triggered, this, [this]() {
            filtersWidget->save();
        });

    filterMenuActions.load =
        createAction(this, QCoreApplication::translate("Menu", "Load filter"));
    QObject::connect(
        filterMenuActions.load, &QAction::triggered, this, [this]() {
            loadFilters();
        });

    filterMenuActions.remove =
        createAction(this, QCoreApplication::translate("Menu", "Remove"));
    QObject::connect(
        filterMenuActions.remove, &QAction::triggered, this, [this]() {});
    filterMenuActions.remove->setEnabled(false);

    filterMenuActions.hide =
        createAction(this, QCoreApplication::translate("Menu", "Hide"), {}, "Ctrl+H");
    QObject::connect(
        filterMenuActions.hide, &QAction::triggered, this, [this]() {
            hideOrShowFilters();
        });

    about = createAction(this, QCoreApplication::translate("Menu", "About"));
    QObject::connect(about, &QAction::triggered, this, [this]() {
        QMessageBox about;
        about.setParent(this);
        about.setWindowTitle(QCoreApplication::translate("Menu", "About"));
        about.setText(description());
        about.setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
        about.setStandardButtons(QMessageBox::Ok);
        about.setIconPixmap(QIcon(":/icons/AIM.ico").pixmap(60, 60));
        about.exec();
    });
}

QString MainWindow::description() const
{
    return QCoreApplication::translate(
        "About", "Archaeological Inventory Maker\nv. 0.1\n\n2026\n\nAhka2004@yandex.ru");
}

void MainWindow::updateStatus()
{
    auto tableProxyModel = qobject_cast<TableProxyModel*>(tableWidget->model());
    auto tableModel = qobject_cast<TableModel*>(tableProxyModel->sourceModel());
    statusLabel->setText(
        QCoreApplication::translate(
            "Status", "Records count: %1, with errors: %2")
            .arg(
                QString::number(tableModel->rowCount() - 1),
                QString::number(tableModel->rowsWithErrors())));
}

void MainWindow::loadFilters()
{
    auto fileName = QFileDialog::getOpenFileName();
    if (!fileName.isEmpty())
    {
        auto tableProxyModel =
            qobject_cast<TableProxyModel*>(tableWidget->model());
        tableProxyModel->setFiltersFromFile(fileName);
        filterNamesStorage->add(fileName);
    }
}

void MainWindow::hideOrShowFilters()
{
    if (splitter->sizes().at(0) != 0)
    {
        splitter->setSizes({0, splitter->width()});
        filterMenuActions.hide->setText(QCoreApplication::translate("Menu", "Show"));
    }
    else
    {
        auto filtersWidth = filtersWidget->minimumSizeHint().width();
        splitter->setSizes({filtersWidth, splitter->width() - filtersWidth});
        filterMenuActions.hide->setText(QCoreApplication::translate("Menu", "Hide"));
    }
}

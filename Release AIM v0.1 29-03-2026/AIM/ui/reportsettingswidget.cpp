#include "reportsettingswidget.h"

#include <QCoreApplication>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

ReportSettingsWidget::ReportSettingsWidget(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle(
        QCoreApplication::translate("Report", "Export to report"));
    setMinimumWidth(800);
    setStyleSheet(
        "QDialog { background-image: url(:/images/background_settings.jpg) }");

    QLabel* researcherLabel = new QLabel();
    researcherLabel->setText(QCoreApplication::translate("Report", "Researcher"));
    QLabel* venueLabel = new QLabel();
    venueLabel->setText(QCoreApplication::translate("Report", "Venue"));
    QLabel* dateLabel = new QLabel();
    dateLabel->setText(QCoreApplication::translate("Report", "Date"));
    QLabel* listNumberLabel = new QLabel();
    listNumberLabel->setText(QCoreApplication::translate("Report", "List number"));

    researcherEdit = new QLineEdit();
    venueEdit = new QLineEdit();
    dateEdit = new QLineEdit();
    listNumberEdit = new QLineEdit();

    QGridLayout* mainLayout = new QGridLayout();
    mainLayout->addWidget(researcherLabel, 0, 0);
    mainLayout->addWidget(researcherEdit, 0, 1);
    mainLayout->addWidget(venueLabel, 1, 0);
    mainLayout->addWidget(venueEdit, 1, 1);
    mainLayout->addWidget(dateLabel, 2, 0);
    mainLayout->addWidget(dateEdit, 2, 1);
    mainLayout->addWidget(listNumberLabel, 3, 0);
    mainLayout->addWidget(listNumberEdit, 3, 1);

    buttons =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(
        buttons->button(QDialogButtonBox::Ok),
        &QPushButton::clicked,
        this,
        &QDialog::accept);
    connect(
        buttons->button(QDialogButtonBox::Cancel),
        &QPushButton::clicked,
        this,
        &QDialog::reject);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(mainLayout);
    layout->addWidget(buttons, 0, Qt::AlignRight | Qt::AlignBottom);

    setLayout(layout);
}

ReportParams ReportSettingsWidget::getParams()
{
    ReportParams result;
    result.researcher = researcherEdit->text();
    result.venue = venueEdit->text();
    result.date = dateEdit->text();
    result.listNumber = listNumberEdit->text();
    return result;
}

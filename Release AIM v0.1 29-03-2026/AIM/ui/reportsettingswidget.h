#ifndef REPORTSETTINGSWIDGET_H
#define REPORTSETTINGSWIDGET_H

#include "reportgenerator.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>

class ReportSettingsWidget : public QDialog
{
    Q_OBJECT

public:
    ReportSettingsWidget(QWidget* parent = nullptr);

    ReportParams getParams();

private:
    QLineEdit *researcherEdit;
    QLineEdit *venueEdit;
    QLineEdit *dateEdit;
    QLineEdit *listNumberEdit;

    QDialogButtonBox* buttons;
};

#endif // REPORTSETTINGSWIDGET_H

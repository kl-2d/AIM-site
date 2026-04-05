#ifndef FILTERVALUEEDIT_H
#define FILTERVALUEEDIT_H

#include <QLineEdit>

class FilterValueEdit : public QLineEdit
{
    Q_OBJECT

public:
    enum ValueType
    {
        INT,
        DOUBLE
    };

public:
    FilterValueEdit(
        const QString& placeholderText,
        ValueType type,
        QWidget* parent = nullptr);
};

#endif // FILTERVALUEEDIT_H

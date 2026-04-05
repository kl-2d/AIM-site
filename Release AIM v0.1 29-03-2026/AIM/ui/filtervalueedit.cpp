#include "filtervalueedit.h"

FilterValueEdit::FilterValueEdit(
    const QString& placeholderText, ValueType type, QWidget* parent) :
  QLineEdit(parent)
{
    Q_UNUSED(type)
    setPlaceholderText(placeholderText);
}

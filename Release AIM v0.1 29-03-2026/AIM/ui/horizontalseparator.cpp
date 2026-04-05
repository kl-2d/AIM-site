#include "horizontalseparator.h"

HorizontalSeparator::HorizontalSeparator(QWidget* parent) : QFrame(parent)
{
    setFixedHeight(1);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setStyleSheet("QFrame { background-color: gray; border: none; }");
}

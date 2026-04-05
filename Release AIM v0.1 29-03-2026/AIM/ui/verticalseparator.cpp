#include "verticalseparator.h"

VerticalSeparator::VerticalSeparator(QWidget* parent) : QFrame(parent)
{
    setFixedWidth(1);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setStyleSheet("QFrame { background-color: gray; border: none; }");
}

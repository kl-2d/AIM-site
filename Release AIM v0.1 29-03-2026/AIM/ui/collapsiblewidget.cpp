#include "collapsiblewidget.h"

#include <QHBoxLayout>
#include <QIcon>
#include <QVBoxLayout>

CollapsibleWidget::CollapsibleWidget(const QString& title, FilterWidget *content, QWidget* parent) :
  QWidget(parent)
{
    setMouseTracking(true);

    titleLabel = new QLabel(title);
    titleLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    titleLabel->setAlignment(Qt::AlignLeft);
    QFont font = titleLabel->font();
    font.setPixelSize(16);
    titleLabel->setFont(font);

    collapseIconLabel = new QLabel();
    collapseIconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    collapseIconLabel->setAlignment(Qt::AlignTop);
    QIcon collapseIcon(":/icons/expand_less.svg");
    collapseIconLabel->setPixmap(collapseIcon.pixmap(15, 15));

    highlightIconLabel = new QLabel();
    highlightIconLabel->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Preferred);
    highlightIconLabel->setAlignment(Qt::AlignTop);
    QIcon highlightIcon(":/icons/dot.svg");
    highlightIconLabel->setPixmap(highlightIcon.pixmap(15, 15));
    highlightIconLabel->setVisible(false);

    auto* spacerWidget = new QWidget();

    contentWidget = content;

    QObject::connect(contentWidget, &FilterWidget::updated, this, [this]() {
        highlightIconLabel->setVisible(collapsed && !contentWidget->isEmpty());
    });

    QIcon collapseIcon2(
        collapsed ? ":/icons/expand_more.svg" : ":/icons/expand_less.svg");
    collapseIconLabel->setPixmap(collapseIcon2.pixmap(15, 15));
    contentWidget->setVisible(!collapsed);
    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->addWidget(collapseIconLabel);
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(highlightIconLabel);
    titleLayout->addWidget(spacerWidget);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(titleLayout);
    layout->addWidget(contentWidget);

    setLayout(layout);
}

void CollapsibleWidget::mouseReleaseEvent(QMouseEvent* event)
{
    collapsed = !collapsed;
    update();
}

void CollapsibleWidget::setCollapsed(bool value)
{
    collapsed = value;
    update();
}

void CollapsibleWidget::update()
{
    QIcon collapseIcon(
        collapsed ? ":/icons/expand_more.svg" : ":/icons/expand_less.svg");
    collapseIconLabel->setPixmap(collapseIcon.pixmap(15, 15));
    contentWidget->setVisible(!collapsed);
    highlightIconLabel->setVisible(collapsed && !contentWidget->isEmpty());
}

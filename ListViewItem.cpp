#include "ListViewItem.h"

#include "ui_ListViewItem.h"

ListViewItem::ListViewItem(QWidget *parent)
    : QWidget(parent), ui(new Ui::ListViewItem), m_clicked(false) {
  ui->setupUi(this);
  ui->toolButton->setText(tr("             Buttons"));
  ui->toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  connect(ui->toolButton, &QToolButton::clicked, this,
          &ListViewItem::slot_toolbtnclicked);
  ui->list_widgets->hide();
}

ListViewItem::~ListViewItem() { delete ui; }

LVGLWidgetListView *ListViewItem::getlistview() {
  return this->ui->list_widgets;
}

void ListViewItem::slot_toolbtnclicked() {
  m_clicked = !m_clicked;
  if (m_clicked) {
    ui->toolButton->setIcon(QIcon(":/icons/Widget Box_arrow_90.png"));
    ui->list_widgets->show();
  } else {
    ui->toolButton->setIcon(QIcon(":/icons/Widget Box_arrow_0.png"));
    ui->list_widgets->hide();
  }

  ui->toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

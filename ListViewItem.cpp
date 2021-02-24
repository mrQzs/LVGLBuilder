#include "ListViewItem.h"

#include "ui_ListViewItem.h"

ListViewItem::ListViewItem(QWidget *parent)
    : QWidget(parent), ui(new Ui::ListViewItem), m_clicked(false) {
  ui->setupUi(this);
  ui->toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  connect(ui->toolButton, &QToolButton::clicked, this,
          &ListViewItem::slot_toolbtnclicked);
  ui->list_widgets->hide();
  ui->toolButton->setStyleSheet(
      "QToolButton "
      "{padding:2px "
      "4px;border:1px;border-radius:5px;border-top-left-radius:2px;"
      "border-top-right-radius:"
      "2px;border-style:"
      "solid;"
      "border-image: "
      "url(:/icons/Widget "
      "Box_background_nor.png);}"
      "QToolButton:hover{border-radius:5px;border-image:url(:/icons/"
      "Widget Box_background_cli.png);}");
}

ListViewItem::~ListViewItem() { delete ui; }

LVGLWidgetListView *ListViewItem::getlistview() {
  return this->ui->list_widgets;
}

void ListViewItem::settoolbtnText(const QString &str) {
  ui->toolButton->setText(str);
}

void ListViewItem::slot_toolbtnclicked() {
  m_clicked = !m_clicked;
  if (m_clicked) {
    ui->toolButton->setIcon(QIcon(":/icons/Widget Box_arrow_90.png"));
    ui->list_widgets->show();
    ui->toolButton->setStyleSheet(
        "QToolButton "
        "{padding:2px "
        "4px;border:1px;border-radius:5px;border-top-left-radius:2px;border-"
        "top-"
        "right-radius:"
        "2px;border-style:"
        "solid;"
        "border-image: "
        "url(:/icons/Widget "
        "Box_background_cli.png);}"
        "QToolButton:hover{border:1px;border-image:url(:/icons/"
        "Widget Box_background_nor.png);}");

    int btnheight = ui->toolButton->height();
    int row = ui->list_widgets->model()->rowCount();
    int rowheight = 0;
    if (row == 3)
      rowheight = row * 29;
    else if (row == 13)
      rowheight = row * 24.5;
    else
      rowheight = row * 23.5;

    this->setFixedHeight(btnheight + rowheight);
  } else {
    ui->toolButton->setIcon(QIcon(":/icons/Widget Box_arrow_0.png"));
    ui->list_widgets->hide();
    ui->toolButton->setStyleSheet(
        "QToolButton "
        "{padding:2px "
        "4px;border:1px;border-radius:5px;border-top-left-radius:2px;border-"
        "top-"
        "right-radius:"
        "2px;border-style:"
        "solid;"
        "border-image: "
        "url(:/icons/Widget "
        "Box_background_nor.png);}"
        "QToolButton:hover{border:1px;border-radius:5px;border-image:url(:/"
        "icons/"
        "Widget Box_background_cli.png);}");
    int btnheight = ui->toolButton->height();
    this->setFixedHeight(btnheight);
  }

  ui->toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
}

#include "ListDelegate.h"

#include <QDebug>
#include <QPainter>

ListDelegate::ListDelegate(QObject *parent) {}

ListDelegate::~ListDelegate() {}

void ListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const {
  QStyleOptionViewItem viewoption(option);
  QStyledItemDelegate::paint(painter, viewoption, index);
  QString name = index.data().toString();
  if (name == "Button") {
    QPixmap pixmap = QPixmap(":/icons/testBtn.jpg");
    painter->drawPixmap(viewoption.rect, pixmap);
    qDebug() << option.rect.height();
    qDebug() << option.rect.width();
  } else if (name == "Image button") {
  }
}

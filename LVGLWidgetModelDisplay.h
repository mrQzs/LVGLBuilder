#ifndef LVGLWIDGETMODELDISPLAY_H
#define LVGLWIDGETMODELDISPLAY_H

#include <QAbstractListModel>

class LVGLWidget;

class LVGLWidgetModelDisplay : public QAbstractListModel {
  Q_OBJECT

 public:
  explicit LVGLWidgetModelDisplay(QObject *parent = nullptr);

  // Basic functionality:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  virtual QStringList mimeTypes() const override;
  virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;
};

#endif  // LVGLWIDGETMODEL_H

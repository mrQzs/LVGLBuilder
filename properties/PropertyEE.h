#ifndef PROPERTYEE_H
#define PROPERTYEE_H

#include <LVGLProperty.h>

#include <QWidget>

typedef struct _int_value_2 {
  int value1;
  int value2;
  _int_value_2() : value1(1), value2(0) {}
} IntValue2;
Q_DECLARE_METATYPE(IntValue2)

QT_BEGIN_NAMESPACE
namespace Ui {
class PropertyEE;
}
QT_END_NAMESPACE

class PropertyEE : public QWidget {
  Q_OBJECT

 public:
  PropertyEE(QWidget *parent = nullptr);
  ~PropertyEE();
  void addItems(const QStringList &l1, const QStringList &l2);
  void setCurrentIndex(const int &index1, const int &index2);

  IntValue2 getCurrentIndex();

 private:
  Ui::PropertyEE *ui;
};

class LVGLPropertyEnumThree : public LVGLProperty {
 public:
  LVGLPropertyEnumThree(QStringList enumText1, QStringList enumText2,
                        LVGLProperty *parent = nullptr);

  QVariant value(LVGLObject *obj) const override;
  void setValue(LVGLObject *obj, QVariant value) override;

  bool hasEditor() const override;

  QWidget *editor(QWidget *parent) override;
  void updateEditor(LVGLObject *obj) override;
  void updateWidget(LVGLObject *obj) override;

 protected:
  QStringList m_enum1;
  QStringList m_enum2;
  PropertyEE *m_widget;

  virtual IntValue2 get(LVGLObject *obj) const = 0;
  virtual void set(LVGLObject *obj, IntValue2 value) = 0;
};
#endif  // PROPERTYEE_H

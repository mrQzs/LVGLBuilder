#include "PropertyEE.h"

#include "ui_PropertyEE.h"

PropertyEE::PropertyEE(QWidget *parent)
    : QWidget(parent), ui(new Ui::PropertyEE) {
  ui->setupUi(this);
}

PropertyEE::~PropertyEE() { delete ui; }

void PropertyEE::addItems(const QStringList &l1, const QStringList &l2) {
  ui->comboBox->addItems(l1);
  ui->comboBox_2->addItems(l2);
}

void PropertyEE::setCurrentIndex(const int &index1, const int &index2) {
  ui->comboBox->setCurrentIndex(index1);
  ui->comboBox_2->setCurrentIndex(index2);
}

IntValue2 PropertyEE::getCurrentIndex() {
  IntValue2 var;
  var.value1 = ui->comboBox->currentIndex();
  var.value2 = ui->comboBox_2->currentIndex();
  return var;
}

LVGLPropertyEnumThree::LVGLPropertyEnumThree(QStringList enumText1,
                                             QStringList enumText2,
                                             LVGLProperty *parent)
    : LVGLProperty(parent),
      m_enum1(enumText1),
      m_enum2(enumText2),
      m_widget(Q_NULLPTR) {}

QVariant LVGLPropertyEnumThree::value(LVGLObject *obj) const {
  auto index = get(obj);
  Q_ASSERT(index.value1 < m_enum1.size());
  Q_ASSERT(index.value2 < m_enum2.size());
  return QStringList() << m_enum1.at(index.value1) << m_enum2.at(index.value2);
}

void LVGLPropertyEnumThree::setValue(LVGLObject *obj, QVariant value) {
  IntValue2 var = value.value<IntValue2>();
  set(obj, var);
}

bool LVGLPropertyEnumThree::hasEditor() const { return true; }

QWidget *LVGLPropertyEnumThree::editor(QWidget *parent) {
  m_widget = new PropertyEE(parent);
  m_widget->addItems(m_enum1, m_enum2);
  return m_widget;
}

void LVGLPropertyEnumThree::updateEditor(LVGLObject *obj) {
  IntValue2 val = get(obj);

  m_widget->setCurrentIndex(val.value1, val.value2);
}

void LVGLPropertyEnumThree::updateWidget(LVGLObject *obj) {
  set(obj, m_widget->getCurrentIndex());
}

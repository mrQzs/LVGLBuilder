#include "LVGLButtonMatrix.h"

#include <QDebug>
#include <QIcon>

#include "LVGLObject.h"
#include "properties/LVGLPropertyAnyFunc.h"

class LVGLPropertyBtnmatrixButtonsText : public LVGLPropertyStringPlus {
 public:
  LVGLPropertyBtnmatrixButtonsText() : m_btnTotal(5) {}

  int getBtnTotal() { return m_btnTotal; }

  QString name() const { return "Button's Text"; }

  QStringList function(LVGLObject *obj) const {
    return QStringList()
           << QString(
                  "lv_roller_set_options(%1, \"%2\",LV_ROLLER_MODE_NORMAL);")
                  .arg(obj->codeName())
                  .arg(get(obj));
  }

 protected:
  QString get(LVGLObject *obj) const {
    const char **map = lv_btnmatrix_get_map_array(obj->obj());
    QStringList strmap;
    for (int i = 0; i < m_btnTotal; ++i) strmap << QString(map[i]);
    QString tmp;
    for (int i = 0; i < strmap.size(); ++i)
      if (i < strmap.size() - 1)
        tmp += strmap[i] + ",";
      else
        tmp += strmap[i];
    return tmp;
  }
  void set(LVGLObject *obj, QString string) {
    QStringList strlist = string.split(',');
    m_btnTotal = strlist.size();
    const char **p = new const char *[strlist.size() + 1];
    for (int i = 0; i < strlist.size(); ++i) {
      auto text = strlist[i].toUtf8();
      char *data = new char[text.size() + 1];
      strcpy(data, text.data());
      p[i] = data;
    }
    p[strlist.size()] = "";
    lv_btnmatrix_set_map(obj->obj(), p);
  }

 private:
  int m_btnTotal;
};

class LVGLPropertyBtnmatrixTextAlign : public LVGLPropertyEnum {
 public:
  LVGLPropertyBtnmatrixTextAlign()
      : LVGLPropertyEnum({"Left", "Right", "Center", "Auto"}),
        m_values({"LV_LABEL_ALIGN_LEFT", "LV_LABEL_ALIGN_RIGHT",
                  "LV_LABEL_ALIGN_CENTER", "LV_LABEL_ALIGN_AUTO"}) {}

  QString name() const { return "Text Align"; }

  QStringList function(LVGLObject *obj) const {
    return QStringList() << QString("lv_btnmatrix_set_align(%1, %2);")
                                .arg(obj->codeName())
                                .arg(m_values.at(get(obj)));
    return QStringList();
  }

 protected:
  int get(LVGLObject *obj) const { return lv_btnmatrix_get_align(obj->obj()); }
  void set(LVGLObject *obj, int index) {
    lv_btnmatrix_set_align(obj->obj(), index & 0xff);
  }
  QStringList m_values;
};

class LVGLPropertyBtnmatrixFocus : public LVGLPropertyInt {
 public:
  LVGLPropertyBtnmatrixFocus(LVGLPropertyBtnmatrixButtonsText *p)
      : LVGLPropertyInt(-1, UINT16_MAX - 1, ""), m_btnid(-1), m_lpbbt(p) {}

  QString name() const { return "Focus"; }

  QStringList function(LVGLObject *obj) const {
    if (m_btnid != -1)
      return QStringList() << QString("lv_btnmatrix_set_focused_btn(%1,%2);")
                                  .arg(obj->codeName())
                                  .arg(m_widget->value());
    else
      return QStringList()
             << QString(
                    "lv_btnmatrix_set_focused_btn(%1,LV_BTNMATRIX_BTN_NONE);")
                    .arg(obj->codeName());
  }

 protected:
  int get(LVGLObject *obj) const {
    auto max = lv_btnmatrix_get_focused_btn(obj->obj());
    if (max == LV_BTNMATRIX_BTN_NONE) return -1;
    return max;
  }
  void set(LVGLObject *obj, int value) {
    m_btnid = value;
    if (m_btnid == -1)
      lv_btnmatrix_set_focused_btn(obj->obj(), LV_BTNMATRIX_BTN_NONE);
    if (value >= m_lpbbt->getBtnTotal()) {
      auto var = m_lpbbt->getBtnTotal() - 1;
      lv_btnmatrix_set_focused_btn(obj->obj(), static_cast<uint16_t>(var));
    } else
      lv_btnmatrix_set_focused_btn(obj->obj(), static_cast<uint16_t>(value));
  }

 private:
  int m_btnid;
  LVGLPropertyBtnmatrixButtonsText *m_lpbbt;
};

class LVGLPropertyBtnmatrixButtonCtrl : public LVGLPropertyAnyFunc {
 public:
  LVGLPropertyBtnmatrixButtonCtrl(AnyFuncColType arr[], int size,
                                  LVGLPropertyBtnmatrixButtonsText *p)
      : LVGLPropertyAnyFunc(arr, size),
        m_lpbbt(p),
        m_list(QStringList() << "Empty list"),
        m_frun(true) {}
  QString name() const { return "Button's Ctrl"; }

 protected:
  QStringList get(LVGLObject *obj) const {
    if (m_frun) {
      m_frun = false;
      QStringList l1, l2;
      int total = m_lpbbt->getBtnTotal();
      for (int i = 0; i < total; ++i) l1 << QString::number(i);
      l2 << "LV_BTNMATRIX_CTRL_HIDDEN"
         << "LV_BTNMATRIX_CTRL_NO_REPEAT"
         << "LV_BTNMATRIX_CTRL_DISABLED"
         << "LV_BTNMATRIX_CTRL_CHECKABLE"
         << "LV_BTNMATRIX_CTRL_CHECK_STATE"
         << "LV_BTNMATRIX_CTRL_CLICK_TRIG";
      updateData(0, l1);
      updateData(1, l2);
    }

    if (m_list[0] != "Empty list") return m_list;
    return QStringList();
  }
  void set(LVGLObject *obj, QStringList list) { m_list = list; }
  LVGLPropertyBtnmatrixButtonsText *m_lpbbt;

  QStringList m_list;
  mutable bool m_frun;
};

LVGLButtonMatrix::LVGLButtonMatrix() {
  m_defaultobj = lv_btnmatrix_create(m_parent, NULL);
  initStateStyles();
  auto p = new LVGLPropertyBtnmatrixButtonsText;
  m_properties << p;
  m_properties << new LVGLPropertyBtnmatrixFocus(p);
  m_properties << new LVGLPropertyBtnmatrixTextAlign;
  static AnyFuncColType arr[3]{e_QComboBox, e_QComboBox};
  m_properties << new LVGLPropertyBtnmatrixButtonCtrl(arr, 2, p);
  m_parts << LV_BTNMATRIX_PART_BG << LV_BTNMATRIX_PART_BTN;
  m_editableStyles << LVGL::Background;    // LV_BTNMATRIX_PART_BG
  m_editableStyles << LVGL::BtnMatrixBTN;  // LV_BTNMATRIX_PART_BTN
}

QString LVGLButtonMatrix::name() const { return "Button Matrix"; }

QString LVGLButtonMatrix::className() const { return "lv_btnm"; }

LVGLWidget::Type LVGLButtonMatrix::type() const { return ButtonMatrix; }

QIcon LVGLButtonMatrix::icon() const { return QIcon(); }

lv_obj_t *LVGLButtonMatrix::newObject(lv_obj_t *parent) const {
  lv_obj_t *obj = lv_btnmatrix_create(parent, nullptr);
  return obj;
}

QSize LVGLButtonMatrix::minimumSize() const { return QSize(150, 100); }

QStringList LVGLButtonMatrix::styles() const {
  return QStringList() << "BTNMATRIX_PART_BG"
                       << "BTNMATRIX_PART_BTN";
}

lv_style_t *LVGLButtonMatrix::style(lv_obj_t *obj, lv_obj_part_t part) const {
  return lv_obj_get_local_style(obj, part);
}

void LVGLButtonMatrix::setStyle(lv_obj_t *obj, int type,
                                lv_style_t *style) const {
  lv_obj_add_style(obj, LV_BTN_PART_MAIN, style);
}

void LVGLButtonMatrix::addStyle(lv_obj_t *obj, lv_style_t *style,
                                lv_obj_part_t part) const {
  lv_obj_add_style(obj, part, style);
}

void LVGLButtonMatrix::initStateStyles() {
  for (int i = 0; i < 2; ++i) {
    lv_style_t *de = new lv_style_t;
    lv_style_t *ch = new lv_style_t;
    lv_style_t *fo = new lv_style_t;
    lv_style_t *ed = new lv_style_t;
    lv_style_t *ho = new lv_style_t;
    lv_style_t *pr = new lv_style_t;
    lv_style_t *di = new lv_style_t;
    lv_style_init(de);
    lv_style_init(ch);
    lv_style_init(fo);
    lv_style_init(ed);
    lv_style_init(ho);
    lv_style_init(pr);
    lv_style_init(di);
    QList<lv_style_t *> stateStyles;
    stateStyles << de << ch << fo << ed << ho << pr << di;
    m_partsStyles[i] = stateStyles;
  }
}

#include "LVGLDropDownList.h"

#include <QIcon>

#include "LVGLObject.h"

class LVGLPropertyDropdownDir : public LVGLPropertyEnum {
 public:
  LVGLPropertyDropdownDir()
      : LVGLPropertyEnum(QStringList() << "Down"
                                       << "Up"
                                       << "Left"
                                       << "Right"),
        m_values({"LV_DROPDOWN_DIR_DOWN", "LV_DROPDOWN_DIR_UP",
                  "LV_DROPDOWN_DIR_LEFT", "LV_DROPDOWN_DIR_RIGHT"}) {}

  QString name() const { return "Dir"; }

  QStringList function(LVGLObject *obj) const {
    if (get(obj) == LV_LABEL_ALIGN_LEFT) return QStringList();
    return QStringList() << QString("lv_dropdown_set_dir(%1, %2);")
                                .arg(obj->codeName())
                                .arg(m_values.at(get(obj)));
  }

 protected:
  int get(LVGLObject *obj) const { return lv_dropdown_get_dir(obj->obj()); }
  void set(LVGLObject *obj, int index) {
    lv_dropdown_set_dir(obj->obj(), index & 0xff);
  }

  QStringList m_values;
};

class LVGLPropertyDropdownOptions : public LVGLPropertyStringPlus {
 public:
  QString name() const { return "Options"; }

  QStringList function(LVGLObject *obj) const {
    return QStringList() << QString("lv_dropdown_set_options(%1, \"%2\");")
                                .arg(obj->codeName())
                                .arg(get(obj));
  }

 protected:
  QString get(LVGLObject *obj) const {
    return lv_dropdown_get_options(obj->obj());
  }
  void set(LVGLObject *obj, QString string) {
    lv_dropdown_set_options(obj->obj(), qUtf8Printable(string));
  }
};

class LVGLPropertyDropdownArrow : public LVGLPropertyEnum {
 public:
  LVGLPropertyDropdownArrow()
      : LVGLPropertyEnum(QStringList() << "Down"
                                       << "Up"
                                       << "Left"
                                       << "Right"),
        m_values({"LV_SYMBOL_DOWN", "LV_SYMBOL_UP", "LV_SYMBOL_LEFT",
                  "LV_SYMBOL_RIGHT"}) {}

  QString name() const { return "Arrow"; }

  QStringList function(LVGLObject *obj) const {
    if (get(obj) == LV_LABEL_ALIGN_LEFT) return QStringList();
    return QStringList() << QString("lv_dropdown_set_symbol(%1, %2);")
                                .arg(obj->codeName())
                                .arg(m_values.at(get(obj)));
  }

 protected:
  int get(LVGLObject *obj) const {
    auto arrow = lv_dropdown_get_symbol(obj->obj());

    if (arrow == LV_SYMBOL_DOWN)
      return 0;
    else if (arrow == LV_SYMBOL_UP)
      return 1;
    else if (arrow == LV_SYMBOL_LEFT)
      return 2;
    else if (arrow == LV_SYMBOL_RIGHT)
      return 3;
    else
      return 0;
  }
  void set(LVGLObject *obj, int index) {
    auto arrow = LV_SYMBOL_DOWN;
    switch (index) {
      case 0:
        arrow = LV_SYMBOL_DOWN;
        break;
      case 1:
        arrow = LV_SYMBOL_UP;
        break;
      case 2:
        arrow = LV_SYMBOL_LEFT;
        break;
      case 3:
        arrow = LV_SYMBOL_RIGHT;
        break;
      default:
        arrow = LV_SYMBOL_DOWN;
    }
    lv_dropdown_set_symbol(obj->obj(), arrow);
  }

  QStringList m_values;
};

class LVGLPropertyDropdownMaxHeight : public LVGLPropertyInt {
 public:
  LVGLPropertyDropdownMaxHeight() : LVGLPropertyInt(0, UINT16_MAX, "") {}

  QString name() const { return "Max height"; }

  QStringList function(LVGLObject *obj) const {
    return QStringList() << QString("lv_dropdown_set_max_height(%1,%2);")
                                .arg(obj->codeName())
                                .arg(m_value);
  }

 protected:
  int get(LVGLObject *obj) const {
    m_value = lv_dropdown_get_max_height(obj->obj());
    return m_value;
  }
  void set(LVGLObject *obj, int value) {
    lv_dropdown_set_max_height(obj->obj(), static_cast<uint16_t>(value));
  }

 private:
  mutable uint16_t m_value;
};

// class LVGLPropertyDropdownManuallyOC : public LVGLPropertyEnum {
// public:
//  LVGLPropertyDropdownManuallyOC()
//      : LVGLPropertyEnum(QStringList() << "Open"
//                                       << "Close"),
//        m_values({"LV_DROPDOWN_DIR_DOWN", "LV_DROPDOWN_DIR_UP"}) {}

//  QString name() const { return "Dir"; }

//  QStringList function(LVGLObject *obj) const {
//    if (get(obj) == LV_LABEL_ALIGN_LEFT) return QStringList();
//    return QStringList() << QString("lv_dropdown_set_dir(%1, %2);")
//                                .arg(obj->codeName())
//                                .arg(m_values.at(get(obj)));
//  }

// protected:
//  int get(LVGLObject *obj) const { return lv_dropdown_get_dir(obj->obj()); }
//  void set(LVGLObject *obj, int index) {
//    if (index == 0)
//      lv_dropdown_open(obj);
//    else

//      lv_dropdown_set_dir(obj->obj(), index & 0xff);
//  }

//  QStringList m_values;
//};

LVGLDropDownList::LVGLDropDownList() {
  initStateStyles();
  m_parts << LV_DROPDOWN_PART_MAIN << LV_DROPDOWN_PART_LIST
          << LV_DROPDOWN_PART_SCROLLBAR << LV_DROPDOWN_PART_SELECTED;

  m_properties << new LVGLPropertyDropdownOptions;
  m_properties << new LVGLPropertyDropdownDir;
  m_properties << new LVGLPropertyDropdownArrow;
  m_properties << new LVGLPropertyDropdownMaxHeight;
  //  m_properties << new LVGLPropertyDDListDrawArrow;
  //  m_properties << new LVGLPropertyDDListStayOpen;

  m_editableStyles << LVGL::DropdownMAIN;  // LV_DROPDOWN_PART_MAIN
  //  m_editableStyles << LVGL::DropdownLIST;      // LV_DROPDOWN_PART_LIST
  //  m_editableStyles << LVGL::DropdownDCRLBAR;   // LV_DROPDOWN_PART_SCROLLBAR
  //  m_editableStyles << LVGL::DropdownSELECTED;  // LV_DROPDOWN_PART_SELECTED
}

QString LVGLDropDownList::name() const { return "Dropdown"; }

QString LVGLDropDownList::className() const { return "lv_dropdown"; }

LVGLWidget::Type LVGLDropDownList::type() const { return DropDownList; }

QIcon LVGLDropDownList::icon() const { return QIcon(); }

lv_obj_t *LVGLDropDownList::newObject(lv_obj_t *parent) const {
  lv_obj_t *obj = lv_dropdown_create(parent, nullptr);
  return obj;
}

QSize LVGLDropDownList::minimumSize() const { return QSize(100, 35); }

QStringList LVGLDropDownList::styles() const {
  return QStringList() << "DROPDOWN_PART_MAIN";
  //                       << "DROPDOWN_PART_LIST"
  //                       << "DROPDOWN_PART_SCROLLBAR"
  //                       << "DROPDOWN_PART_SELECTED";
}

lv_style_t *LVGLDropDownList::style(lv_obj_t *obj, lv_obj_part_t part) const {
  return lv_obj_get_local_style(obj, part);
}

void LVGLDropDownList::setStyle(lv_obj_t *obj, int type,
                                lv_style_t *style) const {
  lv_obj_add_style(obj, LV_BTN_PART_MAIN, style);
}

void LVGLDropDownList::addStyle(lv_obj_t *obj, lv_style_t *style,
                                lv_obj_part_t part) const {
  lv_obj_add_style(obj, part, style);
}

void LVGLDropDownList::initStateStyles() {
  for (int i = 0; i < 1; ++i) {
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

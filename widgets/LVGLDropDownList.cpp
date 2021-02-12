#include "LVGLDropDownList.h"

#include <QIcon>

#include "LVGLObject.h"

// class LVGLPropertyDDListAlign : public LVGLPropertyEnum {
// public:
//  LVGLPropertyDDListAlign()
//      : LVGLPropertyEnum(QStringList() << "Left"
//                                       << "Center"
//                                       << "Right") {}
//  QString name() const { return "Align"; }

// protected:
//  int get(LVGLObject *obj) const { return lv_ddlist_get_align(obj->obj()); }
//  void set(LVGLObject *obj, int index) {
//    lv_ddlist_set_align(obj->obj(), index & 0xff);
//  }
//};

// class LVGLPropertyDDListFixedWidth : public LVGLPropertyCoord {
// public:
//  inline LVGLPropertyDDListFixedWidth(LVGLProperty *p = nullptr)
//      : LVGLPropertyCoord(Qt::Vertical, p) {}
//  inline QString name() const override { return "Fixed width"; }

// protected:
//  inline lv_coord_t get(LVGLObject *obj) const override {
//    return lv_obj_get_width(obj->obj());
//  }
//  inline void set(LVGLObject *obj, lv_coord_t value) override {
//    lv_ddlist_set_fix_width(obj->obj(), value);
//  }
//};

// class LVGLPropertyDDListFixedHeight : public LVGLPropertyCoord {
// public:
//  inline LVGLPropertyDDListFixedHeight(LVGLProperty *p = nullptr)
//      : LVGLPropertyCoord(Qt::Vertical, p) {}
//  inline QString name() const override { return "Fixed height"; }

// protected:
//  inline lv_coord_t get(LVGLObject *obj) const override {
//    return lv_ddlist_get_fix_height(obj->obj());
//  }
//  inline void set(LVGLObject *obj, lv_coord_t value) override {
//    lv_ddlist_set_fix_height(obj->obj(), value);
//  }
//};

// class LVGLPropertyDDListScrollbars : public LVGLPropertyEnum {
// public:
//  LVGLPropertyDDListScrollbars()
//      : LVGLPropertyEnum(QStringList() << "Off"
//                                       << "On"
//                                       << "Drag"
//                                       << "Auto") {}
//  QString name() const { return "Scrollbars"; }

// protected:
//  int get(LVGLObject *obj) const {
//    return lv_ddlist_get_sb_mode(obj->obj()) & 0x3;
//  }
//  void set(LVGLObject *obj, int index) {
//    lv_ddlist_set_sb_mode(obj->obj(), index & 0x3);
//  }
//};

// class LVGLPropertyDDListAnimationTime : public LVGLPropertyInt {
// public:
//  LVGLPropertyDDListAnimationTime() : LVGLPropertyInt(0, UINT16_MAX, " ms") {}

//  QString name() const { return "Animation time"; }

// protected:
//  int get(LVGLObject *obj) const {
//    return lv_dropdown_get_anim_time(obj->obj());
//  }
//  void set(LVGLObject *obj, int value) {
//    lv_dropdown_set_anim_time(obj->obj(), static_cast<uint16_t>(value));
//  }
//};

// class LVGLPropertyDDListDrawArrow : public LVGLPropertyBool {
// public:
//  QString name() const { return "Decoration arrow"; }

// protected:
//  bool get(LVGLObject *obj) const {
//    return lv_ddlist_get_draw_arrow(obj->obj());
//  }
//  void set(LVGLObject *obj, bool statue) {
//    lv_ddlist_set_draw_arrow(obj->obj(), statue);
//  }
//};

// class LVGLPropertyDDListStayOpen : public LVGLPropertyBool {
// public:
//  QString name() const { return "Stay open"; }

// protected:
//  bool get(LVGLObject *obj) const {
//    return lv_dropdown_get_show_selected(obj->obj());
//  }
//  void set(LVGLObject *obj, bool statue) {
//    lv_dropdown_set_stay_open(obj->obj(), statue);
//  }
//};

LVGLDropDownList::LVGLDropDownList() {
  initStateStyles();
  m_parts << LV_DROPDOWN_PART_MAIN << LV_DROPDOWN_PART_LIST
          << LV_DROPDOWN_PART_SCROLLBAR << LV_DROPDOWN_PART_SELECTED;
  //  m_properties << new LVGLPropertyDDListAlign;
  //  m_properties << new LVGLPropertyDDListFixedWidth;
  //  m_properties << new LVGLPropertyDDListFixedHeight;
  //  m_properties << new LVGLPropertyDDListScrollbars;
  //  m_properties << new LVGLPropertyDDListAnimationTime;
  //  m_properties << new LVGLPropertyDDListDrawArrow;
  //  m_properties << new LVGLPropertyDDListStayOpen;

  m_editableStyles << LVGL::DropdownMAIN;      // LV_DROPDOWN_PART_MAIN
  m_editableStyles << LVGL::DropdownLIST;      // LV_DROPDOWN_PART_LIST
  m_editableStyles << LVGL::DropdownDCRLBAR;   // LV_DROPDOWN_PART_SCROLLBAR
  m_editableStyles << LVGL::DropdownSELECTED;  // LV_DROPDOWN_PART_SELECTED
}

QString LVGLDropDownList::name() const { return "Drop down list"; }

QString LVGLDropDownList::className() const { return "lv_ddlist"; }

LVGLWidget::Type LVGLDropDownList::type() const { return DropDownList; }

QIcon LVGLDropDownList::icon() const { return QIcon(); }

lv_obj_t *LVGLDropDownList::newObject(lv_obj_t *parent) const {
  lv_obj_t *obj = lv_dropdown_create(parent, nullptr);
  return obj;
}

QSize LVGLDropDownList::minimumSize() const { return QSize(100, 35); }

QStringList LVGLDropDownList::styles() const {
  return QStringList() << "DROPDOWN_PART_MAIN"
                       << "DROPDOWN_PART_LIST"
                       << "DROPDOWN_PART_SCROLLBAR"
                       << "DROPDOWN_PART_SELECTED";
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
  for (int i = 0; i < 4; ++i) {
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

#include "LVGLRoller.h"

#include <QIcon>

#include "LVGLObject.h"

LVGLRoller::LVGLRoller() {
  m_defaultobj = lv_roller_create(m_parent, NULL);
  initStateStyles();
  m_parts << LV_ROLLER_PART_BG << LV_ROLLER_PART_SELECTED;
  m_editableStyles << LVGL::Body;  // LV_ROLLER_PART_BG
  m_editableStyles << LVGL::Body;  // LV_ROLLER_PART_SELECTED
}

QString LVGLRoller::name() const { return "Roller"; }

QString LVGLRoller::className() const { return "lv_roller"; }

LVGLWidget::Type LVGLRoller::type() const { return Roller; }

QIcon LVGLRoller::icon() const { return QIcon(); }

lv_obj_t *LVGLRoller::newObject(lv_obj_t *parent) const {
  lv_obj_t *obj = lv_roller_create(parent, nullptr);
  return obj;
}

QSize LVGLRoller::minimumSize() const { return QSize(100, 100); }

QStringList LVGLRoller::styles() const {
  return QStringList() << "LV_ROLLER_PART_BG"
                       << "LV_ROLLER_PART_SELECTED";
}

lv_style_t *LVGLRoller::style(lv_obj_t *obj, lv_obj_part_t part) const {
  return lv_obj_get_local_style(obj, part);
}

void LVGLRoller::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const {
  lv_obj_add_style(obj, LV_BTN_PART_MAIN, style);
}

void LVGLRoller::addStyle(lv_obj_t *obj, lv_style_t *style,
                          lv_obj_part_t part) const {
  lv_obj_add_style(obj, part, style);
}

void LVGLRoller::initStateStyles() {
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

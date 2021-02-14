#include "LVGLBar.h"

#include <QIcon>

#include "LVGLCore.h"
#include "LVGLObject.h"
#include "properties/LVGLPropertyRange.h"

class LVGLPropertyBarValue : public LVGLPropertyInt {
 public:
  LVGLPropertyBarValue() : LVGLPropertyInt(INT16_MIN, INT16_MAX) {}

  QString name() const { return "Value"; }

  QStringList function(LVGLObject *obj) const {
    return QStringList() << QString("lv_bar_set_value(%1, %2, LV_ANIM_ON);")
                                .arg(obj->codeName())
                                .arg(get(obj));
  }

 protected:
  int get(LVGLObject *obj) const { return lv_bar_get_value(obj->obj()); }
  void set(LVGLObject *obj, int value) {
    lv_bar_set_value(obj->obj(), static_cast<int16_t>(value), LV_ANIM_OFF);
  }
};

class LVGLPropertyBarRange : public LVGLPropertyRange {
 public:
  QStringList function(LVGLObject *obj) const {
    return {QString("lv_bar_set_range(%1, %2, %2);")
                .arg(obj->codeName())
                .arg(getMin(obj))
                .arg(getMax(obj))};
  }

 protected:
  int getMin(LVGLObject *obj) const { return lv_bar_get_min_value(obj->obj()); }
  int getMax(LVGLObject *obj) const { return lv_bar_get_max_value(obj->obj()); }
  void set(LVGLObject *obj, int min, int max) {
    lv_bar_set_range(obj->obj(), static_cast<int16_t>(min),
                     static_cast<int16_t>(max));
  }
};

LVGLBar::LVGLBar() {
  m_defaultobj = lv_bar_create(m_parent, NULL);
  initStateStyles();
  m_parts << LV_BAR_PART_BG;
  m_parts << LV_BAR_PART_INDIC;
  m_properties << new LVGLPropertyBarValue;
  m_properties << new LVGLPropertyBarRange;

  m_editableStyles << LVGL::Body;  // LV_BAR_STYLE_BG
  m_editableStyles << LVGL::Body;  // LV_BAR_STYLE_INDIC
}

QString LVGLBar::name() const { return "Bar"; }

QString LVGLBar::className() const { return "lv_bar"; }

LVGLWidget::Type LVGLBar::type() const { return Bar; }

QIcon LVGLBar::icon() const { return QIcon(); }

lv_obj_t *LVGLBar::newObject(lv_obj_t *parent) const {
  lv_obj_t *obj = lv_bar_create(parent, nullptr);
  _lv_obj_set_style_local_color(
      obj, LV_BAR_PART_BG,
      (LV_STATE_DEFAULT << LV_STYLE_STATE_POS) | LV_STYLE_BG_COLOR,
      lvgl.fromColor(QColor("#01a2b1")));
  return obj;
}

QSize LVGLBar::minimumSize() const { return QSize(200, 30); }

QStringList LVGLBar::styles() const {
  return QStringList() << "BAR_PART_BG"
                       << "BAR_PART_INDIC";
}

lv_style_t *LVGLBar::style(lv_obj_t *obj, lv_obj_part_t part) const {
  return lv_obj_get_local_style(obj, part);
}

void LVGLBar::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const {
  lv_obj_add_style(obj, LV_BAR_PART_BG, style);
}

void LVGLBar::addStyle(lv_obj_t *obj, lv_style_t *style,
                       lv_obj_part_t part) const {
  lv_obj_add_style(obj, part, style);
}

void LVGLBar::initStateStyles() {
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
    lv_style_set_bg_color(de, LV_STATE_DEFAULT,
                          lvgl.fromColor(QColor("#01a2b1")));
    QList<lv_style_t *> stateStyles;
    stateStyles << de << ch << fo << ed << ho << pr << di;
    m_partsStyles[i] = stateStyles;
  }
}

#include "LVGLArc.h"

#include <QIcon>

#include "LVGLObject.h"
#include "properties/LVGLPropertyVal2.h"

class LVGLPropertyArcRotation : public LVGLPropertyInt {
 public:
  LVGLPropertyArcRotation() : LVGLPropertyInt(0, 360, ""), m_rations(0) {}

  QString name() const { return "Rotation"; }

  QStringList function(LVGLObject *obj) const {
    return QStringList() << QString("lv_arc_set_rotation(%1,%2);")
                                .arg(obj->codeName())
                                .arg(m_widget->value());
  }

 protected:
  int get(LVGLObject *obj) const {
    Q_UNUSED(obj)
    return m_rations;
  }
  void set(LVGLObject *obj, int value) {
    m_rations = value;
    lv_arc_set_rotation(obj->obj(), static_cast<uint16_t>(value));
  }

 private:
  int m_rations;
};

LVGLArc::LVGLArc() {
  initStateStyles();
  m_parts << LV_ARC_PART_BG << LV_ARC_PART_INDIC << LV_ARC_PART_KNOB;
  m_properties << new LVGLPropertyVal2UInt16(
      0, 360, "Start", lv_arc_get_angle_start, 0, 360, "End",
      lv_arc_get_angle_end, "lv_arc_set_angles", lv_arc_set_angles, "Angles");

  m_properties << new LVGLPropertyArcRotation;

  m_editableStyles << LVGL::ArcBG     // LV_ARC_PART_BG
                   << LVGL::ArcINDIC  // LV_ARC_PART_INDIC
                   << LVGL::ArcKNOB;  // LV_ARC_PART_KNOB
}

QString LVGLArc::name() const { return "Arc"; }

QString LVGLArc::className() const { return "lv_arc"; }

LVGLWidget::Type LVGLArc::type() const { return Arc; }

QIcon LVGLArc::icon() const { return QIcon(); }

lv_obj_t *LVGLArc::newObject(lv_obj_t *parent) const {
  lv_obj_t *obj = lv_arc_create(parent, nullptr);
  return obj;
}

QSize LVGLArc::minimumSize() const { return {75, 75}; }

QStringList LVGLArc::styles() const {
  return QStringList() << "ARC_PART_BG"
                       << "ARC_PART_INDIC"
                       << "ARC_PART_KNOB";
}

lv_style_t *LVGLArc::style(lv_obj_t *obj, lv_obj_part_t part) const {
  return lv_obj_get_local_style(obj, part);
}

void LVGLArc::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const {
  lv_obj_add_style(obj, LV_BTN_PART_MAIN, style);
}

void LVGLArc::addStyle(lv_obj_t *obj, lv_style_t *style,
                       lv_obj_part_t part) const {
  lv_obj_add_style(obj, part, style);
}

void LVGLArc::initStateStyles() {
  for (int i = 0; i < 3; ++i) {
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

#include "LVGLLabel.h"

#include <QIcon>

#include "LVGLObject.h"
#include "properties/LVGLPropertyColor.h"

class LVGLPropertyLabelAlign : public LVGLPropertyEnum {
 public:
  LVGLPropertyLabelAlign()
      : LVGLPropertyEnum(QStringList() << "Left"
                                       << "Center"
                                       << "Right"),
        m_values({"LV_LABEL_ALIGN_LEFT", "LV_LABEL_ALIGN_CENTER",
                  "LV_LABEL_ALIGN_RIGHT"}) {}

  QString name() const { return "Align"; }

  QStringList function(LVGLObject *obj) const {
    if (get(obj) == LV_LABEL_ALIGN_LEFT) return QStringList();
    return QStringList() << QString("lv_label_set_align(%1, %2);")
                                .arg(obj->codeName())
                                .arg(m_values.at(get(obj)));
  }

 protected:
  int get(LVGLObject *obj) const { return lv_label_get_align(obj->obj()); }
  void set(LVGLObject *obj, int index) {
    lv_label_set_align(obj->obj(), index & 0xff);
  }

  QStringList m_values;
};

class LVGLPropertyLabelLongMode : public LVGLPropertyEnum {
 public:
  LVGLPropertyLabelLongMode()
      : LVGLPropertyEnum(QStringList() << "Expand"
                                       << "Break"
                                       << "Dot"
                                       << "Scroll"
                                       << "Circular scroll"
                                       << "Corp"),
        m_values({"LV_LABEL_LONG_EXPAND", "LV_LABEL_LONG_BREAK",
                  "LV_LABEL_LONG_DOT", "LV_LABEL_LONG_SROLL",
                  "LV_LABEL_LONG_SROLL_CIRC", "LV_LABEL_LONG_CROP"}) {}

  QString name() const { return "Long mode"; }

  QStringList function(LVGLObject *obj) const {
    if (get(obj) == LV_LABEL_LONG_EXPAND) return QStringList();
    return QStringList() << QString("lv_label_set_long_mode(%1, %2);")
                                .arg(obj->codeName())
                                .arg(m_values.at(get(obj)));
  }

 protected:
  int get(LVGLObject *obj) const { return lv_label_get_long_mode(obj->obj()); }
  void set(LVGLObject *obj, int index) {
    LV_LABEL_LONG_SROLL_CIRC;
    lv_label_set_long_mode(obj->obj(), index & 0xff);
  }

  QStringList m_values;
};

class LVGLPropertyLabelRecolor : public LVGLPropertyBool {
 public:
  QString name() const { return "Recolor"; }

  QStringList function(LVGLObject *obj) const {
    if (!get(obj)) return QStringList();
    return QStringList() << QString("lv_label_set_recolor(%1, %2);")
                                .arg(obj->codeName())
                                .arg(QVariant(get(obj)).toString());
  }

 protected:
  bool get(LVGLObject *obj) const { return lv_label_get_recolor(obj->obj()); }
  void set(LVGLObject *obj, bool boolean) {
    lv_label_set_recolor(obj->obj(), boolean);
  }
};

LVGLLabel::LVGLLabel() {
  initStateStyles();
  m_parts << LV_LABEL_PART_MAIN;
  m_properties << new LVGLPropertyLabelAlign;
  m_properties << new LVGLPropertyLabelLongMode;
  m_properties << new LVGLPropertyLabelRecolor;
  m_properties << new LVGLPropertyString("Text", "lv_label_set_text",
                                         lv_label_set_text, lv_label_get_text);

  // swap geometry in order to stop autosize
  const int index = m_properties.indexOf(m_geometryProp);
  qSwap(m_properties[index], m_properties.last());

  m_editableStyles << LVGL::Label;  // LV_LABEL_PART_MAIN
}

QString LVGLLabel::name() const { return "Label"; }

QString LVGLLabel::className() const { return "lv_label"; }

LVGLWidget::Type LVGLLabel::type() const { return Label; }

QIcon LVGLLabel::icon() const { return QIcon(); }

lv_obj_t *LVGLLabel::newObject(lv_obj_t *parent) const {
  lv_obj_t *obj = lv_label_create(parent, nullptr);
  return obj;
}

QSize LVGLLabel::minimumSize() const { return QSize(100, 35); }

QStringList LVGLLabel::styles() const {
  return QStringList() << "LABEL_PART_MAIN";
}
lv_style_t *LVGLLabel::style(lv_obj_t *obj, lv_obj_part_t part) const {
  return lv_obj_get_local_style(obj, part);
}

void LVGLLabel::setStyle(lv_obj_t *obj, int type, lv_style_t *style) const {
  lv_obj_add_style(obj, LV_BTN_PART_MAIN, style);
}

void LVGLLabel::addStyle(lv_obj_t *obj, lv_style_t *style,
                         lv_obj_part_t part) const {
  lv_obj_add_style(obj, part, style);
}

void LVGLLabel::initStateStyles() {
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

#include "LVGLButtonMatrix.h"

#include <QIcon>

#include "LVGLObject.h"

LVGLButtonMatrix::LVGLButtonMatrix() {
  m_defaultobj = lv_btnmatrix_create(m_parent, NULL);
  initStateStyles();
  m_parts << LV_BTNMATRIX_PART_BG << LV_BTNMATRIX_PART_BTN;
  m_editableStyles << LVGL::Background;  // LV_BTNMATRIX_PART_BG
  m_editableStyles << LVGL::Background;  // LV_BTNMATRIX_PART_BTN
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

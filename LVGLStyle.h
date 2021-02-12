#ifndef LVGLSTYLE_H
#define LVGLSTYLE_H

#include <lvgl/lvgl.h>

#include <QList>
#include <QString>
#include <QVariant>
class LVGLObject;

namespace LVGL {
enum StylePart {
  None = 0x00,
  Mix = 0x01,
  Padding = 0x02,
  Margin = 0x04,
  Backound = 0x08,
  Border = 0x10,
  Outline = 0x20,
  Shadow = 0x40,
  Pattern = 0x80,
  Value = 0x100,
  Text = 0x200,
  Line = 0x400,
  Image = 0x800,
  Transition = 0x1000,
  Scale = 0x2000,
  All = Mix | Padding | Margin | Backound | Border | Outline | Shadow |
        Pattern | Value | Text | Line | Image | Transition | Scale,
  Body = Mix | Backound | Border | Outline,
  Button =
      Mix | Backound | Border | Outline | Shadow | Value | Pattern | Transition,
  CalendarHead = Mix | Backound | Padding | Margin,
  Canvas = Image,
  Checkbox = Backound,
  Container = Backound | Padding,
  CPickerBG = Mix | Padding | Backound,
  CpickerKNOB = Mix | Padding | Backound,
  DropdownMAIN = Backound,
  DropdownLIST = Backound | Text,
  DropdownDCRLBAR = Backound,
  DropdownSELECTED = Backound,
  GaugeMAIN = Backound | Text | Line | Scale,
  GaugeMAJOR = Line | Scale,
  GaugeNEEDLE = Line | Backound,
  ImgMAIN = Backound | Image | Padding,
  KeyboardBTN = Backound | Text,
  Label = Backound | Padding | Text,
};
Q_DECLARE_FLAGS(StyleParts, StylePart)

}  // namespace LVGL
Q_DECLARE_OPERATORS_FOR_FLAGS(LVGL::StyleParts)

class LVGLStyleItem {
 public:
  enum Type {
    Color,
    Coord,
    Opacity,
    BorderPart,
    ShadowType,
    Font,
    Property,
    Graddir,
    Blendmode,
    BoolType,
    PointImg,
    PointChar,
    Align,
    TextDecor,
  };

  LVGLStyleItem(QString name, LVGL::StyleParts stylePart,
                LVGLStyleItem *parent = nullptr);
  LVGLStyleItem(QString name, Type type, lv_style_property_t prop,
                LVGL::StyleParts stylePart, LVGLStyleItem *parent = nullptr);
  LVGLStyleItem(QString name, LVGLStyleItem *parent = nullptr);
  ~LVGLStyleItem();

  QString name() const;

  const LVGLStyleItem *parent() const;
  int count() const;
  const LVGLStyleItem *child(int index) const;
  int row() const;
  int indexOf(const LVGLStyleItem *item) const;
  void addChild(LVGLStyleItem *child);

  Type type() const;

  lv_style_property_t prop() const;

  LVGL::StyleParts stylePart() const;

 protected:
  QString m_name;
  LVGLStyleItem *m_parent;
  Type m_type;
  size_t m_offset;
  lv_style_property_t m_prop;
  QList<LVGLStyleItem *> m_childs;
  LVGL::StyleParts m_stylePart;
};

class LVGLStyle : public LVGLStyleItem {
 public:
  LVGLStyle();

  QVariant get(const LVGLStyleItem *item) const;
  lv_border_side_t getBorderPart(const LVGLStyleItem *item) const;
  void set(const LVGLStyleItem *item, QVariant value);

  lv_style_t *style() const;
  void setStyle(lv_style_t *style);
  void setCurobj(lv_obj_t *obj) { m_curobj = obj; }
  void setState(lv_state_t state) { m_state = state; }
  void setPart(lv_obj_part_t part) { m_part = part; }

  static bool hasStyleChanged(const lv_style_t *style, const lv_style_t *base,
                              LVGL::StyleParts parts = LVGL::All);
  void changepart(lv_obj_part_t part) { m_part = part; }
  void changestate(lv_state_t state) { m_state = state; }
  void updateDate();

 private:
  lv_style_t *m_style;
  lv_obj_t *m_curobj;
  lv_obj_part_t m_part;
  lv_state_t m_state;
};

#endif  // LVGLSTYLE_H

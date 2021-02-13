#include "LVGLStyle.h"

#include <QColor>
#include <QDebug>

#include "LVGLCore.h"
#include "LVGLObject.h"

using lv_font_ref = const lv_font_t*;

LVGLStyleItem::LVGLStyleItem(QString name, LVGL::StyleParts stylePart,
                             LVGLStyleItem* parent)
    : m_name(name),
      m_parent(parent),
      m_type(Property),
      m_offset(std::numeric_limits<size_t>::max()),
      m_stylePart(stylePart) {}

LVGLStyleItem::LVGLStyleItem(QString name, Type type, lv_style_property_t prop,
                             LVGL::StyleParts stylePart, LVGLStyleItem* parent)
    : m_name(name),
      m_parent(parent),
      m_type(type),
      m_prop(prop),
      m_stylePart(stylePart) {}

LVGLStyleItem::~LVGLStyleItem() { qDeleteAll(m_childs); }

QString LVGLStyleItem::name() const { return m_name; }

const LVGLStyleItem* LVGLStyleItem::parent() const { return m_parent; }

int LVGLStyleItem::count() const { return m_childs.size(); }

const LVGLStyleItem* LVGLStyleItem::child(int index) const {
  return m_childs.at(index);
}

int LVGLStyleItem::row() const {
  return m_parent ? m_parent->indexOf(this) : 0;
}

int LVGLStyleItem::indexOf(const LVGLStyleItem* item) const {
  return m_childs.indexOf(const_cast<LVGLStyleItem*>(item));
}

void LVGLStyleItem::addChild(LVGLStyleItem* child) { m_childs.append(child); }

LVGLStyleItem::Type LVGLStyleItem::type() const { return m_type; }

lv_style_property_t LVGLStyleItem::prop() const { return m_prop; }

LVGL::StyleParts LVGLStyleItem::stylePart() const { return m_stylePart; }

LVGLStyle::LVGLStyle()
    : LVGLStyleItem("", LVGL::None),
      m_style(nullptr),
      m_part(LV_OBJ_PART_MAIN),
      m_state(LV_STATE_DEFAULT) {
  LVGLStyleItem* mixed = new LVGLStyleItem("mixed", LVGL::Mix, this);
  mixed->addChild(
      new LVGLStyleItem("radius", Coord, LV_STYLE_RADIUS, LVGL::Mix, mixed));
  mixed->addChild(new LVGLStyleItem("clip_corner", BoolType,
                                    LV_STYLE_CLIP_CORNER, LVGL::Mix, mixed));
  mixed->addChild(
      new LVGLStyleItem("size", Coord, LV_STYLE_SIZE, LVGL::Mix, mixed));
  mixed->addChild(new LVGLStyleItem(
      "transform_width", Coord, LV_STYLE_TRANSFORM_WIDTH, LVGL::Mix, mixed));
  mixed->addChild(new LVGLStyleItem(
      "transform_height", Coord, LV_STYLE_TRANSFORM_HEIGHT, LVGL::Mix, mixed));
  mixed->addChild(new LVGLStyleItem(
      "transform_angle", Coord, LV_STYLE_TRANSFORM_ANGLE, LVGL::Mix, mixed));
  mixed->addChild(new LVGLStyleItem("transform_zoom", Coord,
                                    LV_STYLE_TRANSFORM_ZOOM, LVGL::Mix, mixed));
  mixed->addChild(new LVGLStyleItem("opa_scale", Coord, LV_STYLE_OPA_SCALE,
                                    LVGL::Mix, mixed));

  LVGLStyleItem* padding = new LVGLStyleItem("padding", LVGL::Padding, this);
  padding->addChild(new LVGLStyleItem("pad_top", Coord, LV_STYLE_PAD_TOP,
                                      LVGL::Padding, padding));
  padding->addChild(new LVGLStyleItem("pad_bottom", Coord, LV_STYLE_PAD_BOTTOM,
                                      LVGL::Padding, padding));
  padding->addChild(new LVGLStyleItem("pad_left", Coord, LV_STYLE_PAD_LEFT,
                                      LVGL::Padding, padding));
  padding->addChild(new LVGLStyleItem("pad_right", Coord, LV_STYLE_PAD_RIGHT,
                                      LVGL::Padding, padding));
  padding->addChild(new LVGLStyleItem("pad_inner", Coord, LV_STYLE_PAD_INNER,
                                      LVGL::Padding, padding));

  LVGLStyleItem* margin = new LVGLStyleItem("margin", LVGL::Margin, this);
  margin->addChild(new LVGLStyleItem("margin_top", Coord, LV_STYLE_MARGIN_TOP,
                                     LVGL::Margin, margin));
  margin->addChild(new LVGLStyleItem(
      "margin_bottom", Coord, LV_STYLE_MARGIN_BOTTOM, LVGL::Margin, margin));
  margin->addChild(new LVGLStyleItem("margin_left", Coord, LV_STYLE_MARGIN_LEFT,
                                     LVGL::Margin, margin));
  margin->addChild(new LVGLStyleItem(
      "margin_right", Coord, LV_STYLE_MARGIN_RIGHT, LVGL::Margin, margin));

  LVGLStyleItem* background =
      new LVGLStyleItem("background", LVGL::Background, this);
  background->addChild(new LVGLStyleItem("bg_color", Color, LV_STYLE_BG_COLOR,
                                         LVGL::Background, background));
  background->addChild(new LVGLStyleItem("bg_opa", Opacity, LV_STYLE_BG_OPA,
                                         LVGL::Background, background));
  background->addChild(new LVGLStyleItem("bg_grad_color", Color,
                                         LV_STYLE_BG_GRAD_COLOR, LVGL::Background,
                                         background));
  background->addChild(new LVGLStyleItem("bg_main_stop", Coord,
                                         LV_STYLE_BG_MAIN_STOP, LVGL::Background,
                                         background));
  background->addChild(new LVGLStyleItem("bg_grad_stop", Coord,
                                         LV_STYLE_BG_GRAD_STOP, LVGL::Background,
                                         background));
  background->addChild(new LVGLStyleItem("bg_grad_dir", Graddir,
                                         LV_STYLE_BG_GRAD_DIR, LVGL::Background,
                                         background));
  background->addChild(new LVGLStyleItem("bg_blend_mode", Blendmode,
                                         LV_STYLE_BG_BLEND_MODE, LVGL::Background,
                                         background));

  LVGLStyleItem* border = new LVGLStyleItem("border", LVGL::Border, this);
  border->addChild(new LVGLStyleItem(
      "border_color", Color, LV_STYLE_BORDER_COLOR, LVGL::Border, border));
  border->addChild(new LVGLStyleItem("border_opa", Opacity, LV_STYLE_BORDER_OPA,
                                     LVGL::Border, border));
  border->addChild(new LVGLStyleItem(
      "border_width", Coord, LV_STYLE_BORDER_WIDTH, LVGL::Border, border));
  border->addChild(new LVGLStyleItem(
      "border_side", BorderPart, LV_STYLE_BORDER_POST, LVGL::Border, border));
  border->addChild(new LVGLStyleItem(
      "border_post", BoolType, LV_STYLE_BORDER_POST, LVGL::Border, border));
  border->addChild(new LVGLStyleItem("border_blend_mode", Blendmode,
                                     LV_STYLE_BORDER_BLEND_MODE, LVGL::Border,
                                     border));

  LVGLStyleItem* outline = new LVGLStyleItem("outline", LVGL::Outline, this);
  outline->addChild(new LVGLStyleItem(
      "outline_color", Color, LV_STYLE_OUTLINE_COLOR, LVGL::Outline, outline));
  outline->addChild(new LVGLStyleItem(
      "outline_opa", Opacity, LV_STYLE_OUTLINE_OPA, LVGL::Outline, outline));
  outline->addChild(new LVGLStyleItem(
      "outline_width", Coord, LV_STYLE_OUTLINE_WIDTH, LVGL::Outline, outline));
  outline->addChild(new LVGLStyleItem(
      "outline_pad", Coord, LV_STYLE_OUTLINE_PAD, LVGL::Outline, outline));
  outline->addChild(new LVGLStyleItem("outline_blend_mode", Blendmode,
                                      LV_STYLE_OUTLINE_BLEND_MODE,
                                      LVGL::Outline, outline));

  LVGLStyleItem* shadow = new LVGLStyleItem("shadow", LVGL::Shadow, this);
  shadow->addChild(new LVGLStyleItem(
      "shadow_color", Color, LV_STYLE_SHADOW_COLOR, LVGL::Shadow, shadow));
  shadow->addChild(new LVGLStyleItem("shadow_opa", Opacity, LV_STYLE_SHADOW_OPA,
                                     LVGL::Shadow, shadow));
  shadow->addChild(new LVGLStyleItem(
      "shadow_width", Coord, LV_STYLE_SHADOW_WIDTH, LVGL::Shadow, shadow));
  shadow->addChild(new LVGLStyleItem(
      "shadow_ofs_x", Coord, LV_STYLE_SHADOW_OFS_X, LVGL::Shadow, shadow));
  shadow->addChild(new LVGLStyleItem(
      "shadow_ofs_y", Coord, LV_STYLE_SHADOW_OFS_Y, LVGL::Shadow, shadow));
  shadow->addChild(new LVGLStyleItem(
      "shadow_spread", Coord, LV_STYLE_SHADOW_SPREAD, LVGL::Shadow, shadow));
  shadow->addChild(new LVGLStyleItem("shadow_blend_mode", Blendmode,
                                     LV_STYLE_SHADOW_BLEND_MODE, LVGL::Shadow,
                                     shadow));

  LVGLStyleItem* pattern = new LVGLStyleItem("pattern", LVGL::Pattern, this);
  pattern->addChild(new LVGLStyleItem("pattern_image", PointImg,
                                      LV_STYLE_PATTERN_IMAGE, LVGL::Pattern,
                                      pattern));
  pattern->addChild(new LVGLStyleItem(
      "pattern_opa", Opacity, LV_STYLE_PATTERN_OPA, LVGL::Pattern, pattern));
  pattern->addChild(new LVGLStyleItem("pattern_recolor", Color,
                                      LV_STYLE_PATTERN_RECOLOR, LVGL::Pattern,
                                      pattern));
  pattern->addChild(new LVGLStyleItem("pattern_recolor_opa", Opacity,
                                      LV_STYLE_PATTERN_RECOLOR_OPA,
                                      LVGL::Pattern, pattern));
  pattern->addChild(new LVGLStyleItem("pattern_repeat", BoolType,
                                      LV_STYLE_PATTERN_REPEAT, LVGL::Pattern,
                                      pattern));
  pattern->addChild(new LVGLStyleItem("pattern_blend_mode", Blendmode,
                                      LV_STYLE_PATTERN_BLEND_MODE,
                                      LVGL::Pattern, pattern));

  LVGLStyleItem* value = new LVGLStyleItem("value", LVGL::Value, this);
  value->addChild(new LVGLStyleItem("value_str", PointChar, LV_STYLE_VALUE_STR,
                                    LVGL::Value, value));
  value->addChild(new LVGLStyleItem("value_color", Color, LV_STYLE_VALUE_COLOR,
                                    LVGL::Value, value));
  value->addChild(new LVGLStyleItem("value_opa", Opacity, LV_STYLE_VALUE_OPA,
                                    LVGL::Value, value));
  value->addChild(new LVGLStyleItem("value_font", Font, LV_STYLE_VALUE_FONT,
                                    LVGL::Value, value));
  value->addChild(new LVGLStyleItem("value_letter_space", Coord,
                                    LV_STYLE_VALUE_LETTER_SPACE, LVGL::Value,
                                    value));
  value->addChild(new LVGLStyleItem("value_line_space", Coord,
                                    LV_STYLE_VALUE_LINE_SPACE, LVGL::Value,
                                    value));
  value->addChild(new LVGLStyleItem("value_align", Align, LV_STYLE_VALUE_ALIGN,
                                    LVGL::Value, value));
  value->addChild(new LVGLStyleItem("value_ofs_x", Coord, LV_STYLE_VALUE_OFS_X,
                                    LVGL::Value, value));
  value->addChild(new LVGLStyleItem("value_ofs_y", Coord, LV_STYLE_VALUE_OFS_Y,
                                    LVGL::Value, value));
  value->addChild(new LVGLStyleItem("value_blend_mode", Blendmode,
                                    LV_STYLE_VALUE_BLEND_MODE, LVGL::Value,
                                    value));

  LVGLStyleItem* text = new LVGLStyleItem("text", LVGL::Text, this);
  text->addChild(new LVGLStyleItem("text_color", Color, LV_STYLE_TEXT_COLOR,
                                   LVGL::Text, text));
  text->addChild(new LVGLStyleItem("text_opa", Opacity, LV_STYLE_TEXT_OPA,
                                   LVGL::Text, text));
  text->addChild(new LVGLStyleItem("text_font", Font, LV_STYLE_TEXT_FONT,
                                   LVGL::Text, text));
  text->addChild(new LVGLStyleItem("text_letter_space", Coord,
                                   LV_STYLE_TEXT_LETTER_SPACE, LVGL::Text,
                                   text));
  text->addChild(new LVGLStyleItem("text_line_space", Coord,
                                   LV_STYLE_TEXT_LINE_SPACE, LVGL::Text, text));
  text->addChild(new LVGLStyleItem("text_decor", TextDecor, LV_STYLE_TEXT_DECOR,
                                   LVGL::Text, text));
  text->addChild(new LVGLStyleItem("text_sel_color", Color,
                                   LV_STYLE_TEXT_SEL_COLOR, LVGL::Text, text));
  text->addChild(new LVGLStyleItem("text_sel_bg_color", Color,
                                   LV_STYLE_TEXT_SEL_BG_COLOR, LVGL::Text,
                                   text));
  text->addChild(new LVGLStyleItem("text_blend_mode", Blendmode,
                                   LV_STYLE_TEXT_BLEND_MODE, LVGL::Text, text));

  LVGLStyleItem* line = new LVGLStyleItem("line", LVGL::Line, this);
  line->addChild(new LVGLStyleItem("line_color", Color, LV_STYLE_LINE_COLOR,
                                   LVGL::Line, line));
  line->addChild(new LVGLStyleItem("line_width", Coord, LV_STYLE_LINE_WIDTH,
                                   LVGL::Line, line));
  line->addChild(new LVGLStyleItem("line_opa", Opacity, LV_STYLE_LINE_OPA,
                                   LVGL::Line, line));
  line->addChild(new LVGLStyleItem("line_dash_width", Coord,
                                   LV_STYLE_LINE_DASH_WIDTH, LVGL::Line, line));
  line->addChild(new LVGLStyleItem("line_dash_gap", Coord,
                                   LV_STYLE_LINE_DASH_GAP, LVGL::Line, line));
  line->addChild(new LVGLStyleItem("line_rounded", BoolType,
                                   LV_STYLE_LINE_ROUNDED, LVGL::Line, line));
  line->addChild(new LVGLStyleItem("line_blend_mode", Blendmode,
                                   LV_STYLE_LINE_BLEND_MODE, LVGL::Line, line));

  LVGLStyleItem* image = new LVGLStyleItem("image", LVGL::Image, this);
  image->addChild(new LVGLStyleItem(
      "image_recolor", Color, LV_STYLE_IMAGE_RECOLOR, LVGL::Image, image));
  image->addChild(new LVGLStyleItem("image_recolor_opa", Opacity,
                                    LV_STYLE_IMAGE_RECOLOR_OPA, LVGL::Image,
                                    image));
  image->addChild(new LVGLStyleItem("image_opa", Opacity, LV_STYLE_IMAGE_OPA,
                                    LVGL::Image, image));
  image->addChild(new LVGLStyleItem("image_blend_mode", Blendmode,
                                    LV_STYLE_IMAGE_BLEND_MODE, LVGL::Image,
                                    image));

  LVGLStyleItem* transition =
      new LVGLStyleItem("transition", LVGL::Transition, this);
  transition->addChild(new LVGLStyleItem("transition_time", Coord,
                                         LV_STYLE_TRANSITION_TIME,
                                         LVGL::Transition, transition));
  transition->addChild(new LVGLStyleItem("transition_delay", Coord,
                                         LV_STYLE_TRANSITION_DELAY,
                                         LVGL::Transition, transition));
  transition->addChild(new LVGLStyleItem("transition_prop_1", Coord,
                                         LV_STYLE_TRANSITION_PROP_1,
                                         LVGL::Transition, transition));
  transition->addChild(new LVGLStyleItem("transition_prop_2", Coord,
                                         LV_STYLE_TRANSITION_PROP_2,
                                         LVGL::Transition, transition));
  transition->addChild(new LVGLStyleItem("transition_prop_3", Coord,
                                         LV_STYLE_TRANSITION_PROP_3,
                                         LVGL::Transition, transition));
  transition->addChild(new LVGLStyleItem("transition_prop_4", Coord,
                                         LV_STYLE_TRANSITION_PROP_4,
                                         LVGL::Transition, transition));
  transition->addChild(new LVGLStyleItem("transition_prop_5", Coord,
                                         LV_STYLE_TRANSITION_PROP_5,
                                         LVGL::Transition, transition));
  transition->addChild(new LVGLStyleItem("transition_prop_6", Coord,
                                         LV_STYLE_TRANSITION_PROP_6,
                                         LVGL::Transition, transition));

  LVGLStyleItem* scale = new LVGLStyleItem("scale", LVGL::Transition, this);
  scale->addChild(new LVGLStyleItem("scale_grad_color", Color,
                                    LV_STYLE_SCALE_GRAD_COLOR, LVGL::Scale,
                                    scale));
  scale->addChild(new LVGLStyleItem(
      "scale_end_color", Color, LV_STYLE_SCALE_END_COLOR, LVGL::Scale, scale));
  scale->addChild(new LVGLStyleItem("scale_width", Coord, LV_STYLE_SCALE_WIDTH,
                                    LVGL::Scale, scale));
  scale->addChild(new LVGLStyleItem("scale_border_width", Coord,
                                    LV_STYLE_SCALE_BORDER_WIDTH, LVGL::Scale,
                                    scale));
  scale->addChild(new LVGLStyleItem("scale_end_border_width", Coord,
                                    LV_STYLE_SCALE_END_BORDER_WIDTH,
                                    LVGL::Scale, scale));
  scale->addChild(new LVGLStyleItem("scale_end_line_width", Coord,
                                    LV_STYLE_SCALE_END_LINE_WIDTH, LVGL::Scale,
                                    scale));

  m_childs << mixed << padding << margin << background << border << outline
           << shadow << pattern << value << text << line << image << transition
           << scale;
}

QVariant LVGLStyle::get(const LVGLStyleItem* item) const {
  if ((m_style == nullptr) || (m_curobj == nullptr) ||
      (item->type() == Property))
    return QVariant();
  auto type = item->prop();
  if (item->type() == Coord) {
    lv_coord_t c = _lv_obj_get_style_int(
        m_curobj, m_part, (m_state << LV_STYLE_STATE_POS) | type);
    return QVariant(c);
  } else if (item->type() == Opacity) {
    lv_opa_t c = _lv_obj_get_style_opa(m_curobj, m_part,
                                       (m_state << LV_STYLE_STATE_POS) | type);
    return QVariant(c);
  } else if (item->type() == Color) {
    lv_color_t c = _lv_obj_get_style_color(
        m_curobj, m_part, (m_state << LV_STYLE_STATE_POS) | type);
    return lvgl.toColor(c);
  } else if (item->type() == BorderPart) {
    lv_border_side_t c = _lv_obj_get_style_int(
        m_curobj, m_part, (m_state << LV_STYLE_STATE_POS) | type);
    QStringList props;
    if (c == LV_BORDER_SIDE_NONE)
      return "None";
    else if (c == LV_BORDER_SIDE_FULL)
      return "Full";
    else if (c == LV_BORDER_SIDE_INTERNAL)
      return "Internal";

    if (c & 0x01) props << "Bottom";
    if (c & 0x02) props << "Top";
    if (c & 0x04) props << "Left";
    if (c & 0x08) props << "Right";
    return props.join(" | ");
  } else if (item->type() == Font) {
    const lv_font_t* c = (lv_font_t*)_lv_obj_get_style_ptr(
        m_curobj, m_part, (m_state << LV_STYLE_STATE_POS) | type);
    const int index = lvgl.indexOfFont(c);
    if (index == -1) {
      const void* f = (void*)lvgl.font("lv_font_chinese_16");
      _lv_obj_set_style_local_ptr(m_curobj, m_part,
                                  (m_state << LV_STYLE_STATE_POS) | type, f);
      c = (lv_font_t*)_lv_obj_get_style_ptr(
          m_curobj, m_part, (m_state << LV_STYLE_STATE_POS) | type);
      const int index2 = lvgl.indexOfFont(c);
      return lvgl.fontNames().at(index2);
    } else
      return lvgl.fontNames().at(index);
  } else if (item->type() == BoolType) {
    bool b = _lv_obj_get_style_int(m_curobj, m_part,
                                   (m_state << LV_STYLE_STATE_POS) | type);
    return b;
  } else if (item->type() == Blendmode) {
    lv_blend_mode_t c = _lv_obj_get_style_int(
        m_curobj, m_part, (m_state << LV_STYLE_STATE_POS) | type);
    switch (c) {
      case LV_BLEND_MODE_NORMAL:
        return "Normal";
      case LV_BLEND_MODE_ADDITIVE:
        return "Additive";
      case LV_BLEND_MODE_SUBTRACTIVE:
        return "SubtrActive";
      default:
        return "Normal";
    }
  } else if (item->type() == Graddir) {
    lv_grad_dir_t c = _lv_obj_get_style_int(
        m_curobj, m_part, (m_state << LV_STYLE_STATE_POS) | type);
    switch (c) {
      case LV_GRAD_DIR_NONE:
        return "None";
      case LV_GRAD_DIR_VER:
        return "Vertical ";
      case LV_GRAD_DIR_HOR:
        return "Horizontal ";
      default:
        return "None";
    }
  } else if (item->type() == PointChar) {
    const char* c = (const char*)_lv_obj_get_style_ptr(
        m_curobj, m_part, (m_state << LV_STYLE_STATE_POS) | type);
    QString text(c);
    return text;
  }
  return QVariant();
}

lv_border_side_t LVGLStyle::getBorderPart(const LVGLStyleItem* item) const {
  if (item->type() == BorderPart)
    return lv_obj_get_style_border_side(m_curobj, m_part);
  return 0;
}

template <class T, class P>
void set_helper(P value, size_t offset, lv_style_t* style) {
  T& c = *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(style) + offset);
  c = static_cast<T>(value);
}

void LVGLStyle::set(const LVGLStyleItem* item, QVariant value) {
  if ((m_style == nullptr) || (item->type() == Property)) return;
  auto type = item->prop();

  if (item->type() == Coord) {
    lv_coord_t c = value.toInt();
    _lv_obj_set_style_local_int(m_curobj, m_part,
                                type | (m_state << LV_STYLE_STATE_POS), c);
  } else if (item->type() == Opacity) {
    lv_opa_t c = value.toInt();

    _lv_obj_set_style_local_opa(m_curobj, m_part,
                                type | (m_state << LV_STYLE_STATE_POS), c);
  } else if (item->type() == Color) {
    lv_color_t c = lvgl.fromColor(value);
    _lv_obj_set_style_local_color(m_curobj, m_part,
                                  type | (m_state << LV_STYLE_STATE_POS), c);
  } else if (item->type() == Font) {
    const lv_font_t* font = lvgl.font(value.toString());
    _lv_obj_set_style_local_ptr(m_curobj, m_part,
                                type | (m_state << LV_STYLE_STATE_POS), font);
  } else if (item->type() == BorderPart) {
    lv_border_side_t side = value.toUInt();
    _lv_obj_set_style_local_int(m_curobj, m_part,
                                type | (m_state << LV_STYLE_STATE_POS), side);
  } else if (item->type() == BoolType) {
    bool b = value.toBool();
    _lv_obj_set_style_local_int(m_curobj, m_part,
                                type | (m_state << LV_STYLE_STATE_POS), b);
  } else if (item->type() == Blendmode) {
    lv_blend_mode_t c = value.toInt();
    _lv_obj_set_style_local_int(m_curobj, m_part,
                                type | (m_state << LV_STYLE_STATE_POS), c);
  } else if (item->type() == Graddir) {
    lv_grad_dir_t c = value.toInt();
    lv_obj_set_style_local_bg_grad_dir(m_curobj, m_part, m_state, c);
    _lv_obj_set_style_local_int(m_curobj, m_part,
                                type | (m_state << LV_STYLE_STATE_POS), c);
  } else if (item->type() == PointChar) {
    auto size = value.toString().toUtf8().size();
    char* p = new char(size + 1);
    memcpy(p, value.toString().toUtf8().data(), size);
    p[size] = '\0';
    _lv_obj_set_style_local_ptr(m_curobj, m_part,
                                type | (m_state << LV_STYLE_STATE_POS), p);
  }

  lv_obj_add_style(m_curobj, m_part, m_style);
}

lv_style_t* LVGLStyle::style() const { return m_style; }

void LVGLStyle::setStyle(lv_style_t* style) { m_style = style; }

bool LVGLStyle::hasStyleChanged(const lv_style_t* style, const lv_style_t* base,
                                LVGL::StyleParts parts) {
  if (parts & LVGL::Background) {
    if (axs_style_equal_bg_color(style, base))
      return true;
    else if (axs_style_equal_bg_grad_color(style, base))
      return true;
    else if (axs_style_equal_radius(style, base))
      return true;
    else if (axs_style_equal_bg_opa(style, base))
      return true;
    if (parts & LVGL::Border) {
      if (axs_style_equal_border_color(style, base))
        return true;
      else if (axs_style_equal_border_width(style, base))
        return true;
      else if (axs_style_equal_border_side(style, base))
        return true;
      else if (axs_style_equal_bg_opa(style, base))
        return true;
    }
    if (parts & LVGL::Shadow) {
      if (axs_style_equal_shadow_color(style, base))
        return true;
      else if (axs_style_equal_shadow_width(style, base))
        return true;
    }
    if (parts & LVGL::Padding) {
      if (axs_style_equal_pad_top(style, base))
        return true;
      else if (axs_style_equal_pad_bottom(style, base))
        return true;
      else if (axs_style_equal_pad_left(style, base))
        return true;
      else if (axs_style_equal_pad_right(style, base))
        return true;
      else if (axs_style_equal_pad_inner(style, base))
        return true;
    }
  }
  if (parts & LVGL::Text) {
    if (axs_style_equal_text_color(style, base))
      return true;
    else if (axs_style_equal_text_sel_color(style, base))
      return true;
    else if (axs_style_equal_text_font(style, base))
      return true;
    else if (axs_style_equal_text_letter_space(style, base))
      return true;
    else if (axs_style_equal_text_line_space(style, base))
      return true;
    else if (axs_style_equal_text_opa(style, base))
      return true;
  }
  if (parts & LVGL::Image) {
    if (axs_style_equal_image_recolor(style, base))
      return true;
    else if (axs_style_equal_image_opa(style, base))
      return true;
  }
  if (parts & LVGL::Line) {
    if (axs_style_equal_line_color(style, base))
      return true;
    else if (axs_style_equal_line_width(style, base))
      return true;
    else if (axs_style_equal_line_opa(style, base))
      return true;
  }
  return false;
}

void LVGLStyle::updateDate() { lv_obj_add_style(m_curobj, m_part, m_style); }

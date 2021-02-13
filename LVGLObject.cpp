#include "LVGLObject.h"

#include <QDebug>
#include <QJsonArray>

#include "LVGLCore.h"

LVGLObject::LVGLObject(const LVGLWidget *widgetClass, QString name,
                       LVGLObject *parent)
    : m_obj(widgetClass->newObject(parent->obj())),
      m_widgetClass(widgetClass),
      m_name(name),
      m_locked(false),
      m_accessible(false),
      m_movable(true),
      m_index(-1),
      m_parent(parent) {
  if (parent) parent->m_childs.append(this);
  if (m_name.isEmpty()) generateName();
}

LVGLObject::LVGLObject(const LVGLWidget *widgetClass, QString name,
                       lv_obj_t *parent)
    : m_obj(widgetClass->newObject(parent)),
      m_widgetClass(widgetClass),
      m_name(name),
      m_locked(false),
      m_accessible(false),
      m_movable(true),
      m_index(-1),
      m_parent(nullptr) {
  if (m_name.isEmpty()) generateName();
}

LVGLObject::LVGLObject(const LVGLWidget *widgetClass, LVGLObject *parent,
                       lv_obj_t *parentObj)
    : m_obj(widgetClass->newObject(parentObj)),
      m_widgetClass(widgetClass),
      m_locked(false),
      m_accessible(false),
      m_movable(true),
      m_index(-1),
      m_parent(parent) {
  generateName();
}

LVGLObject::LVGLObject(lv_obj_t *obj, const LVGLWidget *widgetClass,
                       LVGLObject *parent, bool movable, int index)
    : m_obj(obj),
      m_widgetClass(widgetClass),
      m_locked(false),
      m_accessible(false),
      m_movable(movable),
      m_index(index),
      m_parent(parent) {
  Q_ASSERT(m_widgetClass);
  if (parent) parent->m_childs.append(this);
  generateName();
}

LVGLObject::~LVGLObject() {
  lv_obj_del(m_obj);
  for (auto s : m_liststyles) delete s;
}

lv_obj_t *LVGLObject::obj() const { return m_obj; }

QString LVGLObject::name() const { return m_name; }

void LVGLObject::setName(const QString &name) {
  if (doesNameExists(name, this))
    generateName();
  else
    m_name = name;
}

QString LVGLObject::codeName() const {
  return name().toLower().replace(" ", "_");
}

bool LVGLObject::isLocked() const { return m_locked; }

void LVGLObject::setLocked(bool locked) { m_locked = locked; }

bool LVGLObject::isMovable() const { return m_movable; }

const LVGLWidget *LVGLObject::widgetClass() const { return m_widgetClass; }

LVGLWidget::Type LVGLObject::widgetType() const {
  return m_widgetClass->type();
}

QPoint LVGLObject::absolutePosition() const {
  // if (lv_obj_get_parent(m_obj) == lv_scr_act())
  //	return QPoint(m_obj->coords.x1, m_obj->coords.x2);
  int x = lv_obj_get_x(m_obj);
  int y = lv_obj_get_y(m_obj);
  lv_obj_t *parent = lv_obj_get_parent(m_obj);
  while (parent) {
    if (parent == lv_scr_act()) break;
    x += lv_obj_get_x(parent);
    y += lv_obj_get_y(parent);
    parent = lv_obj_get_parent(parent);
  }
  return QPoint(x, y);
}

QPoint LVGLObject::position() const {
  return QPoint(lv_obj_get_x(m_obj), lv_obj_get_y(m_obj));
}

int LVGLObject::x() const { return lv_obj_get_x(m_obj); }

int LVGLObject::y() const { return lv_obj_get_y(m_obj); }

void LVGLObject::setPosition(const QPoint &pos) {
  if (pos != position()) {
    lv_obj_set_pos(m_obj, static_cast<lv_coord_t>(pos.x()),
                   static_cast<lv_coord_t>(pos.y()));
    emit positionChanged();
  }
}

void LVGLObject::setX(int x) {
  if (x != lv_obj_get_x(m_obj)) {
    lv_obj_set_x(m_obj, static_cast<lv_coord_t>(x));
    emit positionChanged();
  }
}

void LVGLObject::setY(int y) {
  if (y != lv_obj_get_y(m_obj)) {
    lv_obj_set_y(m_obj, static_cast<lv_coord_t>(y));
    emit positionChanged();
  }
}

QSize LVGLObject::size() const {
  return QSize(lv_obj_get_width(m_obj), lv_obj_get_height(m_obj));
}

QRect LVGLObject::geometry() const { return QRect(absolutePosition(), size()); }

void LVGLObject::setGeometry(QRect geometry) {
  setPosition(geometry.topLeft());
  lv_obj_set_size(m_obj, static_cast<lv_coord_t>(geometry.width()),
                  static_cast<lv_coord_t>(geometry.height()));
}

int LVGLObject::width() const { return lv_obj_get_width(m_obj); }

int LVGLObject::height() const { return lv_obj_get_height(m_obj); }

void LVGLObject::setWidth(int width) {
  lv_obj_set_width(m_obj, static_cast<lv_coord_t>(width));
}

void LVGLObject::setHeight(int height) {
  lv_obj_set_height(m_obj, static_cast<lv_coord_t>(height));
}

QJsonObject LVGLObject::toJson() {
  QJsonObject object({{"widget", m_widgetClass->name()},
                      {"class", m_widgetClass->className()},
                      {"name", m_name}});
  if (m_index > -1) object.insert("index", m_index);
  for (LVGLProperty *p : m_widgetClass->properties()) {
    QJsonValue v = p->toJson(this);
    if (!v.isNull()) object.insert(p->name().toLower(), v);
  }

  QJsonArray styles = jsonStyles();
  if (!m_styles.isEmpty() && (styles.size() > 0))
    object.insert("styles", styles);
  if (!m_childs.isEmpty()) {
    QJsonArray childs;
    for (LVGLObject *c : m_childs) childs.append(c->toJson());
    object.insert("children", childs);
  }
  return object;
}

QJsonArray LVGLObject::jsonStyles() const {
  QJsonArray styles;
  for (auto it = m_styles.begin(); it != m_styles.end(); ++it) {
    const lv_style_t &objStyle = it.value();
    const lv_style_t *defaultStyle = m_widgetClass->style(m_obj);

    const LVGL::StyleParts editableParts =
        m_widgetClass->editableStyles(it.key());
    if (defaultStyle &&
        !LVGLStyle::hasStyleChanged(&objStyle, defaultStyle, editableParts))
      continue;

    QJsonObject style;
    style.insert("type", m_widgetClass->styles().at(it.key()));

    QString baseStyle = lvgl.baseStyleName(defaultStyle);
    if (!baseStyle.isEmpty()) style.insert("base", baseStyle);

    if (editableParts & LVGL::Background) {
      QJsonObject body;
      if (!defaultStyle || (axs_style_equal_bg_color(defaultStyle, &objStyle)))
        body.insert("main_color",
                    QVariant(lvgl.toColor(axs_lv_style_get_bg_color(&objStyle)))
                        .toString());
      if (!defaultStyle ||
          (axs_style_equal_bg_grad_color(defaultStyle, &objStyle)))
        body.insert(
            "grad_color",
            QVariant(lvgl.toColor(axs_lv_style_get_bg_grad_color(&objStyle)))
                .toString());
      if (!defaultStyle || (axs_style_equal_radius(defaultStyle, &objStyle)))
        body.insert("radius",
                    static_cast<int>(axs_lv_style_get_radius(&objStyle)));
      if (!defaultStyle || (axs_style_equal_bg_opa(defaultStyle, &objStyle)))
        body.insert("opa", axs_lv_style_get_bg_opa(&objStyle));

      if (editableParts & LVGL::Border) {
        QJsonObject border;
        if (!defaultStyle ||
            (axs_style_equal_border_color(defaultStyle, &objStyle)))
          border.insert(
              "color",
              QVariant(lvgl.toColor(axs_lv_style_get_border_color(&objStyle)))
                  .toString());
        if (!defaultStyle ||
            (axs_style_equal_border_width(defaultStyle, &objStyle)))
          border.insert("width", axs_lv_style_get_border_width(&objStyle));
        if (!defaultStyle ||
            (axs_style_equal_border_side(defaultStyle, &objStyle)))
          border.insert("part", axs_lv_style_get_border_side(&objStyle));
        if (!defaultStyle ||
            (axs_style_equal_border_opa(defaultStyle, &objStyle)))
          border.insert("opa", axs_lv_style_get_bg_opa(&objStyle));
        if (!border.isEmpty()) body.insert("border", border);
      }

      if (editableParts & LVGL::Shadow) {
        QJsonObject shadow;
        if (!defaultStyle ||
            (axs_style_equal_shadow_color(defaultStyle, &objStyle)))
          shadow.insert(
              "color",
              QVariant(lvgl.toColor(axs_lv_style_get_shadow_color(&objStyle)))
                  .toString());
        if (!defaultStyle ||
            (axs_style_equal_shadow_width(defaultStyle, &objStyle)))
          shadow.insert("width", axs_lv_style_get_shadow_width(&objStyle));
        if (!shadow.isEmpty()) body.insert("shadow", shadow);
      }

      if (editableParts & LVGL::Padding) {
        QJsonObject padding;
        if (!defaultStyle || (axs_style_equal_pad_top(defaultStyle, &objStyle)))
          padding.insert("top", axs_lv_style_get_pad_top(&objStyle));
        if (!defaultStyle ||
            (axs_style_equal_pad_bottom(defaultStyle, &objStyle)))
          padding.insert("bottom", axs_lv_style_get_pad_bottom(&objStyle));
        if (!defaultStyle ||
            (axs_style_equal_pad_left(defaultStyle, &objStyle)))
          padding.insert("left", axs_lv_style_get_pad_left(&objStyle));
        if (!defaultStyle ||
            (axs_style_equal_pad_right(defaultStyle, &objStyle)))
          padding.insert("right", axs_lv_style_get_pad_right(&objStyle));
        if (!defaultStyle ||
            (axs_style_equal_pad_inner(defaultStyle, &objStyle)))
          padding.insert("inner", axs_lv_style_get_pad_inner(&objStyle));
        if (!padding.isEmpty()) body.insert("padding", padding);
      }
      if (!body.isEmpty()) style.insert("background", body);
    }

    if (editableParts & LVGL::Text) {
      QJsonObject text;
      if (!defaultStyle ||
          (axs_style_equal_text_color(defaultStyle, &objStyle)))
        text.insert(
            "color",
            QVariant(lvgl.toColor(axs_lv_style_get_text_color(&objStyle)))
                .toString());
      if (!defaultStyle ||
          (axs_style_equal_text_sel_color(defaultStyle, &objStyle)))
        text.insert(
            "sel_color",
            QVariant(lvgl.toColor(axs_lv_style_get_text_sel_color(&objStyle)))
                .toString());
      if (!defaultStyle ||
          (axs_style_equal_text_font(defaultStyle, &objStyle))) {
        text.insert("font",
                    lvgl.fontName(axs_lv_style_get_text_font(&objStyle)));
      }
      if (!defaultStyle ||
          (axs_style_equal_text_letter_space(defaultStyle, &objStyle)))
        text.insert("letter_space",
                    axs_lv_style_get_text_letter_space(&objStyle));
      if (!defaultStyle ||
          (axs_style_equal_text_line_space(defaultStyle, &objStyle)))
        text.insert("line_space", axs_lv_style_get_text_line_space(&objStyle));
      if (!defaultStyle || (axs_style_equal_text_opa(defaultStyle, &objStyle)))
        text.insert("opa", axs_lv_style_get_text_opa(&objStyle));
      if (!text.isEmpty()) style.insert("text", text);
    }

    if (editableParts & LVGL::Image) {
      QJsonObject image;
      if (!defaultStyle ||
          (axs_style_equal_image_recolor(defaultStyle, &objStyle)))
        image.insert(
            "recolor",
            QVariant(lvgl.toColor(axs_lv_style_get_image_recolor(&objStyle)))
                .toString());
      if (!defaultStyle || (axs_style_equal_image_opa(defaultStyle, &objStyle)))
        image.insert("opa", axs_lv_style_get_image_opa(&objStyle));
      if (!image.isEmpty()) style.insert("image", image);
    }

    if (editableParts & LVGL::Line) {
      QJsonObject line;
      if (!defaultStyle ||
          (axs_style_equal_line_color(defaultStyle, &objStyle)))
        line.insert(
            "color",
            QVariant(lvgl.toColor(axs_lv_style_get_line_color(&objStyle)))
                .toString());
      if (!defaultStyle ||
          (axs_style_equal_line_width(defaultStyle, &objStyle)))
        line.insert("width", axs_lv_style_get_line_width(&objStyle));
      if (!defaultStyle || (axs_style_equal_line_opa(defaultStyle, &objStyle)))
        line.insert("opa", axs_lv_style_get_line_opa(&objStyle));
      if (!line.isEmpty()) style.insert("line", line);
    }

    if (style.size() > 0) styles.append(style);
  }
  return styles;
}

QStringList LVGLObject::codeStyle(QString styleVar, int type) const {
  QStringList ret;
  if (!hasCustomStyle(type)) return ret;

  const lv_style_t &objStyle = m_styles[type];
  const lv_style_t *defaultStyle = m_widgetClass->style(m_obj);

  const LVGL::StyleParts editableParts = m_widgetClass->editableStyles(type);
  if (defaultStyle &&
      !LVGLStyle::hasStyleChanged(&objStyle, defaultStyle, editableParts))
    return ret;

  if (editableParts & LVGL::Background) {
    if (!defaultStyle || (axs_style_equal_bg_color(defaultStyle, &objStyle))) {
      QString color =
          QVariant(lvgl.toColor(axs_lv_style_get_bg_color(&objStyle)))
              .toString()
              .replace("#", "0x");
      ret << styleVar + ".body.main_color = lv_color_hex(" + color + ");";
    }
    if (!defaultStyle ||
        (axs_style_equal_bg_grad_color(defaultStyle, &objStyle))) {
      QString color =
          QVariant(lvgl.toColor(axs_lv_style_get_bg_grad_color(&objStyle)))
              .toString()
              .replace("#", "0x");
      ret << styleVar + ".body.grad_color = lv_color_hex(" + color + ");";
    }
    if (!defaultStyle || (axs_style_equal_radius(defaultStyle, &objStyle)))
      ret << styleVar + ".body.radius = " +
                 QString::number(axs_lv_style_get_radius(&objStyle)) + ";";
    if (!defaultStyle || (axs_style_equal_bg_opa(defaultStyle, &objStyle)))
      ret << styleVar + ".body.opa = " +
                 QString::number(axs_lv_style_get_bg_opa(&objStyle)) + ";";

    if (editableParts & LVGL::Border) {
      if (!defaultStyle ||
          (axs_style_equal_border_color(defaultStyle, &objStyle))) {
        QString color =
            QVariant(lvgl.toColor(axs_lv_style_get_border_color(&objStyle)))
                .toString()
                .replace("#", "0x");
        ret << styleVar + ".body.border.color = lv_color_hex(" + color + ");";
      }
      if (!defaultStyle ||
          (axs_style_equal_border_width(defaultStyle, &objStyle)))
        ret << styleVar + ".body.border.width = " +
                   QString::number(axs_lv_style_get_border_width(&objStyle)) +
                   ";";
      if (!defaultStyle ||
          (axs_style_equal_border_side(defaultStyle, &objStyle)))
        ret << styleVar + ".body.border.part = " +
                   QString::number(axs_lv_style_get_border_side(&objStyle)) +
                   ";";
      if (!defaultStyle ||
          (axs_style_equal_border_opa(defaultStyle, &objStyle)))
        ret << styleVar + ".body.border.opa = " +
                   QString::number(axs_lv_style_get_bg_opa(&objStyle)) + ";";
    }

    if (editableParts & LVGL::Shadow) {
      QJsonObject shadow;
      if (!defaultStyle ||
          (axs_style_equal_shadow_color(defaultStyle, &objStyle))) {
        QString color =
            QVariant(lvgl.toColor(axs_lv_style_get_shadow_color(&objStyle)))
                .toString()
                .replace("#", "0x");
        ret << styleVar + ".body.shadow.color = lv_color_hex(" + color + ");";
      }
      if (!defaultStyle ||
          (axs_style_equal_shadow_width(defaultStyle, &objStyle)))
        ret << styleVar + ".body.shadow.width = " +
                   QString::number(axs_lv_style_get_shadow_width(&objStyle)) +
                   ";";
    }

    if (editableParts & LVGL::Padding) {
      QJsonObject padding;
      if (!defaultStyle || (axs_style_equal_pad_top(defaultStyle, &objStyle)))
        ret << styleVar + ".body.padding.top = " +
                   QString::number(axs_lv_style_get_pad_top(&objStyle)) + ";";
      if (!defaultStyle ||
          (axs_style_equal_pad_bottom(defaultStyle, &objStyle)))
        ret << styleVar + ".body.padding.bottom = " +
                   QString::number(axs_lv_style_get_pad_bottom(&objStyle)) +
                   ";";
      if (!defaultStyle || (axs_style_equal_pad_left(defaultStyle, &objStyle)))
        ret << styleVar + ".body.padding.left = " +
                   QString::number(axs_lv_style_get_pad_left(&objStyle)) + ";";
      if (!defaultStyle || (axs_style_equal_pad_right(defaultStyle, &objStyle)))
        ret << styleVar + ".body.padding.right = " +
                   QString::number(axs_lv_style_get_pad_right(&objStyle)) + ";";
      if (!defaultStyle || (axs_style_equal_pad_inner(defaultStyle, &objStyle)))
        ret << styleVar + ".body.padding.inner = " +
                   QString::number(axs_lv_style_get_pad_inner(&objStyle)) + ";";
    }
  }

  if (editableParts & LVGL::Text) {
    if (!defaultStyle ||
        (axs_style_equal_text_color(defaultStyle, &objStyle))) {
      QString color =
          QVariant(lvgl.toColor(axs_lv_style_get_text_color(&objStyle)))
              .toString()
              .replace("#", "0x");
      ret << styleVar + ".text.color = lv_color_hex(" + color + ");";
    }
    if (!defaultStyle ||
        (axs_style_equal_text_sel_color(defaultStyle, &objStyle))) {
      QString color =
          QVariant(lvgl.toColor(axs_lv_style_get_text_sel_color(&objStyle)))
              .toString()
              .replace("#", "0x");
      ret << styleVar + ".text.sel_color = lv_color_hex(" + color + ");";
    }
    if (!defaultStyle || (axs_style_equal_text_font(defaultStyle, &objStyle))) {
      ret << styleVar + ".text.font = &" +
                 lvgl.fontCodeName(axs_lv_style_get_text_font(&objStyle)) + ";";
    }
    if (!defaultStyle ||
        (axs_style_equal_text_letter_space(defaultStyle, &objStyle)))
      ret << styleVar + ".text.letter_space = " +
                 QString::number(
                     axs_lv_style_get_text_letter_space(&objStyle)) +
                 ";";
    if (!defaultStyle ||
        (axs_style_equal_text_line_space(defaultStyle, &objStyle)))
      ret << styleVar + ".text.line_space = " +
                 QString::number(axs_lv_style_get_text_line_space(&objStyle)) +
                 ";";
    if (!defaultStyle || (axs_style_equal_text_opa(defaultStyle, &objStyle)))
      ret << styleVar + ".text.opa = " +
                 QString::number(axs_lv_style_get_text_opa(&objStyle)) + ";";
  }

  if (editableParts & LVGL::Image) {
    if (!defaultStyle ||
        (axs_style_equal_image_recolor(defaultStyle, &objStyle))) {
      QString color =
          QVariant(lvgl.toColor(axs_lv_style_get_image_recolor(&objStyle)))
              .toString()
              .replace("#", "0x");
      ret << styleVar + ".image.recolor = lv_color_hex(" + color + ");";
    }
    if (!defaultStyle || (axs_style_equal_image_opa(defaultStyle, &objStyle)))
      ret << styleVar + ".image.opa = " +
                 QString::number(axs_lv_style_get_image_opa(&objStyle)) + ";";
  }

  if (editableParts & LVGL::Line) {
    if (!defaultStyle ||
        (axs_style_equal_line_color(defaultStyle, &objStyle))) {
      QString color =
          QVariant(lvgl.toColor(axs_lv_style_get_line_color(&objStyle)))
              .toString()
              .replace("#", "0x");
      ret << styleVar + ".line.color = lv_color_hex(" + color + ");";
    }
    if (!defaultStyle || (axs_style_equal_line_width(defaultStyle, &objStyle)))
      ret << styleVar + ".line.width = " +
                 QString::number(axs_lv_style_get_line_width(&objStyle)) + ";";
    if (!defaultStyle || (axs_style_equal_line_opa(defaultStyle, &objStyle)))
      ret << styleVar + ".line.opa = " +
                 QString::number(axs_lv_style_get_line_opa(&objStyle)) + ";";
  }

  return ret;
}

void LVGLObject::parseStyles(const QJsonArray &styles) {
  for (int i = 0; i < styles.size(); ++i) {
    QJsonObject style = styles[i].toObject();
    int type = m_widgetClass->styles().indexOf(style["type"].toString());
    Q_ASSERT(type >= 0);

    lv_style_t *objStyle = this->style(type);
    if (style.contains("background")) {
      QJsonObject body = style["background"].toObject();
      if (body.contains("main_color")) {
        const QColor c =
            QVariant(body["main_color"].toString()).value<QColor>();
        axs_lv_style_set_bg_color(objStyle, lvgl.fromColor(c));
      }
      if (body.contains("grad_color")) {
        const QColor c =
            QVariant(body["grad_color"].toString()).value<QColor>();
        axs_lv_style_set_bg_grad_color(objStyle, lvgl.fromColor(c));
      }
      if (body.contains("radius"))
        axs_lv_style_set_radius(
            objStyle, static_cast<lv_coord_t>(body["radius"].toInt()));
      if (body.contains("opa"))
        axs_lv_style_set_bg_opa(objStyle,
                                static_cast<lv_opa_t>(body["opa"].toInt()));

      if (body.contains("border")) {
        QJsonObject border = body["border"].toObject();
        if (border.contains("color")) {
          const QColor c = QVariant(border["color"].toString()).value<QColor>();
          axs_lv_style_set_border_color(objStyle, lvgl.fromColor(c));
        }
        if (border.contains("width"))
          axs_lv_style_set_border_width(
              objStyle, static_cast<lv_coord_t>(border["width"].toInt()));

        if (border.contains("part"))
          axs_lv_style_set_border_side(
              objStyle, static_cast<lv_border_side_t>(border["part"].toInt()));
        if (border.contains("opa"))
          axs_lv_style_set_border_opa(
              objStyle, static_cast<lv_opa_t>(border["opa"].toInt()));
      }

      if (body.contains("shadow")) {
        QJsonObject shadow = body["shadow"].toObject();
        if (shadow.contains("color")) {
          const QColor c = QVariant(shadow["color"].toString()).value<QColor>();
          axs_lv_style_set_shadow_color(objStyle, lvgl.fromColor(c));
        }
        if (shadow.contains("width"))
          axs_lv_style_set_shadow_width(
              objStyle, static_cast<lv_coord_t>(shadow["width"].toInt()));
      }

      if (body.contains("padding")) {
        QJsonObject padding = body["padding"].toObject();
        if (padding.contains("top"))
          axs_lv_style_set_pad_top(
              objStyle, static_cast<lv_coord_t>(padding["top"].toInt()));
        if (padding.contains("bottom"))
          axs_lv_style_set_pad_bottom(
              objStyle, static_cast<lv_coord_t>(padding["bottom"].toInt()));
        if (padding.contains("left"))
          axs_lv_style_set_pad_left(
              objStyle, static_cast<lv_coord_t>(padding["left"].toInt()));
        if (padding.contains("right"))
          axs_lv_style_set_pad_right(
              objStyle, static_cast<lv_coord_t>(padding["right"].toInt()));
        if (padding.contains("inner"))
          axs_lv_style_set_pad_inner(
              objStyle, static_cast<lv_coord_t>(padding["inner"].toInt()));
      }
    }
    if (style.contains("text")) {
      QJsonObject text = style["text"].toObject();
      if (text.contains("color")) {
        const QColor c = QVariant(text["color"].toString()).value<QColor>();
        axs_lv_style_set_text_color(objStyle, lvgl.fromColor(c));
      }
      if (text.contains("sel_color")) {
        const QColor c = QVariant(text["sel_color"].toString()).value<QColor>();
        axs_lv_style_set_text_sel_color(objStyle, lvgl.fromColor(c));
      }
      if (text.contains("font")) {
        const QString fontName = text["font"].toString();
        const lv_font_t *f = lvgl.font(fontName);
        axs_lv_style_set_text_font(objStyle, f);
      }
      if (text.contains("line_space"))
        axs_lv_style_set_text_line_space(
            objStyle, static_cast<lv_coord_t>(text["line_space"].toInt()));
      if (text.contains("letter_space"))
        axs_lv_style_set_text_letter_space(
            objStyle, static_cast<lv_coord_t>(text["letter_space"].toInt()));
      if (text.contains("opa"))
        axs_lv_style_set_text_opa(objStyle,
                                  static_cast<lv_opa_t>(text["opa"].toInt()));
    }

    if (style.contains("image")) {
      QJsonObject image = style["image"].toObject();
      if (image.contains("recolor")) {
        const QColor c = QVariant(image["recolor"].toString()).value<QColor>();
        axs_lv_style_set_image_recolor(objStyle, lvgl.fromColor(c));
      }
      if (image.contains("opa"))
        axs_lv_style_set_image_opa(objStyle,
                                   static_cast<lv_opa_t>(image["opa"].toInt()));
    }

    if (style.contains("line")) {
      QJsonObject line = style["line"].toObject();
      if (line.contains("color")) {
        const QColor c = QVariant(line["color"].toString()).value<QColor>();
        axs_lv_style_set_line_color(objStyle, lvgl.fromColor(c));
      }
      if (line.contains("width"))
        axs_lv_style_set_line_width(
            objStyle, static_cast<lv_opa_t>(line["width"].toInt()));
      if (line.contains("opa"))
        axs_lv_style_set_line_opa(objStyle,
                                  static_cast<lv_opa_t>(line["opa"].toInt()));
    }
    m_widgetClass->setStyle(m_obj, type, objStyle);
    // m_widgetClass->addStyle(m_obj,type,objStyle);
  }
}

LVGLObject *LVGLObject::parent() const { return m_parent; }

LVGLObject::operator const lv_obj_t *() const noexcept { return m_obj; }

LVGLObject::operator lv_obj_t *() noexcept { return m_obj; }

bool LVGLObject::hasCustomStyle(int type) const {
  return m_styles.contains(type);
}

lv_style_t *LVGLObject::style(int type) {
  auto s = m_widgetClass->style(m_obj);
  if (s == nullptr) {
    auto p = new (lv_style_t);
    m_liststyles << p;
    lv_style_init(p);
    m_widgetClass->addStyle(m_obj, p);
    s = p;
  }
  return s;
}

lv_style_t *LVGLObject::style(lv_obj_part_t part) {
  auto s = m_widgetClass->style(m_obj, part);
  if (s == nullptr) {
    auto p = new (lv_style_t);
    m_liststyles << p;
    lv_style_init(p);
    m_widgetClass->addStyle(m_obj, p, part);
    s = p;
  }
  return s;
}

lv_style_t *LVGLObject::style(int partsindex, int stateindex) {
  return m_widgetClass->getstyle(partsindex, stateindex);
}

QString LVGLObject::styleCodeName(int type) const {
  return "style" + QString::number(type) + "_" +
         name().toLower().replace(' ', '_');
}

QList<LVGLObject *> LVGLObject::childs() const { return m_childs; }

void LVGLObject::removeChild(LVGLObject *child) { m_childs.removeOne(child); }

bool LVGLObject::isAccessible() const { return m_accessible; }

void LVGLObject::setAccessible(bool accessible) { m_accessible = accessible; }

bool LVGLObject::doesNameExists() const {
  for (LVGLObject *o : lvgl.allObjects()) {
    if (o == this) continue;
    if (o->name() == m_name) return true;
  }
  return false;
}

bool LVGLObject::doesNameExists(const QString &name, LVGLObject *except) {
  for (LVGLObject *o : lvgl.allObjects()) {
    if (except && (except == o)) continue;
    if (o->name() == name) return true;
  }
  return false;
}

void LVGLObject::generateName() {
  // create sorted list by type
  QList<int> numbers;
  const QString prefix = m_widgetClass->name().toLower().replace(' ', '_');

  for (LVGLObject *o : lvgl.allObjects()) {
    if (o->widgetType() == m_widgetClass->type() &&
        o->name().startsWith(prefix)) {
      const int index = o->name().lastIndexOf('_');
      const int num = o->name().mid(index + 1).toInt();
      if (num > 0) {
        auto ind = std::lower_bound(numbers.begin(), numbers.end(), num);
        numbers.insert(ind, num);
      }
    }
  }

  if (m_name.startsWith(prefix)) {
    const int index = m_name.lastIndexOf('_');
    const int num = m_name.mid(index + 1).toInt();
    if (!numbers.contains(num)) return;  // no new name needed
  }

  // find next free id
  int id = 1;
  for (int num : numbers) {
    if (num == id)
      id++;
    else
      break;
  }
  m_name = QString("%1_%2").arg(prefix).arg(id);
  return;
}

LVGLObject *LVGLObject::parse(QJsonObject object, LVGLObject *parent) {
  QString className = object["class"].toString();
  const LVGLWidget *widgetClass = lvgl.widget(className);
  if (widgetClass) {
    LVGLObject *newObj;
    QString name = object["name"].toString();
    if (parent)
      newObj = new LVGLObject(widgetClass, name, parent);
    else
      newObj = new LVGLObject(widgetClass, name, lv_scr_act());
    // generate name if nessessary
    if (newObj->name().isEmpty() || doesNameExists(name, newObj))
      newObj->generateName();
    lvgl.addObject(newObj);

    for (LVGLProperty *p : widgetClass->properties()) {
      QString pname = p->name().toLower();
      if (pname == "name") continue;
      if (object.contains(pname)) {
        QVariant v = object[pname].toVariant();
        if (!v.isNull()) p->setValue(newObj, v);
      }
    }
    if (object.contains("styles"))
      newObj->parseStyles(object["styles"].toArray());

    if (object.contains("children")) {
      QJsonArray childs = object["children"].toArray();
      for (int i = 0; i < childs.size(); ++i) {
        QJsonObject child = childs[i].toObject();
        if (widgetClass->type() == LVGLWidget::TabView &&
            child.contains("index")) {
          LVGLObject *page = newObj->findChildByIndex(child["index"].toInt());
          page->setName(child["name"].toString());
          // parse page props
          for (LVGLProperty *p : lvgl.widget("lv_page")->specialProperties()) {
            const QString pname = p->name().toLower();
            if (child.contains(pname)) {
              QVariant v = child[pname].toVariant();
              if (!v.isNull()) p->setValue(page, v);
            }
          }
          // parse styles
          if (child.contains("styles"))
            page->parseStyles(child["styles"].toArray());
          // parse children of page
          if (child.contains("children")) {
            QJsonArray page_childs = child["children"].toArray();
            for (int j = 0; j < page_childs.size(); ++j)
              parse(page_childs[j].toObject(), page);
          }
        } else {
          parse(child, newObj);
        }
      }
    }
    return newObj;
  }
  return nullptr;
}

int LVGLObject::index() const { return m_index; }

LVGLObject *LVGLObject::findChildByIndex(int index) const {
  for (LVGLObject *child : m_childs) {
    if (child->index() == index) return child;
  }
  return nullptr;
}

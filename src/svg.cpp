#include "svg.h"

#include <utility>

namespace Svg {
  void RenderColor(std::ostream& out, std::monostate) {
    out << "none";
  }

  void RenderColor(std::ostream& out, const std::string& value) {
    out << value;
  }

  void RenderColor(std::ostream& out, Rgba rgba) {
    out << "rgb";
    if (rgba.alpha) {
      out << "a";
    }
    out << '(' << static_cast<int>(rgba.red)
        << "," << static_cast<int>(rgba.green)
        << "," << static_cast<int>(rgba.blue);
    if (rgba.alpha) {
      out << ',' << *rgba.alpha;
    }
    out << ')';
  }

  void RenderColor(std::ostream& out, const Color& color) {
    std::visit([&out](const auto& value) { RenderColor(out, value); },
               color);
  }

  template <typename Owner>
  Owner& PathProps<Owner>::AsOwner() {
    return static_cast<Owner&>(*this);
  }

  template <typename Owner>
  Owner& PathProps<Owner>::SetFillColor(const Color& color) {
    fill_color_ = color;
    return AsOwner();
  }

  template <typename Owner>
  Owner& PathProps<Owner>::SetStrokeColor(const Color& color) {
    stroke_color_ = color;
    return AsOwner();
  }

  template <typename Owner>
  Owner& PathProps<Owner>::SetStrokeWidth(double value) {
    stroke_width_ = value;
    return AsOwner();
  }

  template <typename Owner>
  Owner& PathProps<Owner>::SetStrokeLineCap(const std::string& value) {
    stroke_line_cap_ = value;
    return AsOwner();
  }

  template <typename Owner>
  Owner& PathProps<Owner>::SetStrokeLineJoin(const std::string& value) {
    stroke_line_join_ = value;
    return AsOwner();
  }

  template <typename Owner>
    void PathProps<Owner>::RenderAttrs(std::ostream& out) const {
    out << "fill=\"";
    RenderColor(out, fill_color_);
    out << "\" ";
    out << "stroke=\"";
    RenderColor(out, stroke_color_);
    out << "\" ";
    out << "stroke-width=\"" << stroke_width_ << "\" ";
    if (stroke_line_cap_) {
      out << "stroke-linecap=\"" << *stroke_line_cap_ << "\" ";
    }
    if (stroke_line_join_) {
      out << "stroke-linejoin=\"" << *stroke_line_join_ << "\" ";
    }
  }

  Circle& Circle::SetCenter(Point point) {
    center_ = point;
    return *this;
  }
  Circle& Circle::SetRadius(double radius) {
    radius_ = radius;
    return *this;
  }

  void Circle::Render(std::ostream& out) const {
    out << "<circle ";
    out << "cx=\"" << center_.x << "\" ";
    out << "cy=\"" << center_.y << "\" ";
    out << "r=\"" << radius_ << "\" ";
    PathProps::RenderAttrs(out);
    out << "/>";
  }

  Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
  }

  void Polyline::Render(std::ostream& out) const {
    out << "<polyline ";
    out << "points=\"";
    bool first = true;
    for (const Point point : points_) {
      if (first) {
        first = false;
      } else {
        out << " ";
      }
      out << point.x << "," << point.y;
    }
    out << "\" ";
    PathProps::RenderAttrs(out);
    out << "/>";
  }

  Text& Text::SetPoint(Point point) {
    point_ = point;
    return *this;
  }

  Text& Text::SetOffset(Point point) {
    offset_ = point;
    return *this;
  }

  Text& Text::SetFontSize(std::uint32_t size) {
    font_size_ = size;
    return *this;
  }

  Text& Text::SetFontFamily(const std::string& value) {
    font_family_ = value;
    return *this;
  }

  Text& Text::SetData(const std::string& data) {
    data_ = data;
    return *this;
  }

  void Text::Render(std::ostream& out) const {
    out << "<text ";
    out << "x=\"" << point_.x << "\" ";
    out << "y=\"" << point_.y << "\" ";
    out << "dx=\"" << offset_.x << "\" ";
    out << "dy=\"" << offset_.y << "\" ";
    out << "font-size=\"" << font_size_ << "\" ";
    if (font_family_) {
      out << "font-family=\"" << *font_family_ << "\" ";
    }
    PathProps::RenderAttrs(out);
    out << ">";
    out << data_;
    out << "</text>";
  }

  template <typename ObjectType>
  void Document::Add(ObjectType object) {
    objects_.push_back(std::make_unique<ObjectType>(std::move(object)));
  }

  void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">";
    for (const auto& object_ptr : objects_) {
      object_ptr->Render(out);
    }
    out << "</svg>";
  }

  template class PathProps<Circle>;
  template class PathProps<Polyline>;
  template class PathProps<Text>;
}

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace Svg {
  struct Point {
    double x;
    double y;
  };

  std::ostream& operator<<(std::ostream& out, const Point& point) {
    return out << point.x << ',' << point.y;
  }

  struct Rgb {
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
  };

  std::ostream& operator<<(std::ostream& out, const Rgb& rgb) {
    return out << "rgb(" << static_cast<int>(rgb.red)
               << ',' << static_cast<int>(rgb.green)
               << ',' << static_cast<int>(rgb.blue) << ')';
  }

  enum class Colors {
    EMPTY
  };

  std::ostream& operator<<(std::ostream& out, Colors color) {
    switch (color) {
    case Colors::EMPTY:
      return out << "none";
    }
    return out;
  }

  class Color {
  public:
    Color() : color(Colors::EMPTY) {}
    Color(const std::string& color_) : color(color_) {}
    Color(const char* color_) : color(color_) {}
    Color(std::string&& color_) : color(std::move(color_)) {}

    Color& operator=(const std::string& color_) {
      color = color_;
      return *this;
    }
    Color& operator=(std::string&& color_) {
      color = std::move(color_);
      return *this;
    }

    Color(const Rgb& rgb) : color(rgb) {}
    Color& operator=(const Rgb& rgb) {
      color = rgb;
      return *this;
    }
    Color& operator=(Rgb&& rgb) {
      color = std::move(rgb);
      return *this;
    }
    friend std::ostream& operator<<(std::ostream& out, const Color& color);
  private:
    std::variant<Colors, std::string, Rgb> color;
  };

  std::ostream& operator<<(std::ostream& out, const Color& color) {
    visit([&out](const auto& color) {
            out << color;
          },
      color.color);
    return out;
  }

  Color NoneColor = "none";

  template <typename T>
  void PrintAttribute(std::ostream& out, const std::string& name, const T& value) {
    out << ' ' << name << "=\"" << value << '"';
  }

  template <typename T>
  void PrintAttribute(std::ostream& out, const std::string& name, const std::optional<T>& value) {
    if (value) {
      PrintAttribute<T>(out, name, *value);
    }
  }

  class Printable {
  public:
    friend std::ostream& operator<<(std::ostream& out, const Printable& object);
    virtual ~Printable() {}
  private:
    virtual void Print(std::ostream& out) const = 0;
  };

  std::ostream& operator<<(std::ostream& out, const Printable& object) {
    object.Print(out);
    return out;
  }

#define CASTING_CHAINING_SETTER(object_type, setter, property_type, property) \
  object_type& Set##setter(const property_type& property) {             \
    this->property = property;                                          \
    return static_cast<object_type&>(*this);                            \
  }                                                                     \
  object_type& Set##setter(property_type&& property) {                  \
    this->property = std::move(property);                               \
    return static_cast<object_type&>(*this);                            \
  }

  template <typename T>
  class Object : public Printable {
  public:
    CASTING_CHAINING_SETTER(T, FillColor, Color, fill)
    CASTING_CHAINING_SETTER(T, StrokeColor, Color, stroke)
    CASTING_CHAINING_SETTER(T, StrokeWidth, double, stroke_width)
    CASTING_CHAINING_SETTER(T, StrokeLineCap, std::string, stroke_linecap)
    CASTING_CHAINING_SETTER(T, StrokeLineJoin, std::string, stroke_linejoin)
    ~Object() {}
  protected:
    void PrintAttributes(std::ostream& out) const;
  private:
    Color fill = NoneColor;
    Color stroke = NoneColor;
    double stroke_width = 1;
    std::optional<std::string> stroke_linecap = std::nullopt;
    std::optional<std::string> stroke_linejoin = std::nullopt;
  };

  template <typename T>
  void Object<T>::PrintAttributes(std::ostream& out) const {
    PrintAttribute(out, "fill", fill);
    PrintAttribute(out, "stroke", stroke);
    PrintAttribute(out, "stroke-width", stroke_width);
    PrintAttribute(out, "stroke-linecap", stroke_linecap);
    PrintAttribute(out, "stroke-linejoin", stroke_linejoin);
  }

#define CHAINING_SETTER(object_type, setter, property_type, property)   \
  object_type& Set##setter(const property_type& property) {             \
    this->property = property;                                          \
    return *this;                                                       \
  }                                                                     \
  object_type& Set##setter(property_type&& property) {                  \
    this->property = std::move(property);                               \
    return *this;                                                       \
  }

  class Circle : public Object<Circle> {
  public:
    CHAINING_SETTER(Circle, Center, Point, center)
    CHAINING_SETTER(Circle, Radius, double, radius)
  private:
    void Print(std::ostream& out) const override;
    Point center{0, 0};
    double radius = 1;
  };

  void Circle::Print(std::ostream& out) const {
    out << "<circle";
    Object::PrintAttributes(out);
    PrintAttribute(out, "cx", center.x);
    PrintAttribute(out, "cy", center.y);
    PrintAttribute(out, "r", radius);
    out << "/>";
  }

  class Polyline : public Object<Polyline> {
  public:
    Polyline& AddPoint(const Point& point) {
      points.push_back(point);
      return *this;
    }
    Polyline& AddPoint(Point&& point) {
      points.push_back(std::move(point));
      return *this;
    }
  private:
    void Print(std::ostream& out) const override;
    std::vector<Point> points;
  };

  void Polyline::Print(std::ostream& out) const {
    out << "<polyline";
    Object::PrintAttributes(out);
    std::ostringstream points_out;
    for (const auto& point : points) {
      points_out << point << ' ';
    }
    PrintAttribute(out, "points", points_out.str());
    out << "/>";
  }

  class Text : public Object<Text> {
  public:
    CHAINING_SETTER(Text, Point, Point, point)
    CHAINING_SETTER(Text, Offset, Point, offset)
    CHAINING_SETTER(Text, FontSize, uint32_t, font_size)
    CHAINING_SETTER(Text, FontFamily, std::string, font_family)
    CHAINING_SETTER(Text, Data, std::string, data)
  private:
    void Print(std::ostream& out) const override;
    Point point{0, 0};
    Point offset{0, 0};
    uint32_t font_size = 1;
    std::optional<std::string> font_family;
    std::string data;
  };

  void Text::Print(std::ostream& out) const {
    out << "<text";
    Object::PrintAttributes(out);
    PrintAttribute(out, "x", point.x);
    PrintAttribute(out, "y", point.y);
    PrintAttribute(out, "dx", offset.x);
    PrintAttribute(out, "dy", offset.y);
    PrintAttribute(out, "font-size", font_size);
    PrintAttribute(out, "font-family", font_family);
    out << '>' << data << "</text>";
  }

  class Document {
  public:
    Document() {}

    template <typename T>
    void Add(const T& object) {
      objects.push_back(std::make_unique<T>(object));
    }

    void Render(std::ostream& out) {
      out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << '\n'
          << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << '\n';

      for (const auto& object : objects) {
        out << *object << '\n';
      }

      out << "</svg>";
    };
  private:
    std::vector<std::unique_ptr<Printable>> objects;
  };
}

/*int main() {
  Svg::Document svg;

  svg.Add(
          Svg::Polyline{}
          .SetStrokeColor(Svg::Rgb{140, 198, 63})  // soft green
          .SetStrokeWidth(16)
          .SetStrokeLineCap("round")
          .AddPoint({50, 50})
          .AddPoint({250, 250})
          );

  for (const auto point : {Svg::Point{50, 50}, Svg::Point{250, 250}}) {
    svg.Add(
            Svg::Circle{}
            .SetFillColor("white")
            .SetRadius(6)
            .SetCenter(point)
            );
  }

  svg.Add(
          Svg::Text{}
          .SetPoint({50, 50})
          .SetOffset({10, -10})
          .SetFontSize(20)
          .SetFontFamily("Verdana")
          .SetFillColor("black")
          .SetData("C")
          );
  svg.Add(
          Svg::Text{}
          .SetPoint({250, 250})
          .SetOffset({10, -10})
          .SetFontSize(20)
          .SetFontFamily("Verdana")
          .SetFillColor("black")
          .SetData("C++")
          );

  svg.Render(std::cout);

  return 0;
}
*/

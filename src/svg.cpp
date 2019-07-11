#include <cstdint>
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

  struct Rgb {
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
  };

  class Color {
  public:
    Color() : color(std::nullopt) {}
    Color(const std::string& color_) : color(color_) {}
    Color(const Rgb& rgb) : color(rgb) {}
    friend std::ostream& operator<<(std::ostream& out, const Color& color);
  private:
    std::variant<std::optional<std::string>, Rgb> color;
  };

  Color NoneColor = Color();

  class Object {
  public:
    friend std::ostream& operator<<(std::ostream& out, const Object& object);
    virtual ~Object() {}
  private:
    virtual std::string ToString() const = 0;
  };

#define CASTING_CHAINING_SETTER(object_type, setter, property_type, property) \
  object_type& Set##setter(const property_type& property) {             \
    this->property = property;                                          \
    return static_cast<object_type&>(*this);                            \
  }

  template <typename T>
  class ChainingSettersObject : public Object {
  public:
    CASTING_CHAINING_SETTER(T, FillColor, Color, fill)
    CASTING_CHAINING_SETTER(T, StrokeColor, Color, stroke)
    CASTING_CHAINING_SETTER(T, StrokeWidth, double, stroke_width)
    CASTING_CHAINING_SETTER(T, StrokeLineCap, std::string, stroke_linecap)
    CASTING_CHAINING_SETTER(T, StrokeLineJoin, std::string, stroke_linejoin)
  private:
    virtual std::string ToString() const override = 0;
    Color fill = NoneColor;
    Color stroke = NoneColor;
    double stroke_width = 1;
    std::optional<std::string> stoke_linecap = std::nullopt;
    std::optional<std::string> stoke_linejoin = std::nullopt;
  };

#define CHAINING_SETTER(object_type, setter, property_type, property)   \
  object_type& Set##setter(const property_type& property) {             \
    this->property = property;                                          \
    return *this;                                                       \
  }

  class Circle : public ChainingSettersObject<Circle> {
  public:
    CHAINING_SETTER(Circle, Center, Point, center)
    CHAINING_SETTER(Circle, Radius, double, radius)
    private:
    Point center{0, 0};
    double radius = 1;
  };

  class Polyline : public ChainingSettersObject<Polyline> {
  public:
    Polyline& AddPoint(const Point& point) {
      points.push_back(point);
      return *this;
    }
  private:
    std::vector<Point> points;
  };

  class Text : public ChainingSettersObject<Text> {
  public:
    CHAINING_SETTER(Text, Point, Point, point)
    CHAINING_SETTER(Text, Offset, Point, offset)
    CHAINING_SETTER(Text, FontSize, uint32_t, font_size)
    CHAINING_SETTER(Text, FontFamily, std::string, font_family)
    CHAINING_SETTER(Text, Data, std::string, data)
    private:
    Point point{0, 0};
    Point offset{0, 0};
    uint32_t font_size = 1;
    std::optional<std::string> font_family;
    std::string data;
  };

  class Document {
  public:
    Document() {}

    template <typename T>
    void Add(const T& object) { objects.push_back(object); }

    template <typename T>
    void Add(T&& object) { objects.emplace_back(object); }

    void Render(std::ostream& out) {
      out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << '\n'
          << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << '\n';

      for (const auto& object : objects) {
        out << *object << '\n';
      }

      out << "</svg>";
    };
  private:
    std::vector<std::unique_ptr<Object>> objects;
  };
}

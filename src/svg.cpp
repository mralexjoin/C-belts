#include <cstdint>
#include <memory>
#include <optional>
#include <ostream>
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

  const std::optional<std::string> NoneColor = std::nullopt;

  class Color {
  public:
    Color() : color(NoneColor) {}
    Color(const std::string& color_) : color(color_) {}
    Color(const Rgb& rgb) : color(rgb) {}
  private:
    std::variant<std::optional<std::string>, Rgb> color;
  };

  class Object {
  public:
    Object& SetFillColor(const Color& color_) {
      color = color_;
      return *this;
    }

    Object& SetStrokeColor(const Color& color_) {
      stroke_color = color_;
      return *this;
    }

    Object& SetStrokeWidth(double width_) {
      stroke_width = width_;
      return *this;
    }

    Object& SetStrokeLineCap(const string& linecap_) {
      stroke_linecap = linecap_;
      return *this;
    }

    Object& SetStrokeLineJoin(const string& linejoin_) {
      stroke_linejoin = linejoin_;
      return *this;
    }

  private:
    Color color = NoneColor;
    Color stroke_color = NoneColor;
    double stroke_width = 1;
    std::optional<std::string> stoke_linecap = std::nullopt;
    std::optional<std::string> stoke_linejoin = std::nullopt;
  };

  class Circle : public Object {

  };

  using ObjectHolder = std::unique_ptr<Object>;

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
        out << object.get() << '\n';
      }

      out << "</svg>";
    };
  private:
    std::vector<ObjectHolder> objects;
  };
}

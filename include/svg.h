#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace Svg {
  struct Point {
    double x = 0;
    double y = 0;
  };

  struct Rgba {
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
    std::optional<double> alpha = std::nullopt;
  };

  using Color = std::variant<std::monostate, std::string, Rgba>;
  const Color NoneColor{};

  class Object {
  public:
    virtual void Render(std::ostream& out) const = 0;
    virtual ~Object() = default;
  };

  template <typename Owner>
  class PathProps {
  public:
    Owner& SetFillColor(const Color& color);
    Owner& SetStrokeColor(const Color& color);
    Owner& SetStrokeWidth(double value);
    Owner& SetStrokeLineCap(const std::string& value);
    Owner& SetStrokeLineJoin(const std::string& value);
    void RenderAttrs(std::ostream& out) const;

  private:
    Color fill_color_;
    Color stroke_color_;
    double stroke_width_ = 1.0;
    std::optional<std::string> stroke_line_cap_;
    std::optional<std::string> stroke_line_join_;

    Owner& AsOwner();
  };

  class Circle : public Object, public PathProps<Circle> {
  public:
    Circle& SetCenter(Point point);
    Circle& SetRadius(double radius);
    void Render(std::ostream& out) const override;

  private:
    Point center_;
    double radius_ = 1;
  };

  class Polyline : public Object, public PathProps<Polyline> {
  public:
    Polyline& AddPoint(Point point);
    void Render(std::ostream& out) const override;

  private:
    std::vector<Point> points_;
  };

  class Text : public Object, public PathProps<Text> {
  public:
    Text& SetPoint(Point point);
    Text& SetOffset(Point point);
    Text& SetFontSize(std::uint32_t size);
    Text& SetFontFamily(const std::string& value);
    Text& SetData(const std::string& data);
    void Render(std::ostream& out) const override;

  private:
    Point point_;
    Point offset_;
    std::uint32_t font_size_ = 1;
    std::optional<std::string> font_family_;
    std::string data_;
  };

  class Document : public Object {
  public:
    template <typename ObjectType>
      void Add(ObjectType object);

    void Render(std::ostream& out) const override;

  private:
    std::vector<std::unique_ptr<Object>> objects_;
  };
}

#pragma once

#include "json.h"
#include "sphere.h"
#include "svg.h"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace Rendering {
  Svg::Point ParsePoint(const std::vector<Json::Node>& value);
  Svg::Color ParseColor(const Json::Node& color_json);

  std::vector<Svg::Color> ParseColorPallete(const std::vector<Json::Node>& colors);
  class TransportCatalog;
  class Renderer {
  public:
    Renderer(const Json::Dict& render_settings_json) :
      width_(render_settings_json.at("width").AsDouble()),
      height_(render_settings_json.at("height").AsDouble()),
      padding_(render_settings_json.at("padding").AsDouble()),
      stop_radius_(render_settings_json.at("stop_radius").AsDouble()),
      line_width_(render_settings_json.at("line_width").AsDouble()),
      stop_label_font_size_(static_cast<std::uint32_t>(render_settings_json.at("stop_label_font_size").AsInt())),
      stop_label_offset_(ParsePoint(render_settings_json.at("stop_label_offset").AsArray())),
      underlayer_color_(ParseColor(render_settings_json.at("underlayer_color"))),
      underlayer_width_(render_settings_json.at("underlayer_width").AsDouble()),
      color_pallete_(ParseColorPallete(render_settings_json.at("color_pallete").AsArray()))
    {}
    std::string Render(const TransportCatalog& catalog) const;
    void SetRectangle(Sphere::Point min, Sphere::Point max);
  private:
    Svg::Point ConvertPoint(const Sphere::Point& position) const;
    const double width_;
    const double height_;
    const double padding_;
    const double stop_radius_;
    const double line_width_;
    const uint32_t stop_label_font_size_;
    const Svg::Point stop_label_offset_;
    const Svg::Color underlayer_color_;
    const double underlayer_width_;
    const std::vector<Svg::Color> color_pallete_;
  };
}

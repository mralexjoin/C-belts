#include "rendering.h"
#include "transport_catalog.h"

#include <algorithm>

namespace Rendering {
  using namespace std;
  using namespace Json;

  Svg::Point ParsePoint(const vector<Node>& value) {
    return { value[0].AsDouble(), value[1].AsDouble() };
  }

  Svg::Color ParseColor(const string& color) {
    return color;
  }

  Svg::Color ParseColor(const vector<Node>& rgba) {
    auto color = Svg::Rgba {
      static_cast<std::uint8_t>(rgba[0].AsInt()),
      static_cast<std::uint8_t>(rgba[1].AsInt()),
      static_cast<std::uint8_t>(rgba[2].AsInt())
    };
    if (rgba.size() > 3) {
      color.alpha = rgba[3].AsDouble();
    }
    return color;
  }

  Svg::Color ParseColor(const Node& color_json) {
    return visit([](const auto& color_json) { return ParseColor(color_json); },
                 color_json.GetBase());
  }

  vector<Svg::Color> ParseColorPallete(const vector<Node>& colors) {
    vector<Svg::Color> pallete;
    pallete.reserve(colors.size());
    for (const auto& color : colors) {
      pallete.push_back(ParseColor(color));
    }
    return pallete;
  }

  Svg::Point Renderer::ConvertPoint(const Sphere::Point& position) const {
    return {(position.longitude - min_point_.longitude) * zoom_coeff_ + padding_,
          (max_point_.latitude - position.latitude) * zoom_coeff_ + padding_};
  }

  void Renderer::SetRectangle(Sphere::Point min_point, Sphere::Point max_point) {
    min_point_ = move(min_point);
    max_point_ = move(max_point);
    zoom_coeff_ = min(
        max_point_.longitude != min_point_.longitude ? (width_ - 2 * padding_) / (max_point_.longitude - min_point_.longitude) : 0,
        max_point_.latitude != min_point_.latitude ? (height_ - 2 * padding_) / (max_point_.latitude - min_point_.latitude) : 0
    );
  }

  template <double Sphere::Point::* property>
  pair<double, double> min_max_property(const unordered_map<string, Sphere::Point>& stops_dict) {
    if (stops_dict.empty()) {
      return {0, 0};
    }
    const auto& [min_property, max_property] = min_max_element(
        begin(stops_dict),
        end(stops_dict),
        [&property](const auto& lhs, const auto& rhs) {
          return lhs.second.position.*property < rhs.second.position.*property;
        }
    );
    return {
      min_property->second.position->*property,
      max_property->second.position->*property
    };
  }

  string Renderer::Render(const TransportCatalog& catalog) const {
    const auto& stops = catalog.stops_;
    const auto [min_lat, max_lat] = min_max_property<&Sphere::Point::latitude>(stops);
    const auto [min_lon, max_lon] = min_max_property<&Sphere::Point::longitude>(stops);
    const double zoom_coeff_ = min(
        min_lat != max_lat ? (height_ - 2 * padding_) / (max_lat - min_lat) : 0,
        max_lon != min_lon ? (width_ - 2 * padding_) / (max_lon - min_lon) : 0
    );
  }
}

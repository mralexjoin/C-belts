#include "rendering.h"

#include "svg.h"

#include <algorithm>
#include <iomanip>
#include <iterator>
#include <sstream>

namespace Rendering {
  using namespace std;
  using namespace Descriptions;

  using StopsPoints = std::map<std::string, Svg::Point>;
  using ColorPalette = std::vector<Svg::Color>;

  Svg::Point ParsePoint(const vector<Json::Node>& value) {
    return { value[0].AsDouble(), value[1].AsDouble() };
  }

  Svg::Color ParseColor(const string& color) {
    return color;
  }

  Svg::Color ParseColor(const vector<Json::Node>& rgba) {
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

  Svg::Color ParseColor(const Json::Node& color_json) {
    return visit([](const auto& color_json) { return ParseColor(color_json); },
                 color_json.GetBase());
  }

  vector<Svg::Color> ParseColorPalette(const vector<Json::Node>& colors) {
    vector<Svg::Color> palette;
    palette.reserve(colors.size());
    for (const auto& color : colors) {
      palette.push_back(ParseColor(color));
    }
    return palette;
  }

  template <typename PropertyType, PropertyType Sphere::Point::*property>
  pair<PropertyType, PropertyType> minmax_property(const StopsDict& stops_dict) {
    if (stops_dict.empty()) {
      return {0, 0};
    }

    using ItemType = StopsDict::const_iterator::value_type;

    const auto& [min_property, max_property] = minmax_element(
        begin(stops_dict),
        end(stops_dict),
        [](const ItemType& lhs, const ItemType& rhs) {
          return lhs.second->position.*property < rhs.second->position.*property;
        }
    );
    return {
      min_property->second->position.*property,
      max_property->second->position.*property
    };
  }

  StopsPoints TransformStopPoints(
    const StopsDict& stops_dict,
    double padding, double width, double height) {

    const auto [min_lat, max_lat] = minmax_property<double, &Sphere::Point::latitude>(stops_dict);
    const auto [min_lon, max_lon] = minmax_property<double, &Sphere::Point::longitude>(stops_dict);

    vector<double> zoom_coefs;
    const auto zoom_coef_adder = [&zoom_coefs, &padding](const double length, double max_pos, double min_pos) {
      if (max_pos != min_pos) {
        zoom_coefs.push_back((length - 2 * padding) / (max_pos - min_pos));
      }
    };

    zoom_coef_adder(width, max_lon, min_lon);
    zoom_coef_adder(height, max_lat, min_lat);

    const auto zoom_coef = !zoom_coefs.empty() ? *min_element(begin(zoom_coefs), end(zoom_coefs)) : 0;

    StopsPoints stops_points;
    transform(
      begin(stops_dict),
      end(stops_dict),
      inserter(stops_points, end(stops_points)),
      [min_lon, max_lat, zoom_coef, padding](const auto& stop) {
        const auto& position = stop.second->position;
        return pair(stop.first,
          Svg::Point({
              (position.longitude - min_lon) * zoom_coef + padding,
              (max_lat - position.latitude) * zoom_coef + padding
            })
        );
      }
    );
    return stops_points;
  }

  string RenderBusLines(
    const BusesDict& buses_dict,
    const StopsPoints& stop_points,
    const ColorPalette& color_palette,
    double stroke_width) {

    const auto buses_size = buses_dict.size();

    vector<const string*> buses_names;
    buses_names.reserve(buses_size);
    transform(
      begin(buses_dict),
      end(buses_dict),
      back_inserter(buses_names),
      [](const auto& item) { return &item.first; });
    sort(
      begin(buses_names),
      end(buses_names),
      [](const string* lhs, const string* rhs) { return *lhs < *rhs; });

    ostringstream out;
    out.precision(16);

    for (size_t bus_num = 0; bus_num < buses_size; ++bus_num) {
      Svg::Polyline bus_line;
      for (const auto& stop : buses_dict.at(*buses_names[bus_num])->stops) {
        bus_line.AddPoint(stop_points.at(stop));
      }
      bus_line
        .SetStrokeColor(color_palette[bus_num % color_palette.size()])
        .SetStrokeWidth(stroke_width)
        .SetStrokeLineCap("round")
        .Render(out);
    }
    return out.str();
  }

  string RenderStopRounds(const StopsPoints& stop_points, double stop_radius) {
    ostringstream out;
    out.precision(16);
    for (const auto& [name, point] : stop_points) {
       Svg::Circle()
         .SetCenter(point)
         .SetRadius(stop_radius)
         .SetFillColor("white")
         .Render(out);
    }

    return out.str();
  }

  string RenderStopNames(
    const StopsPoints& stop_points,
    const Svg::Point& stop_label_offset,
    int stop_label_font_size,
    const Svg::Color& underlayer_color,
    double underlayer_width) {

    ostringstream out;
    out.precision(16);
    for (const auto& [name, point] : stop_points) {
      auto underlayer = Svg::Text()
        .SetPoint(point)
        .SetOffset(stop_label_offset)
        .SetFontSize(stop_label_font_size)
        .SetFontFamily("Verdana")
        .SetData(name);
      auto name_text = underlayer;
      underlayer
        .SetFillColor(underlayer_color)
        .SetStrokeColor(underlayer_color)
        .SetStrokeWidth(underlayer_width)
        .SetStrokeLineCap("round")
        .SetStrokeLineJoin("round")
        .Render(out);
      name_text
        .SetFillColor("black")
        .Render(out);
    }

    return out.str();
  }

  RenderedObjects RenderObjects(
    const StopsDict& stops_dict,
    const BusesDict& buses_dict,
    const Json::Dict& renderer_settings_json) {

    const auto stops_points = TransformStopPoints(
      stops_dict,
      renderer_settings_json.at("padding").AsDouble(),
      renderer_settings_json.at("width").AsDouble(),
      renderer_settings_json.at("height").AsDouble());

    RenderedObjects objects;

    objects[ObjectIndex(RenderedObjectsTypes::ROUTES)] = RenderBusLines(
      buses_dict,
      stops_points,
      ParseColorPalette(renderer_settings_json.at("color_palette").AsArray()),
      renderer_settings_json.at("line_width").AsDouble()
    );

    objects[ObjectIndex(RenderedObjectsTypes::STOPS)] = RenderStopRounds(
      stops_points,
      renderer_settings_json.at("stop_radius").AsDouble()
    );

    objects[ObjectIndex(RenderedObjectsTypes::STOPS_NAMES)] = RenderStopNames(
      stops_points,
      ParsePoint(renderer_settings_json.at("stop_label_offset").AsArray()),
      renderer_settings_json.at("stop_label_font_size").AsInt(),
      ParseColor(renderer_settings_json.at("underlayer_color")),
      renderer_settings_json.at("underlayer_width").AsDouble()
    );

    return objects;
  }

  ostream& operator<<(ostream& out, const Renderer::Quoted& quoted) {
    string_view str_view = quoted.str_;
    for (size_t pos = 0, escaping_pos = 0;
         pos != str_view.npos;
         escaping_pos = str_view.find_first_of("\\\"", pos)) {
      escaping_pos = str_view.find_first_of("\\\"", pos);
      if (escaping_pos == str_view.npos) {
        out << str_view.substr(pos);
        pos = escaping_pos;
      }
      else {
        out << str_view.substr(pos, escaping_pos - pos)
          << '\\'
          << str_view[escaping_pos];
        pos = escaping_pos + 1;
      }
    }
    return out;
  }

  const std::string& Renderer::GetRendered(RenderedObjectsTypes type) const {
    return rendered_objects[ObjectIndex(type)];
  }

  string Renderer::Render() const {
    ostringstream out;
    out << Quoted("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>")
        << Quoted("<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">")
        << Quoted(GetRendered(RenderedObjectsTypes::ROUTES))
        << Quoted(GetRendered(RenderedObjectsTypes::STOPS))
        << Quoted(GetRendered(RenderedObjectsTypes::STOPS_NAMES))
        << Quoted("</svg>");
    return out.str();
  }
}

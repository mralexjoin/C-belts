#pragma once

#include "descriptions.h"
#include "json.h"
#include "sphere.h"

#include <array>
#include <cstdint>
#include <map>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

class TransportCatalog;

namespace Rendering {
  enum class RenderedObjectsTypes {
    ROUTES,
    STOPS,
    STOPS_NAMES,
    END
  };

  constexpr std::size_t ObjectIndex(RenderedObjectsTypes type) {
    return static_cast<std::size_t>(type);
  }

  using RenderedObjects = std::array<std::string, ObjectIndex(RenderedObjectsTypes::END)>;

  RenderedObjects RenderObjects(
    const Descriptions::StopsDict& stops_dict,
    const Descriptions::BusesDict& buses_dict,
    const Json::Dict& render_settings_json);

  class Renderer {
  private:
    class Quoted {
    public:
      Quoted(std::string_view str) : str_(str) {}
      friend std::ostream& operator<<(std::ostream& out, const Quoted& quoted);
    private:
      std::string_view str_;
    };

    friend std::ostream& operator<<(std::ostream& out, const Quoted& quoted);
  public:
    Renderer(const Descriptions::StopsDict& stops_dict,
             const Descriptions::BusesDict& buses_dict,
             const Json::Dict& render_settings_json) :
      rendered_objects(move(RenderObjects(stops_dict, buses_dict, render_settings_json))) {}
    std::string Render() const;
  private:
    const RenderedObjects rendered_objects;
    const std::string& GetRendered(RenderedObjectsTypes type) const;
  };
}

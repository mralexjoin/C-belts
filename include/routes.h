#pragma once

#include "requests.h"

#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Routes {
  using StringHolder = std::shared_ptr<std::string>;

  struct Position {
    double latitude;
    double longitude;
  };

  double DistanceBetweenPositions(const Position& lhs, const Position& rhs);

  struct Stop {
    Stop(StringHolder name) :
      name(name) {}
    StringHolder name;
    Position position;
  };

  using StopHolder = std::shared_ptr<Stop>;

  struct RouteStats {
    RouteStats(size_t stops_on_route, size_t unique_stops, double route_length) :
      stops_on_route(stops_on_route),
      unique_stops(unique_stops),
      route_length(route_length) {}
    const size_t stops_on_route;
    const size_t unique_stops;
    const double route_length;
  };

  std::ostream& operator <<(std::ostream& out_stream, const RouteStats& stats);

  using RouteStatsHolder = std::shared_ptr<const RouteStats>;

  class Route {
  public:
    Route(StringHolder number,
          bool is_circular,
          std::vector<StopHolder>&& stops) :
      number(number),
      is_circular(is_circular),
      stops(std::move(stops)) {}
    RouteStatsHolder GetRouteStats() const;
    StringHolder number;
    bool is_circular;
    std::vector<StopHolder> stops;
  private:
    mutable RouteStatsHolder routes_stats;
  };

  using RouteHolder = std::shared_ptr<const Route>;

  class AddStopRequest;
  class AddBusRequest;
  class ReadRouteStatsRequest;

  class Routes {
  public:
    void AddStop(const AddStopRequest* request);
    void AddBus(const AddBusRequest* request);
    RouteStatsHolder GetRouteStats(const ReadRouteStatsRequest* request) const;
  private:
    RouteStatsHolder ComputeRouteStats(const Route* route) const;
    StopHolder GetCreateStop(StringHolder name);
    std::unordered_map<std::string_view, StopHolder> stops;
    std::unordered_map<std::string_view, RouteHolder> routes;
  };
}

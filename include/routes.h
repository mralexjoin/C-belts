#pragma once

#include "requests.h"

#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Routes {
  struct Position {
    double latitude;
    double longitude;
  };

  double DistanceBetweenPositions(const Position& lhs, const Position& rhs);

  class Stop;
  using StopHolder = std::shared_ptr<Stop>;
  struct Stop {
    Stop(StringHolder name) :
      name(name),
      routes(std::make_shared<std::set<std::string_view>>()) {}
    StringHolder name;
    Position position;
    StopRoutesHolder routes;
    std::unordered_map<StopHolder, int> distances_to_stops;
  };

  struct RouteStats {
    RouteStats(size_t stops_on_route,
               size_t unique_stops,
               int route_length,
               double curvature) :
      stops_on_route(stops_on_route),
      unique_stops(unique_stops),
      route_length(route_length),
      curvature(curvature) {}
    const size_t stops_on_route;
    const size_t unique_stops;
    const int route_length;
    const double curvature;
  };

  std::ostream& operator <<(std::ostream& out_stream, const RouteStats& stats);

  using RouteStatsHolder = std::shared_ptr<const RouteStats>;

  class Route {
  public:
    Route(StringHolder number, bool is_circular) :
      number(number),
      is_circular(is_circular) {}
    RouteStatsHolder GetRouteStats() const;
    StringHolder number;
    bool is_circular;
    std::vector<StopHolder> stops;
  private:
    double GetDirectDistance() const;
    int GetDistanceByStops() const;
    size_t GetUniqueStopsCount() const;
    size_t GetStopsCount() const;
    mutable RouteStatsHolder routes_stats;
  };

  class AddStopRequest;
  class AddBusRequest;
  class ReadRouteStatsRequest;
  class ReadStopRoutesRequest;

  class Routes {
  public:
    void AddStop(const AddStopRequest* request);
    void AddBus(const AddBusRequest* request);
    RouteStatsHolder GetRouteStats(const ReadRouteStatsRequest* request) const;
    StopRoutesHolder GetStopRoutes(const ReadStopRoutesRequest* request) const;
  private:
    RouteStatsHolder ComputeRouteStats(const Route* route) const;
    StopHolder GetCreateStop(StringHolder name);
    std::unordered_map<std::string_view, StopHolder> stops;
    std::unordered_map<std::string_view, RouteHolder> routes;
  };
}

#pragma once

#include "json.h"
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

  class Bus;
  using BusHolder = std::shared_ptr<Bus>;

  class Stop;
  using StopHolder = std::shared_ptr<Stop>;
  struct Stop {
    Stop(StringHolder name) :
      name(name),
      buses(std::make_shared<StopBuses>()) {}
    StringHolder name;
    Position position;
    std::unordered_map<StopHolder, int> distances_to_stops;
    StopBusesHolder buses;
  };

  struct BusStats {
    BusStats(size_t stop_count,
               size_t unique_stop_count,
               int route_length,
               double curvature) :
      stop_count(stop_count),
      unique_stop_count(unique_stop_count),
      route_length(route_length),
      curvature(curvature) {}
    Json::Node ToJson() const;
    const size_t stop_count;
    const size_t unique_stop_count;
    const int route_length;
    const double curvature;
  };

  using BusStatsHolder = std::shared_ptr<const BusStats>;

  class Bus {
  public:
    Bus(StringHolder name, bool is_roundtrip) :
      name(name),
      is_roundtrip(is_roundtrip) {}
    BusStatsHolder GetRouteStats() const;
    StringHolder name;
    bool is_roundtrip;
    std::vector<StopHolder> stops;
  private:
    double GetDirectDistance() const;
    int GetDistanceByStops() const;
    size_t GetUniqueStopsCount() const;
    size_t GetStopsCount() const;
    mutable BusStatsHolder routes_stats;
  };

  class AddStopRequest;
  class AddBusRequest;
  class ReadRouteStatsRequest;
  class ReadStopBusesRequest;

  class Routes {
  public:
    void AddStop(const AddStopRequest* request);
    void AddBus(const AddBusRequest* request);
    BusStatsHolder GetRouteStats(const ReadRouteStatsRequest* request) const;
    StopBusesHolder GetStopBuses(const ReadStopBusesRequest* request) const;
  private:
    StopHolder GetCreateStop(StringHolder name);
    std::unordered_map<std::string_view, StopHolder> stops;
    std::unordered_map<std::string_view, BusHolder> buses;
  };
}

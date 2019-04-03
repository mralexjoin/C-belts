#pragma once

#include "json.h"
#include "requests.h"

#include <array>
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

  struct Stop;
  using StopHolder = std::shared_ptr<Stop>;
  struct Stop {
    static Graph::VertexId COUNTER = 0;
    Stop(StringHolder name) :
      name(name),
      buses(std::make_shared<StopBuses>()),
      id(COUNTER++) {}
    StringHolder name;
    Position position;
    std::unordered_map<StopHolder, int> distances_to_stops;
    StopBusesHolder buses;
    Graph::VertexId id;
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

  struct Vertex {
    StopHolder stop;
    BusHolder bus;
  };

  class AddStopRequest;
  class AddBusRequest;
  class ReadRouteStatsRequest;
  class ReadStopBusesRequest;

  class Routes {
  private:
    static const size_t MAX_STOP_SIZE = 100;
    static const size_t MAX_BUS_SIZE = 100;
    static const size_t MAX_VERTEXES_SIZE = MAX_STOP_SIZE * (MAX_BUS_SIZE + 1);
  public:
    Routes() : graph(MAX_VERTEXES_SIZE) {}
    void AddStop(const AddStopRequest* request);
    void AddBus(const AddBusRequest* request);
    BusStatsHolder GetRouteStats(const ReadRouteStatsRequest* request) const;
    StopBusesHolder GetStopBuses(const ReadStopBusesRequest* request) const;
    Route GetRoute(const ReadRouteRequest* request) const;
    void SetSettings(int bus_wait_time, double bus_velocity);
    void BuildRouter();
  private:
    StopHolder GetCreateStop(StringHolder name);
    void CreateRouter() const;

    std::unordered_map<std::string_view, StopHolder> stops;
    std::unordered_map<std::string_view, BusHolder> buses;
    int bus_wait_time;
    double bus_velocity;

    std::array<std::variant<StopHolder, BusHolder>, MAX_VERTEXES_SIZE> edges;
    Graph::DirectedWeightedGraph<Weight> graph;
    std::unique_ptr<Graph::Router<Weight>> router;
  };
}

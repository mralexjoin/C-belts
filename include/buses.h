#pragma once

#include "graph.h"
#include "json.h"
#include "router.h"
#include "util.h"

#include <memory>
#include <numeric>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace BusesRouting {
  struct Position {
    double latitude;
    double longitude;
  };

  class Stop {
  public:
    Stop(StringHolder name) :
      name(name),
      buses(std::make_shared<StopBuses>()) {}
    void SetPosition(Position position) { this->position = position; }
    void SetVertexId(Graph::VertexId id) { this->vertex_id = id; }
    void SetDistance(StopHolder other, int distance) { distances_to_stops[other] = distance; }
    void SetDistanceIfNotSet(StopHolder other, int distance) { distances_to_stops.insert( { other, distance }); }
    void AddBus(const std::string& bus_name) { buses->insert(bus_name); }
    const std::string& GetName() const { return *name; }
    StopBusesHolder GetBuses() const { return buses; }
    int GetDistance(StopHolder other) const { return distances_to_stops.at(other); }
    const Position& GetPosition() const { return position; }
    const Graph::VertexId& GetVertexId() const { return vertex_id; };
  private:
    const StringHolder name;
    Position position;
    std::unordered_map<StopHolder, int> distances_to_stops;
    StopBusesHolder buses;
    Graph::VertexId vertex_id;
  };

  class Bus {
  public:
    Bus(StringHolder name, std::vector<StopHolder> stops, bool is_roundtrip);
    BusStatsHolder GetRouteStats() const;
    size_t GetStopCount() const;
    size_t GetUniqueStopCount() const;
    int GetLengthByStops() const;
    double GetDirectLength() const;
    const std::string& GetName() const { return *name; }
    const std::vector<StopHolder>& GetStops() { return stops; }
    bool IsRoundtrip() const { return is_roundtrip; }
  private:
    const StringHolder name;
    std::vector<StopHolder> stops;
    const bool is_roundtrip;
    mutable BusStatsHolder route_stats;
  };

  enum class RouteItemType {
    WAIT,
    BUS,
    EMPTY
  };

  class RouteItem;
  using RouteItemHolder = std::shared_ptr<const RouteItem>;
  using RouteItemHolders = std::optional<std::vector<RouteItemHolder>>;

  class RouteItem : public JsonSerializable {
  public:
    RouteItem(RouteItemType type) :
      type(type) {}
    RouteItemType GetType() const { return type; }
    virtual Time GetTime() const { return 0; };
    virtual RouteItemHolder Combine(const RouteItemHolder other) const = 0;
    virtual bool IsCombinable(const RouteItemHolder other) const;
    Json::Node ToJson() const override;
    virtual ~RouteItem() = default;
  private:
    const RouteItemType type;
  };

  class EmptyRouteItem : public RouteItem {
  public:
    EmptyRouteItem() : RouteItem(RouteItemType::EMPTY) {}
    bool IsCombinable(const RouteItemHolder) const override { return true; };
    RouteItemHolder Combine(const RouteItemHolder other) const override { return other; }
  private:
    void FillJson(Json::Node&) const override {};
  };

  class TimeRouteItem : public RouteItem {
  public:
    TimeRouteItem(RouteItemType type, Time time) :
      RouteItem(type),
      time(time) {}
    Time GetTime() const override { return time; }
    virtual ~TimeRouteItem() = default;
  private:
    const Time time;
  };

  class WaitRouteItem : public TimeRouteItem {
  public:
    WaitRouteItem(StopHolder stop, Time time) :
      TimeRouteItem(RouteItemType::WAIT, time),
      stop(stop) {}
    bool IsCombinable(const RouteItemHolder) const override { return false; }
    RouteItemHolder Combine(const RouteItemHolder) const override;
  private:
    void FillJson(Json::Node&) const override;
    const StopHolder stop;
  };

  class BusRouteItem : public TimeRouteItem {
  public:
    BusRouteItem(BusHolder bus, Time time, size_t spans) :
      TimeRouteItem(RouteItemType::BUS, time),
      bus(bus),
      spans(spans) {}
    bool IsCombinable(const RouteItemHolder other) const override;
    RouteItemHolder Combine(const RouteItemHolder other) const override;
    size_t GetSpans() const { return spans; }
  private:
    bool IsCombinable(const BusRouteItem& other) const;
    RouteItemHolder Combine(const BusRouteItem& other) const;
    void FillJson(Json::Node&) const override;
    const BusHolder bus;
    const size_t spans;
  };

  class AddStopRequest;
  class AddBusRequest;
  class ReadRouteStatsRequest;
  class ReadStopBusesRequest;
  class ReadRouteRequest;

  class Buses {
  public:
    void AddStop(const AddStopRequest* request);
    void AddBus(const AddBusRequest* request);
    BusStatsHolder GetRouteStats(const ReadRouteStatsRequest* request) const;
    StopBusesHolder GetStopBuses(const ReadStopBusesRequest* request) const;
    RouteItemHolders GetRoute(const ReadRouteRequest* request) const;
    void SetBusWaitTime(int bus_wait_time);
    void SetBusVelocity(double bus_velocity);
    void BuildRouter();
  private:
    StopHolder GetCreateStop(StringHolder name);

    std::unordered_map<std::string_view, StopHolder> stops;
    std::unordered_map<std::string_view, BusHolder> buses;
    int bus_wait_time = 0;
    double bus_velocity = 0;

    std::unique_ptr<Graph::DirectedWeightedGraph<Time>> graph;
    std::unique_ptr<Graph::Router<Time>> router;
    std::vector<StopHolder> vertexes;
    std::vector<RouteItemHolder> edges;
  };
}

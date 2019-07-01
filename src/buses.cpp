#include "profile.h"

#include "buses.h"
#include "requests.h"

#include <cmath>
#include <unordered_set>

TotalDuration dur_create_edges("Create edges");

namespace BusesRouting {
  using namespace std;
  using namespace Json;

  double DegreesToRadians(double degrees) {
    constexpr double DEGREE_TO_RADIANS_COEFF = 3.1415926535 / 180;
    return degrees * DEGREE_TO_RADIANS_COEFF;
  }

  double DistanceBetweenPositions(const Position& lhs, const Position& rhs) {
    constexpr double EARTH_RADIUS = 6371000;

    double half_latitude_diff_sin = sin(DegreesToRadians(rhs.latitude - lhs.latitude) / 2);
    double half_longitude_diff_sin = sin(DegreesToRadians(rhs.longitude - lhs.longitude) / 2);

    double a = half_latitude_diff_sin * half_latitude_diff_sin
      + half_longitude_diff_sin * half_longitude_diff_sin
      * cos(DegreesToRadians(lhs.latitude)) * cos(DegreesToRadians(rhs.latitude));
    return 2 * EARTH_RADIUS * atan2(sqrt(a), sqrt(1 - a));
  }

  void Buses::SetBusWaitTime(int bus_wait_time) {
    this->bus_wait_time = bus_wait_time;
  }

  void Buses::SetBusVelocity(double bus_velocity) {
    this->bus_velocity = bus_velocity * 1000 / 60;
  }

  StopHolder Buses::GetCreateStop(StringHolder name) {
    if (auto it = stops.find(*name); it != stops.end()) {
      return it->second;
    }

    auto stop = make_shared<Stop>(name);
    return stops.insert({ stop->GetName(), stop }).first->second;
  }

  void Buses::AddStop(const AddStopRequest* request) {
    auto stop = GetCreateStop(request->name);
    stop->SetPosition({ request->latitude, request->longitude });
    for (const auto& [another_stop_name, distance] : request->road_distances) {
      auto another_stop = GetCreateStop(another_stop_name);
      another_stop->SetDistanceIfNotSet(stop, distance);
      stop->SetDistance(another_stop, distance);
    }
    stop->SetVertexId(vertexes.size());
    vertexes.push_back(stop);
  }

  void Buses::AddBus(const AddBusRequest* request) {
    vector<StopHolder> bus_stops;
    bus_stops.reserve(request->stops.size());
    for (const auto& name : request->stops) {
      bus_stops.push_back(GetCreateStop(name));
    }
    BusHolder bus = make_shared<Bus>(request->name,
                                     move(bus_stops),
                                     request->is_roundtrip);
    buses.insert({ bus->GetName(), bus });
  }

  BusStatsHolder Buses::GetRouteStats(const ReadRouteStatsRequest* request) const {
    if (auto it = buses.find(request->name); it != buses.end()) {
      return it->second->GetRouteStats();
    }
    return nullptr;
  }

  StopBusesHolder Buses::GetStopBuses(const ReadStopBusesRequest* request) const {
    if (auto it = stops.find(request->name); it != stops.end()) {
      return it->second->GetBuses();
    }
    return nullptr;
  }

  Bus::Bus(StringHolder name_, vector<StopHolder> stops_, bool is_roundtrip) :
    name(name_),
    stops(move(stops_)) {
    const auto& str_name = *name;
    for (const auto& stop : stops) {
      stop->AddBus(str_name);
    }
    if (!is_roundtrip && !stops.empty()) {
      stops.reserve(2 * stops.size() - 1);
      for (size_t i = stops.size() - 1; i > 0; --i) {
        stops.push_back(stops[i - 1]);
      }
    }
  }

  int Bus::GetLengthByStops() const {
    int length = 0;
    for (auto from_stop = stops.begin(), to_stop = next(from_stop);
        to_stop != stops.end();
        from_stop = next(from_stop), to_stop = next(to_stop)) {
      length += (*from_stop)->GetDistance(*to_stop);
    }
    return length;
  }

  double Bus::GetDirectLength() const {
    double length = 0;
    for (auto from_stop = stops.begin(), to_stop = next(from_stop);
        to_stop != stops.end();
        from_stop = next(from_stop), to_stop = next(to_stop)) {
      length += DistanceBetweenPositions((*from_stop)->GetPosition(),
                                         (*to_stop)->GetPosition());
    }
    return length;
  }

  size_t Bus::GetUniqueStopCount() const {
    unordered_set<StopHolder> unique_stop_count;
    for (const auto stop : stops) {
      unique_stop_count.insert(stop);
    }
    return unique_stop_count.size();
  }

  size_t Bus::GetStopCount() const {
    return stops.size();
  }

  BusStatsHolder Bus::GetRouteStats() const {
    if (!route_stats) {
      route_stats = make_shared<BusStats>(*this);
    }
    return route_stats;
  }

  void Buses::BuildRouter() {
    size_t vertexes_count = vertexes.size();
    for (const auto& [name, bus] : buses) {
      vertexes_count += bus->GetStopCount();
    }
    graph = make_unique<Graph::DirectedWeightedGraph<Time>>(vertexes_count);

    double wait_time = static_cast<double>(bus_wait_time);

    Graph::VertexId vertex_id = vertexes.size();
    for (const auto& [name, bus] : buses) {

      Graph::VertexId start_id = vertex_id;

      const auto& bus_stops = bus->GetStops();

      for (const auto& stop : bus_stops) {
        edges.push_back(make_shared<WaitRouteItem>(stop, wait_time));
        graph->AddEdge({ stop->GetVertexId(), vertex_id, wait_time });

        edges.push_back(make_shared<EmptyRouteItem>());
        graph->AddEdge({ vertex_id, stop->GetVertexId(), 0 });
        ++vertex_id;
      }
      vertex_id = start_id;
      for (auto from_stop = bus_stops.begin(), to_stop = next(from_stop);
           to_stop != bus_stops.end();
           from_stop = next(from_stop), to_stop = next(to_stop)) {
        auto move_time = (*from_stop)->GetDistance(*to_stop) / bus_velocity;
        edges.push_back(make_shared<BusRouteItem>(bus, move_time, 1));
        graph->AddEdge({ vertex_id, vertex_id + 1, move_time });
        ++vertex_id;
      }
      ++vertex_id;
    }
    router = make_unique<Graph::Router<Time>>(*graph);
  }

  RouteItemHolders Buses::GetRoute(const ReadRouteRequest* request) const {
    auto route = router->BuildRoute(stops.at(request->from)->GetVertexId(),
                                    stops.at(request->to)->GetVertexId());
    if (!route) {
      return nullopt;
    }
    std::vector<RouteItemHolder> items;
    for (size_t i = 0; i < route->edge_count; ++i) {
      auto edge = edges[router->GetRouteEdge(route->id, i)];
      if (!items.empty() && edge->IsCombinable(items.back())) {
        edge = edge->Combine(items.back());
        items.pop_back();
      }
      items.push_back(edge);
    }
    router->ReleaseRoute(route->id);
    return items;
  }

  string RouteItemTypeToString(RouteItemType type) {
    switch (type) {
    case RouteItemType::WAIT:
      return "Wait";
    case RouteItemType::BUS:
      return "Bus";
    default:
      throw invalid_argument("Empty is not valid route item type");
    }
  }

  bool RouteItem::IsCombinable(const RouteItemHolder other) const {
    return other->type == type;
  }

  Node RouteItem::ToJson() const {
    Node object = Node(map<string, Node>({
          { "type", Node(RouteItemTypeToString(type)) },
          { "time", Node(GetTime()) }
        }));
    FillJson(object);
    return object;
  }

  RouteItemHolder WaitRouteItem::Combine(const RouteItemHolder) const {
    throw invalid_argument("Cannot combine stops waiting");
  }

  void WaitRouteItem::FillJson(Node& node) const {
    auto& object = node.AsMap();
    object["stop_name"] = Node(stop->GetName());
  }

  bool BusRouteItem::IsCombinable(const RouteItemHolder other) const {
    return RouteItem::IsCombinable(other)
      && static_cast<const BusRouteItem&>(*other).IsCombinable(*this);
  }

  bool BusRouteItem::IsCombinable(const BusRouteItem& other) const {
    return bus == other.bus;
  }

  RouteItemHolder BusRouteItem::Combine(const RouteItemHolder other) const {
    return static_cast<const BusRouteItem&>(*other).Combine(*this);
  }

  RouteItemHolder BusRouteItem::Combine(const BusRouteItem& other) const {
    return make_shared<const BusRouteItem>(bus, GetTime() + other.GetTime(), spans + other.GetSpans());
  }

  void BusRouteItem::FillJson(Node& node) const {
    auto& object = node.AsMap();
    object["bus"] = Node(bus->GetName());
    object["span_count"] = Node(static_cast<int>(spans));
  }
}

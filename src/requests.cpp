#include "requests.h"
//#include "profile.h"

#include <algorithm>
#include <numeric>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace Json;

//TotalDuration dur_ReadStopBusesRequest("ReadStopBusesRequest");
//TotalDuration dur_ReadRouteRequest("ReadRouteRequest");
//TotalDuration dur_ReadRouteStatsRequest("ReadRouteStatsRequest");
//TotalDuration dur_FromJson("FromJson");
//TotalDuration dur_ToJson("ToJson");

namespace BusesRouting {
  const std::unordered_map<std::string_view, ModifyRequest::Type> ModifyRequest::STR_TO_REQUEST_TYPE = {
    {"Stop", Type::STOP},
    {"Bus", Type::BUS}
  };

  const std::unordered_map<std::string_view, ReadRequest::Type> ReadRequest::STR_TO_REQUEST_TYPE = {
    {"Stop", Type::STOP},
    {"Bus", Type::BUS},
    {"Route", Type::ROUTE}
  };

  ModifyRequestHolder ModifyRequest::Create(ModifyRequest::Type type) {
    switch(type) {
    case Type::STOP:
      return make_unique<AddStopRequest>();
    case Type::BUS:
      return make_unique<AddBusRequest>();
    default:
      return nullptr;
    }
  }

  void AddStopRequest::FromJson(const Node& node) {
    const auto& object = node.AsMap();
    name = make_shared<string>(object.at("name").AsString());
    latitude = object.at("latitude").AsDouble();
    longitude = object.at("longitude").AsDouble();

    if (const auto& it = object.find("road_distances");
        it != object.end()) {
      const auto& road_distances_json = it->second.AsMap();
      road_distances.reserve(road_distances_json.size());
      for (const auto& [name, distance] : road_distances_json) {
        road_distances.push_back(pair(make_shared<string>(name), distance.AsInt()));
      }
    }
  }

  void AddStopRequest::Execute(Buses& buses) const {
    buses.AddStop(this);
  }

  void AddBusRequest::FromJson(const Node& node) {
    const auto& object = node.AsMap();
    name = make_shared<string>(object.at("name").AsString());
    is_roundtrip = object.at("is_roundtrip").AsBool();

    const auto& stops_json = object.at("stops").AsArray();
    stops.reserve(stops.size());
    for (const auto& name : stops_json) {
      stops.push_back(make_shared<string>(name.AsString()));
    }
  }

  void AddBusRequest::Execute(Buses& buses) const {
    buses.AddBus(this);
  }

  ReadRequestHolder ReadRequest::Create(ReadRequest::Type type) {
    switch(type) {
    case Type::STOP:
      return make_unique<ReadStopBusesRequest>();
    case Type::BUS:
      return make_unique<ReadRouteStatsRequest>();
    case Type::ROUTE:
      return make_unique<ReadRouteRequest>();
    default:
      return nullptr;
    }
  }

  void ReadRequest::FromJson(const Node& node) {
    id = node.AsMap().at("id").AsInt();
  }

  void ReadRouteStatsRequest::FromJson(const Node& node) {
    ReadRequest::FromJson(node);
    name = node.AsMap().at("name").AsString();
  }

  ResponseHolder ReadRouteStatsRequest::Execute(const Buses& buses) const {
    //ADD_DURATION(dur_ReadRouteStatsRequest);
    return make_unique<BusResponse>(id, buses.GetRouteStats(this));
  }
  void ReadStopBusesRequest::FromJson(const Node& node) {
    ReadRequest::FromJson(node);
    name = node.AsMap().at("name").AsString();
  }

  ResponseHolder ReadStopBusesRequest::Execute(const Buses& buses) const {
    //ADD_DURATION(dur_ReadStopBusesRequest);
    return make_unique<StopResponse>(id, buses.GetStopBuses(this));
  }

  void ReadRouteRequest::FromJson(const Node& node) {
    //ADD_DURATION(dur_FromJson);
    ReadRequest::FromJson(node);
    from = node.AsMap().at("from").AsString();
    to = node.AsMap().at("to").AsString();
  }

  ResponseHolder ReadRouteRequest::Execute(const Buses& buses) const {
    //ADD_DURATION(dur_ReadRouteRequest);
    return make_unique<RouteResponse>(id, buses.GetRoute(this));
  }

  Node Response::ToJson() const {
    //ADD_DURATION(dur_ToJson);
    Node object = Node(map<string, Node>({ { "request_id", id } }));
    if (!Empty()) {
      FillJson(object);
    }
    else {
      object.AsMap()["error_message"] = "not found"s;
    }
    return object;
  }

  bool BusResponse::Empty() const {
    return route_stats == nullptr;
  }

  void BusResponse::FillJson(Node& node) const {
    auto& object_map = node.AsMap();
    object_map["route_length"] = route_stats->route_length;
    object_map["stop_count"] = static_cast<int>(route_stats->stop_count);
    object_map["unique_stop_count"] = static_cast<int>(route_stats->unique_stop_count);
    object_map["curvature"] = route_stats->curvature;
  }

  bool StopResponse::Empty() const {
    return buses == nullptr;
  }

  void StopResponse::FillJson(Node& node) const {
    auto& object_map = node.AsMap();
    vector<Node> stop_buses;
    stop_buses.reserve(buses->size());
    for (const auto& bus : *buses) {
      stop_buses.emplace_back(string(bus));
    }
    object_map["buses"] = move(stop_buses);
  }

  void RouteResponse::FillJson(Node& node) const {
    auto& object_map = node.AsMap();
    auto& json_items = object_map["items"].AsArray();
    Time total_time = 0;
    for (const auto& item : *items) {
      json_items.emplace_back(item->ToJson());
      total_time += item->GetTime();
    }
    object_map["total_time"] = total_time;
  }

  bool RouteResponse::Empty() const {
    return items == nullopt;
  }
}

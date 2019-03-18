#include "requests.h"
#include "routes.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace Json;

namespace Routes {
  const std::unordered_map<std::string_view, ModifyRequest::Type> ModifyRequest::STR_TO_REQUEST_TYPE = {
    {"Stop", Type::STOP},
    {"Bus", Type::BUS}
  };

  const std::unordered_map<std::string_view, ReadRequest::Type> ReadRequest::STR_TO_REQUEST_TYPE = {
    {"Stop", Type::STOP},
    {"Bus", Type::BUS}
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

    const auto& road_distances_json = object.at("road_distances").AsMap();
    road_distances.reserve(road_distances_json.size());
    for (const auto& [name, distance] : road_distances_json) {
      road_distances.push_back(pair(make_shared<string>(name), distance.AsInt()));
    }
  }

  void AddStopRequest::Execute(Routes& buses) const {
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
    if (is_roundtrip) {
      stops.pop_back();
    }
  }

  void AddBusRequest::Execute(Routes& buses) const {
    buses.AddBus(this);
  }

  ReadRequestHolder ReadRequest::Create(ReadRequest::Type type) {
    switch(type) {
    case Type::STOP:
      return make_unique<ReadStopBusesRequest>();
    case Type::BUS:
      return make_unique<ReadRouteStatsRequest>();
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

  ResponseHolder ReadRouteStatsRequest::Execute(const Routes& buses) const {
    return make_unique<BusResponse>(id, buses.GetRouteStats(this));
  }

  void ReadStopBusesRequest::FromJson(const Node& node) {
    ReadRequest::FromJson(node);
    name = node.AsMap().at("name").AsString();
  }

  ResponseHolder ReadStopBusesRequest::Execute(const Routes& buses) const {
    return make_unique<StopResponse>(id, buses.GetStopBuses(this));
  }

  Node Response::ToJson() const {
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
      stop_buses.push_back(string(bus));
    }
    object_map["buses"] = move(stop_buses);
  }
}

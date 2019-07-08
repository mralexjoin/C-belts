#pragma once

#include "json.h"
#include "util.h"
#include "buses.h"

#include <iostream>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace BusesRouting {
  template <typename Request>
  std::optional<typename Request::Type> ConvertRequestTypeFromString(std::string_view type_str) {
    if (const auto it = Request::STR_TO_REQUEST_TYPE.find(type_str);
        it != Request::STR_TO_REQUEST_TYPE.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  template <typename Request>
  std::unique_ptr<Request> ParseRequest(const Json::Node& node) {
    using namespace std;

    const auto request_type
      = ConvertRequestTypeFromString<Request>(node.AsMap().at("type").AsString());
    if (!request_type) {
      return nullptr;
    }
    unique_ptr<Request> request = Request::Create(*request_type);
    if (request) {
      request->FromJson(node);
    }
    return request;
  }

  template <typename Request>
  std::vector<std::unique_ptr<Request>> ReadRequests(const Json::Node& node) {
    using namespace std;

    const auto& requests_json = node.AsArray();
    vector<unique_ptr<Request>> requests;
    requests.reserve(requests_json.size());

    for (const auto& request_json : requests_json) {
      if (auto request = ParseRequest<Request>(request_json)) {
        requests.emplace_back(move(request));
      }
    }

    return requests;
  }

  template <typename Type>
  class Request {
  public:
    Request(Type type) : type(type) {}
    virtual void FromJson(const Json::Node& node) = 0;
    virtual ~Request() = default;
  private:
    const Type type;
  };

  enum class ModifyRequestType {
    STOP,
    BUS
  };

  class Buses;

  class ModifyRequest;
  using ModifyRequestHolder = std::unique_ptr<ModifyRequest>;

  class ModifyRequest : public ::BusesRouting::Request<ModifyRequestType> {
  public:
    using Type = ModifyRequestType;
    using Request::Request;
    static const std::unordered_map<std::string_view, Type> STR_TO_REQUEST_TYPE;
    static ModifyRequestHolder Create(Type type);
    virtual void Execute(Buses& buses) const = 0;
  };

  class AddStopRequest final : public ModifyRequest {
  public:
    friend class Buses;
    AddStopRequest() : ModifyRequest(Type::STOP) {}
    void FromJson(const Json::Node& node) override;
    void Execute(Buses& buses) const override;
  private:
    StringHolder name;
    double latitude;
    double longitude;
    std::vector<std::pair<StringHolder, int>> road_distances;
  };

  class AddBusRequest final : public ModifyRequest {
  public:
    friend class Buses;
    AddBusRequest() : ModifyRequest(Type::BUS) {}
    void FromJson(const Json::Node& node) override;
    void Execute(Buses& buses) const override;
  private:
    StringHolder name;
    bool is_roundtrip;
    std::vector<StringHolder> stops;
  };

  class Response : public JsonSerializable {
  public:
    Response(int id) : id(id) {}
    Json::Node ToJson() const override;
    virtual ~Response() = default;
  private:
    virtual bool Empty() const = 0;
    const int id;
  };

  struct BusStats {
    BusStats(const Bus& bus) :
      stop_count(bus.GetStopCount()),
      unique_stop_count(bus.GetUniqueStopCount()),
      route_length(bus.GetLengthByStops()),
      curvature(route_length / bus.GetDirectLength()) {}
    const size_t stop_count;
    const size_t unique_stop_count;
    const int route_length;
    const double curvature;
  };

  class BusResponse final : public Response {
  public:
    BusResponse(int id, const BusStatsHolder route_stats) :
      Response(id),
      route_stats(route_stats) {}
  private:
    bool Empty() const override;
    void FillJson(Json::Node& node) const override;
    const BusStatsHolder route_stats;
  };

  class StopResponse final : public Response {
  public:
    StopResponse(int id, StopBusesHolder buses) :
       Response(id),
      buses(buses) {}
  private:
    bool Empty() const override;
    void FillJson(Json::Node& node) const override;
    const StopBusesHolder buses;
  };

  class RouteResponse final : public Response {
  public:
    RouteResponse(int id, RouteItemHolders items) :
      Response(id),
      items(move(items)) {}
  private:
    bool Empty() const override;
    void FillJson(Json::Node& node) const override;
    RouteItemHolders items;
  };

  enum class ReadRequestType {
    BUS,
    STOP,
    ROUTE
  };

  class ReadRequest;
  using ReadRequestHolder = std::unique_ptr<ReadRequest>;

  using ResponseHolder = std::unique_ptr<Response>;

  class ReadRequest : public Request<ReadRequestType> {
  public:
    using Type = ReadRequestType;
    using Request::Request;
    static const std::unordered_map<std::string_view, Type> STR_TO_REQUEST_TYPE;
    static ReadRequestHolder Create(Type type);
    virtual void FromJson(const Json::Node& node) override;
    virtual ResponseHolder Execute(const Buses& buses) const = 0;
  protected:
    int id;
  };

  class ReadRouteStatsRequest final : public ReadRequest {
  public:
    friend class Buses;
    ReadRouteStatsRequest() : ReadRequest(Type::BUS) {}
    void FromJson(const Json::Node& node) override;
    ResponseHolder Execute(const Buses& buses) const override;
  private:
    std::string name;
  };

  class ReadStopBusesRequest final : public ReadRequest {
  public:
    friend class Buses;
    ReadStopBusesRequest() : ReadRequest(Type::STOP) {}
    void FromJson(const Json::Node& node) override;
    ResponseHolder Execute(const Buses& buses) const override;
  private:
    std::string name;
  };

  class ReadRouteRequest final : public ReadRequest {
  public:
    friend class Buses;
    ReadRouteRequest() : ReadRequest(Type::ROUTE) {}
    void FromJson(const Json::Node& node) override;
    ResponseHolder Execute(const Buses& buses) const override;
  private:
    std::string from;
    std::string to;
  };
}

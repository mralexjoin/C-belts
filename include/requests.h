#pragma once

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Routes {
  class Route;
  using RouteHolder = std::shared_ptr<Route>;

  struct Stop;
  struct RouteStats;
  using RouteStatsHolder = std::shared_ptr<const RouteStats>;
  using StringHolder = std::shared_ptr<std::string>;
  using StopRoutesHolder = std::shared_ptr<std::set<std::string_view>>;

  class Routes;

  double ConvertToDouble(std::string_view str);
  int ConvertToInt(std::string_view str);

  StringHolder ConverToStringHolder(std::string_view str);

  std::pair<std::string_view, std::optional<std::string_view>>
  SplitTwoStrict(std::string_view s, std::string_view delimiter = " ");

  std::pair<std::string_view, std::string_view>
  SplitTwo(std::string_view s, std::string_view delimiter = " ");

  std::string_view TrimLeft(std::string_view s);

  std::string_view TrimRight(std::string_view s);

  std::string_view TrimAll(std::string_view s);

  std::string_view ReadToken(std::string_view& s, std::string_view delimiter = " ");

  template <typename Number>
  Number ReadNumberOnLine(std::istream& in_stream) {
    Number number;
    in_stream >> number;
    std::string dummy;
    getline(in_stream, dummy);
    return number;
  }

  template <typename Request>
  std::optional<typename Request::Type> ConvertRequestTypeFromString(std::string_view type_str) {
    if (const auto it = Request::STR_TO_REQUEST_TYPE.find(type_str);
        it != Request::STR_TO_REQUEST_TYPE.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  template <typename Request>
  std::unique_ptr<Request> ParseRequest(std::string_view request_str) {
    using namespace std;
    const auto request_type
      = ConvertRequestTypeFromString<Request>(ReadToken(request_str));
    if (!request_type) {
      return nullptr;
    }
    unique_ptr<Request> request = Request::Create(*request_type);
    if (request) {
      request->ParseFrom(request_str);
    }
    return request;
  }

  template <typename Request>
  std::vector<std::unique_ptr<Request>> ReadRequests(std::istream& in_stream) {
    using namespace std;
    const size_t request_count = ReadNumberOnLine<size_t>(in_stream);

    vector<unique_ptr<Request>> requests;
    requests.reserve(request_count);

    for (size_t i = 0; i < request_count; ++i) {
      string request_str;
      getline(in_stream, request_str);
      if (auto request = ParseRequest<Request>(request_str)) {
        requests.push_back(move(request));
      }
    }
    return requests;
  }

  template <typename Type>
  class Request {
  public:
    Request(Type type) : type(type) {}
    virtual void ParseFrom(std::string_view input) = 0;
    virtual ~Request() = default;
  private:
    const Type type;
  };

  enum class ModifyRequestType {
    STOP,
    BUS
  };

  class ModifyRequest;
  using ModifyRequestHolder = std::unique_ptr<ModifyRequest>;

  class ModifyRequest : public ::Routes::Request<ModifyRequestType> {
  public:
    using Type = ModifyRequestType;
    using Request::Request;
    static const std::unordered_map<std::string_view, Type> STR_TO_REQUEST_TYPE;
    static ModifyRequestHolder Create(Type type);
    virtual void Execute(Routes& routes) const = 0;
  };

  class AddStopRequest final : public ModifyRequest {
  public:
    friend class Routes;
    AddStopRequest() : ModifyRequest(Type::STOP) {}
    void ParseFrom(std::string_view input) override;
    void Execute(Routes& routes) const override;
  private:
    StringHolder name;
    double latitude;
    double longitude;
    std::vector<std::pair<StringHolder, int>> distances_to_stops;
  };

  class AddBusRequest final : public ModifyRequest {
  public:
    friend class Routes;
    AddBusRequest() : ModifyRequest(Type::BUS) {}
    void ParseFrom(std::string_view input) override;
    void Execute(Routes& routes) const override;
  private:
    static std::string_view GetStopsDelimiter(std::string_view input);
    StringHolder number;
    bool is_circular;
    std::vector<StringHolder> stops;
  };

  class Response;
  using ResponseHolder = std::unique_ptr<Response>;

  class Response {
  public:
    virtual void Print(std::ostream& out_stream) const = 0;
    virtual ~Response() = default;
  };

  std::ostream& operator <<(std::ostream& out_stream, const Response& response);
  std::ostream& operator <<(std::ostream& out_stream, const RouteStats& stats);

  class BusResponse final : public Response {
  public:
    BusResponse(std::string_view bus_number, const RouteStatsHolder route_stats) :
      bus_number(bus_number),
      route_stats(route_stats) {}
    void Print(std::ostream& out_stream) const override;
  private:
    std::string_view bus_number;
    RouteStatsHolder route_stats;
  };

  class StopResponse final : public Response {
  public:
    StopResponse(std::string_view stop_name, StopRoutesHolder routes) :
      stop_name(stop_name),
      routes(routes) {}
    void Print(std::ostream& out_stream) const override;
  private:
    std::string_view stop_name;
    StopRoutesHolder routes;
  };

  enum class ReadRequestType {
    BUS,
    STOP
  };

  class ReadRequest;
  using ReadRequestHolder = std::unique_ptr<ReadRequest>;

  class ReadRequest : public Request<ReadRequestType> {
  public:
    using Type = ReadRequestType;
    using Request::Request;
    static const std::unordered_map<std::string_view, Type> STR_TO_REQUEST_TYPE;
    static ReadRequestHolder Create(Type type);
    virtual ResponseHolder Execute(const Routes& routes) const = 0;
  };

  class ReadRouteStatsRequest final : public ReadRequest {
  public:
    friend class Routes;
    ReadRouteStatsRequest() : ReadRequest(Type::BUS) {}
    void ParseFrom(std::string_view input) override;
    ResponseHolder Execute(const Routes& routes) const override;
  private:
    std::string bus_number;
  };

  class ReadStopRoutesRequest final : public ReadRequest {
  public:
    friend class Routes;
    ReadStopRoutesRequest() : ReadRequest(Type::STOP) {}
    void ParseFrom(std::string_view input) override;
    ResponseHolder Execute(const Routes& routes) const override;
  private:
    std::string stop_name;
  };
}

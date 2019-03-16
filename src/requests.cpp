#include "requests.h"
#include "routes.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

using namespace std;

namespace Routes {
  double ConvertToDouble(string_view str) {
    size_t pos;
    const double result = stod(string(str), &pos);
    if (pos != str.length()) {
      stringstream error;
      error << "string " << str << " contains "
            << (str.length() - pos) << " trailing chars";
      throw invalid_argument(error.str());
    }
    return result;
  }

  StringHolder ConverToStringHolder(string_view str) {
    return make_shared<string>(str);
  }

  pair<string_view, optional<string_view>>
  SplitTwoStrict(string_view s, string_view delimiter) {
    const size_t pos = s.find(delimiter);
    if (pos == s.npos) {
      return {s, nullopt};
    }
    return {s.substr(0, pos), s.substr(pos + delimiter.length())};
  }

  pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter) {
    const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
    return {lhs, rhs_opt.value_or("")};
  }

  string_view TrimLeft(string_view s) {
    s.remove_prefix(min(s.find_first_not_of(' '), s.size()));
    return s;
  }

  string_view TrimRight(string_view s) {
    if (size_t last = s.find_last_not_of(' '); last != s.npos) {
      s.remove_suffix(s.size() - last - 1);
    }
    else {
      s.remove_suffix(s.size());
    }
    return s;
  }

  string_view TrimAll(string_view s) {
    return TrimLeft(TrimRight(s));
  }

  string_view ReadToken(string_view& s, string_view delimiter) {
    const auto [lhs, rhs] = SplitTwo(TrimLeft(s), delimiter);
    s = rhs;
    return TrimRight(lhs);
  }

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

  void AddStopRequest::ParseFrom(string_view input) {
    name = ConverToStringHolder(ReadToken(input, ":"));
    latitude = ConvertToDouble(ReadToken(input, ","));
    longitude = ConvertToDouble(ReadToken(input));
    input = TrimAll(input);
    if (input != "") {
      stringstream error;
      error << "request is not fully parsed, trailing chars = " << input;
      throw invalid_argument(error.str());
    }
  }

  void AddStopRequest::Execute(Routes& routes) const {
    routes.AddStop(this);
  }

  void AddBusRequest::ParseFrom(string_view input) {
    number = ConverToStringHolder(ReadToken(input, ":"));

    string_view delimiter = GetStopsDelimiter(input);
    is_circular = (delimiter == ">");

    while (input.length() > 0) {
      stops.push_back(ConverToStringHolder(ReadToken(input, delimiter)));
    }
    if (is_circular) {
      stops.pop_back();
    }
  }

  void AddBusRequest::Execute(Routes& routes) const {
    routes.AddBus(this);
  }

  string_view AddBusRequest::GetStopsDelimiter(string_view input) {
    unordered_set<char> delimiter_candidates = {'-', '>'};
    for (size_t i = 0; i < input.length(); ++i) {
      if (delimiter_candidates.find(input[i]) != delimiter_candidates.end()) {
        return input.substr(i, 1);
      }
    }
    stringstream error;
    error << "cannot get stops delimiter from input = " << input;
    throw invalid_argument(error.str());
  }

  ReadRequestHolder ReadRequest::Create(ReadRequest::Type type) {
    switch(type) {
    case Type::STOP:
      return make_unique<ReadStopRoutesRequest>();
    case Type::BUS:
      return make_unique<ReadRouteStatsRequest>();
    default:
      return nullptr;
    }
  }

  void ReadRouteStatsRequest::ParseFrom(string_view input) {
    bus_number = string(TrimAll(input));
  }

  ResponseHolder ReadRouteStatsRequest::Execute(const Routes& routes) const {
    return make_unique<BusResponse>(bus_number, routes.GetRouteStats(this));
  }

  void BusResponse::Print(ostream& out_stream) const {
    out_stream << "Bus " << bus_number << ": ";
    if (route_stats) {
      out_stream << *route_stats;
    }
    else {
      out_stream << "not found";
    }
  }

  void ReadStopRoutesRequest::ParseFrom(string_view input) {
    stop_name = string(TrimAll(input));
  }

  ResponseHolder ReadStopRoutesRequest::Execute(const Routes& routes) const {
    return make_unique<StopResponse>(stop_name, routes.GetStopRoutes(this));
  }

  void StopResponse::Print(ostream& out_stream) const {
    out_stream << "Stop " << stop_name << ": ";
    if (routes) {
      if (routes->empty()) {
        out_stream << "no buses";
      }
      else {
        out_stream << "buses";
        for (const auto& route : *routes) {
          out_stream << ' ' << route;
        }
      }
    }
    else {
      out_stream << "not found";
    }
  }

  ostream& operator <<(ostream& out_stream, const Response& response) {
    response.Print(out_stream);
    return out_stream;
  }
}

#include "routes.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>

using namespace std;

namespace Routes {
   StopHolder Routes::GetCreateStop(StringHolder name) {
    if (auto it = stops.find(*name); it != stops.end()) {
      return it->second;
    }

    auto stop = make_shared<Stop>(name);
    return stops.insert({ *stop->name, stop }).first->second;
  }

  void Routes::AddStop(const AddStopRequest* request) {
    auto stop = GetCreateStop(request->name);
    stop->position = Position({ request->latitude, request->longitude });
    for (const auto& [another_stop_name, distance] : request->road_distances) {
      auto another_stop = GetCreateStop(another_stop_name);
      another_stop->distances_to_stops.insert({ stop, distance });
      stop->distances_to_stops[another_stop] = distance;
    }
  }

  void Routes::AddBus(const AddBusRequest* request) {
    BusHolder bus = make_shared<Bus>(request->name,
                                     request->is_roundtrip);
    bus->stops.reserve(request->stops.size());
    for (const auto& name : request->stops) {
      StopHolder stop = GetCreateStop(name);
      stop->buses->insert(*bus->name);
      bus->stops.push_back(stop);
    }

    buses.insert({ *bus->name, bus });
  }

  BusStatsHolder Routes::GetRouteStats(const ReadRouteStatsRequest* request) const {
    if (auto it = buses.find(request->name); it != buses.end()) {
      return it->second->GetRouteStats();
    }
    return nullptr;
  }

  StopBusesHolder Routes::GetStopBuses(const ReadStopBusesRequest* request) const {
    if (auto it = stops.find(request->name); it != stops.end()) {
      return it->second->buses;
    }
    return nullptr;
  }

  int Bus::GetDistanceByStops() const {
    int distance = 0;
    for (auto from_stop = stops.begin(), to_stop = next(from_stop);
        to_stop != stops.end();
        from_stop = next(from_stop), to_stop = next(to_stop)) {
      distance += (*from_stop)->distances_to_stops.at(*to_stop);
    }
    if (is_roundtrip && !stops.empty()) {
      distance += stops.back()->distances_to_stops.at(stops.front());
    }
    else {
      for (auto from_stop = stops.rbegin(), to_stop = next(from_stop);
           to_stop != stops.rend();
           from_stop = next(from_stop), to_stop = next(to_stop)) {
        distance += (*from_stop)->distances_to_stops.at(*to_stop);
      }
    }
    return distance;
  }

  double Bus::GetDirectDistance() const {
    double distance = 0;
    for (auto from_stop = stops.begin(), to_stop = next(from_stop);
        to_stop != stops.end();
        from_stop = next(from_stop), to_stop = next(to_stop)) {
      distance += DistanceBetweenPositions((*from_stop)->position, (*to_stop)->position);
    }
    if (is_roundtrip && !stops.empty()) {
      return distance + DistanceBetweenPositions(stops.back()->position, stops.front()->position);
    }
    return 2 * distance;
  }

  size_t Bus::GetUniqueStopsCount() const {
    unordered_set<StopHolder> unique_stop_count;
    for (const auto stop : stops) {
      unique_stop_count.insert(stop);
    }
    return unique_stop_count.size();
  }

  size_t Bus::GetStopsCount() const {
    return stops.size() + (is_roundtrip ? 1 : (stops.size() - 1));
  }

  BusStatsHolder Bus::GetRouteStats() const {
    if (!routes_stats) {
      int distance = GetDistanceByStops();
      routes_stats = make_shared<BusStats>(
        GetStopsCount(),
        GetUniqueStopsCount(),
        distance,
        distance / GetDirectDistance()
      );
    }
    return routes_stats;
  }

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

  ostream& operator <<(ostream& out_stream, const BusStats& stats) {
    return out_stream << stats.stop_count << " stops on route, "
                      << stats.unique_stop_count << " unique stops, "
                      << stats.route_length << " route length, "
                      << stats.curvature << " curvature";
  }
};

#include "routes.h"

#include <cmath>
#include <unordered_set>

using namespace std;

namespace Routes {
  StopHolder Routes::GetCreateStop(StringHolder name) {
    if (auto it = stops.find(*name); it != stops.end()) {
      return it->second;
    }

    StopHolder stop = make_shared<Stop>(name);
    return stops.insert({ *stop->name, stop }).first->second;
  }

  void Routes::AddStop(const AddStopRequest* request) {
    GetCreateStop(request->name)->position 
      = Position({ request->latitude, request->longitude });
  }

  void Routes::AddBus(const AddBusRequest* request) {
    RouteHolder route = make_shared<Route>(request->number,
                                           request->is_circular);
    route->stops.reserve(request->stops.size());
    for (StringHolder stop_name : request->stops) {
      StopHolder stop = GetCreateStop(stop_name);
      stop->routes->insert(*route->number);
      route->stops.push_back(stop);
    }

    routes.insert({ *request->number, route });
  }

  RouteStatsHolder Routes::GetRouteStats(const ReadRouteStatsRequest* request) const {
    if (auto it = routes.find(request->bus_number); it != routes.end()) {
      return it->second->GetRouteStats();
    }
    return nullptr;
  }

  StopRoutesHolder Routes::GetStopRoutes(const ReadStopRoutesRequest* request) const {
    if (auto it = stops.find(request->stop_name); it != stops.end()) {
      return it->second->routes;
    }
    return nullptr;
  }

  RouteStatsHolder Route::GetRouteStats() const {
    if (!routes_stats) {
      unordered_set<StopHolder> unique_stops;
      for (const auto stop : stops) {
        unique_stops.insert(stop);
      }

      double distance = 0;
      for (auto it1 = stops.begin(), it2 = next(it1);
          it2 != stops.end();
          it1 = next(it1), it2 = next(it2)) {
        distance += DistanceBetweenPositions((*it1)->position, (*it2)->position);
      }
      if (is_circular && !stops.empty()) {
        distance += DistanceBetweenPositions(stops.front()->position, stops.back()->position);
      }
      else {
        distance *= 2;
      }

      routes_stats = make_shared<RouteStats>(
        stops.size() + (is_circular ? 1 : (stops.size() - 1)),
        unique_stops.size(),
        distance
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

  ostream& operator <<(ostream& out_stream, const RouteStats& stats) {
    return out_stream << stats.stops_on_route << " stops on route, "
                      << stats.unique_stops << " unique stops, "
                      << stats.route_length << " route length";
  }
};

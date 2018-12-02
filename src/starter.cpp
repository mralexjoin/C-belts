#include <string>
#include <iostream>
#include <cassert>
#include <vector>
#include <map>

using namespace std;

using BusesToStops = map<string, vector<string>>;
using StopsToBuses = map<string, vector<string>>;

enum class QueryType {
  NewBus,
  BusesForStop,
  StopsForBus,
  AllBuses
};

struct Query {
  QueryType type;
  string bus;
  string stop;
  vector<string> stops;
};

istream& operator >> (istream& is, Query& q) {
  string operation;
  is >> operation;
  if (operation == "NEW_BUS") {
    q.type = QueryType::NewBus;
    is >> q.bus;
    int stop_count;
    is >> stop_count;
    q.stops.resize(stop_count);
    for (auto& stop : q.stops)
      is >> stop;
  }
  else if (operation == "BUSES_FOR_STOP") {
    q.type = QueryType::BusesForStop;
    is >> q.stop;
  }
  else if (operation == "STOPS_FOR_BUS") {
    q.type = QueryType::StopsForBus;
    is >> q.bus;
  }
  else if (operation == "ALL_BUSES") {
    q.type = QueryType::AllBuses;
  }

  return is;
}

struct BusesForStopResponse {
  vector<string> buses;
};

ostream& operator << (ostream& os, const BusesForStopResponse& r) {
  if (r.buses.empty()) {
    os << "No stop";
  }
  else {
    bool first = true;
    for (const auto& bus : r.buses) {
      if (!first)
        os << ' ';
      first = false;
      os << bus;
    }
  }
  return os;
}

struct StopsForBusResponse {
  vector<string> stops;
  StopsToBuses stops_to_buses;
};

ostream& operator << (ostream& os, const StopsForBusResponse& r) {
  if (r.stops_to_buses.empty()) {
    os << "No bus";
  }
  else {
    bool first = true;
    for (const auto& stop : r.stops) {
      if (!first)
        os << endl;
      first = false;
      os << "Stop " << stop << ":";
      const vector<string>& buses = r.stops_to_buses.at(stop);
      if (buses.size() == 0) {
        os << " no interchange";
      }
      else {
        for (const auto& bus : buses)
          os << ' ' << bus;
      }
    }
  }
  return os;
}

struct AllBusesResponse {
  BusesToStops buses_to_stops;
};

ostream& operator << (ostream& os, const AllBusesResponse& r) {
  if (r.buses_to_stops.empty()) {
    os << "No buses";
  }
  else {
    bool first = true;
    for (const auto& item : r.buses_to_stops) {
      if (!first)
        cout << endl;
      first = false;
      cout << "Bus " << item.first << ": ";
      for (const auto& stop : item.second) {
        cout << stop << ' ';
      }
    }
  }
  return os;
}

class BusManager {
public:
  void AddBus(const string& bus, const vector<string>& stops) {
    buses_to_stops[bus] = stops;
    for (auto& stop : stops)
      stops_to_buses[stop].push_back(bus);
  }

  BusesForStopResponse GetBusesForStop(const string& stop) const {
    if (stops_to_buses.count(stop) == 0)
      return {{}};
    return {stops_to_buses.at(stop)};
  }

  StopsForBusResponse GetStopsForBus(const string& bus) const {
    if (buses_to_stops.count(bus) == 0)
      return {{}};
    vector<string> stops = buses_to_stops.at(bus);
    StopsToBuses stops_for_bus;
    for (const auto& stop : stops) {
      vector<string>& buses = stops_for_bus[stop];
      for (const auto& other_bus : stops_to_buses.at(stop)) {
        if (other_bus != bus)
          buses.push_back(other_bus);
      }
    }
    return {stops, stops_for_bus};
  }

  AllBusesResponse GetAllBuses() const {
    return {buses_to_stops};
  }
private:
  BusesToStops buses_to_stops;
  StopsToBuses stops_to_buses;
};

// Не меняя тела функции main, реализуйте функции и классы выше

int main() {
  int query_count;
  Query q;

  cin >> query_count;

  BusManager bm;
  for (int i = 0; i < query_count; ++i) {
    cin >> q;
    switch (q.type) {
    case QueryType::NewBus:
      bm.AddBus(q.bus, q.stops);
      break;
    case QueryType::BusesForStop:
      cout << bm.GetBusesForStop(q.stop) << endl;
      break;
    case QueryType::StopsForBus:
      cout << bm.GetStopsForBus(q.bus) << endl;
      break;
    case QueryType::AllBuses:
      cout << bm.GetAllBuses() << endl;
      break;
    }
  }

  return 0;
}

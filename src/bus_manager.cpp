#include "bus_manager.h"

void BusManager::AddBus(const string& bus, const vector<string>& stops) {
  buses_to_stops[bus] = stops;
  for (auto& stop : stops)
    stops_to_buses[stop].push_back(bus);
}

BusesForStopResponse BusManager::GetBusesForStop(const string& stop) const {
  if (stops_to_buses.count(stop) == 0)
    return {{}};
  return {stops_to_buses.at(stop)};
}

StopsForBusResponse BusManager::GetStopsForBus(const string& bus) const {
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

AllBusesResponse BusManager::GetAllBuses() const {
  return {buses_to_stops};
}

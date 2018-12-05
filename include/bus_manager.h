#pragma once

#include <map>
#include <string>
#include <vector>

#include "responses.h"

using BusesToStops = map<string, vector<string>>;
using StopsToBuses = map<string, vector<string>>;

class BusManager {
 public:
  void AddBus(const string& bus, const vector<string>& stops);

  BusesForStopResponse GetBusesForStop(const string& stop) const;

  StopsForBusResponse GetStopsForBus(const string& bus) const;

  AllBusesResponse GetAllBuses() const;
 private:
  BusesToStops buses_to_stops;
  StopsToBuses stops_to_buses;
};

#pragma once

#include <map>
#include <ostream>
#include <string>
#include <vector>

using namespace std;

using BusesToStops = map<string, vector<string>>;
using StopsToBuses = map<string, vector<string>>;

struct BusesForStopResponse {
  vector<string> buses;
};

ostream& operator << (ostream& os, const BusesForStopResponse& r);

struct StopsForBusResponse {
  vector<string> stops;
  StopsToBuses stops_to_buses;
};

ostream& operator << (ostream& os, const StopsForBusResponse& r);

struct AllBusesResponse {
  BusesToStops buses_to_stops;
};

ostream& operator << (ostream& os, const AllBusesResponse& r);

#include "responses.h"

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

ostream& operator << (ostream& os, const AllBusesResponse& r) {
  if (r.buses_to_stops.empty()) {
    os << "No buses";
  }
  else {
    bool first = true;
    for (const auto& item : r.buses_to_stops) {
      if (!first)
        os << endl;
      first = false;
      os << "Bus " << item.first << ": ";
      for (const auto& stop : item.second) {
        os << stop << ' ';
      }
    }
  }
  return os;
}

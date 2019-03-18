#include "test_runner.h"
#include "routes.h"
#include "json.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace Json;

Routes::Routes CreateRoutes(Document& document) {
  Routes::Routes buses;
  auto& requests_by_type = document.GetRoot().AsMap();
  if (const auto& requests = requests_by_type.find("base_requests");
      requests != requests_by_type.end()) {
    for (const auto& request : Routes::ReadRequests<Routes::ModifyRequest>(requests->second)) {
      request->Execute(buses);
    }
  }
  return buses;
}

Document ReadRoutes(const Routes::Routes& buses, Document& document) {
  auto& requests_by_type = document.GetRoot().AsMap();
  vector<Node> requests_node;
  if (const auto& requests = requests_by_type.find("stat_requests");
      requests != requests_by_type.end()) {
    for (const auto& request : Routes::ReadRequests<Routes::ReadRequest>(requests->second)) {
      requests_node.push_back(request->Execute(buses)->ToJson());
    }
  }
  return Document{move(requests_node)};
}

void ProcessQueries(istream& input, ostream& output) {
  ostringstream buffer;
  for (string str; getline(input, str);) {
    cerr << str;
    buffer << str;
  }
  istringstream new_input(buffer.str());
  Document in_json = Load(new_input);
  Document out_json = ReadRoutes(CreateRoutes(in_json), in_json);
  Save(out_json, output);
}

void TestFromTask() {
  istringstream input(R"({"base_requests": [{"type": "Stop", "name": "Tolstopaltsevo", "latitude": 55.611087, "longitude": 37.20829, "road_distances": {"Marushkino": 3900}}, {"type": "Stop", "name": "Marushkino", "latitude": 55.595884, "longitude": 37.209755, "road_distances": {"Rasskazovka": 9900}}, {"type": "Bus", "name": "256", "stops": ["Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"], "is_roundtrip": true}, {"type": "Bus", "name": "750", "stops": ["Tolstopaltsevo", "Marushkino", "Rasskazovka"], "is_roundtrip": false}, {"type": "Stop", "name": "Rasskazovka", "latitude": 55.632761, "longitude": 37.333324, "road_distances": {}}, {"type": "Stop", "name": "Biryulyovo Zapadnoye", "latitude": 55.574371, "longitude": 37.6517, "road_distances": {"Biryusinka": 1800, "Universam": 2400, "Rossoshanskaya ulitsa": 7500}}, {"type": "Stop", "name": "Biryusinka", "latitude": 55.581065, "longitude": 37.64839, "road_distances": {"Universam": 750}}, {"type": "Stop", "name": "Universam", "latitude": 55.587655, "longitude": 37.645687, "road_distances": {"Biryulyovo Tovarnaya": 900, "Rossoshanskaya ulitsa": 5600}}, {"type": "Stop", "name": "Biryulyovo Tovarnaya", "latitude": 55.592028, "longitude": 37.653656, "road_distances": {"Biryulyovo Passazhirskaya": 1300}}, {"type": "Stop", "name": "Biryulyovo Passazhirskaya", "latitude": 55.580999, "longitude": 37.659164, "road_distances": {"Biryulyovo Zapadnoye": 1200}}, {"type": "Bus", "name": "828", "stops": ["Biryulyovo Zapadnoye", "Universam", "Rossoshanskaya ulitsa", "Biryulyovo Zapadnoye"], "is_roundtrip": true}, {"type": "Stop", "name": "Rossoshanskaya ulitsa", "latitude": 55.595579, "longitude": 37.605757, "road_distances": {}}, {"type": "Stop", "name": "Prazhskaya", "latitude": 55.611678, "longitude": 37.603831, "road_distances": {}}], "stat_requests": [{"id": 92297645, "type": "Bus", "name": "256"}, {"id": 1403850987, "type": "Bus", "name": "750"}, {"id": 197444743, "type": "Bus", "name": "751"}, {"id": 605625806, "type": "Stop", "name": "Samara"}, {"id": 401974628, "type": "Stop", "name": "Prazhskaya"}, {"id": 435078922, "type": "Stop", "name": "Biryulyovo Zapadnoye"}]})");
  ostringstream output;
  ProcessQueries(input, output);
  string expected = R"([
  {
    "curvature": 1.36124,
    "request_id": 1965312327,
    "route_length": 5950,
    "stop_count": 6,
    "unique_stop_count": 5
  },
  {
    "curvature": 1.31808,
    "request_id": 519139350,
    "route_length": 27600,
    "stop_count": 5,
    "unique_stop_count": 3
  },
  {
    "error_message": "not found",
    "request_id": 194217464
  },
  {
    "error_message": "not found",
    "request_id": 746888088
  },
  {
    "buses": [],
    "request_id": 65100610
  },
  {
    "buses": [
      "256",
      "828"
    ],
    "request_id": 1042838872
  }
])";
  ASSERT_EQUAL(output.str(), expected);
}

void TestSecond() {
  istringstream input(R"({"base_requests": [{"type": "Stop", "name": "A", "latitude": 0.5, "longitude": -1, "road_distances": {"B": 100000}}, {"type": "Stop", "name": "B", "latitude": 0, "longitude": -1.1, "road_distances": {}}, {"type": "Bus", "name": "256", "stops": ["B", "A"], "is_roundtrip": false}], "stat_requests": [{"id": 2143866354, "type": "Bus", "name": "256"}, {"id": 1759785269, "type": "Stop", "name": "A"}, {"id": 252557929, "type": "Stop", "name": "B"}, {"id": 1839286974, "type": "Stop", "name": "C"}]})");
  ProcessQueries(input, cout);
}

void RunTests() {
  TestRunner tr;
  RUN_TEST(tr, TestSecond);
}

int main() {
  RunTests();
  ProcessQueries(cin, cout);
  return 0;
}

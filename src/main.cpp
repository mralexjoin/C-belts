#include "test_runner.h"
#include "buses.h"
#include "requests.h"
//#include "profile.h"

//#include <conio.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace Json;

//TotalDuration dur_create_buses("Create buses");
//TotalDuration dur_full("Full duration");
//TotalDuration dur_build_router("Build router");
//TotalDuration dur_read_routes("Read routes");

BusesRouting::Buses CreateRoutes(Document& document) {
  //ADD_DURATION(dur_create_buses);
  BusesRouting::Buses buses;
  auto& requests_by_type = document.GetRoot().AsMap();
  if (const auto& settings_node = requests_by_type.find("routing_settings");
      settings_node != requests_by_type.end()) {
    const auto& settings = settings_node->second.AsMap();
    buses.SetBusWaitTime(settings.at("bus_wait_time").AsInt());
    buses.SetBusVelocity(settings.at("bus_velocity").AsDouble());
  }
  if (const auto& requests = requests_by_type.find("base_requests");
      requests != requests_by_type.end()) {
    for (const auto& request : BusesRouting::ReadRequests<BusesRouting::ModifyRequest>(requests->second)) {
      request->Execute(buses);
    }
  }
  //ADD_DURATION(dur_build_router);
  buses.BuildRouter();
  return buses;
}

Document ReadRoutes(const BusesRouting::Buses& buses, Document& document) {
  //ADD_DURATION(dur_read_routes);
  auto& requests_by_type = document.GetRoot().AsMap();
  vector<Node> requests_node;
  if (const auto& requests = requests_by_type.find("stat_requests");
      requests != requests_by_type.end()) {
    for (const auto& request : BusesRouting::ReadRequests<BusesRouting::ReadRequest>(requests->second)) {
      requests_node.emplace_back(request->Execute(buses)->ToJson());
    }
  }
  return Document{move(requests_node)};
}

void ProcessQueries(istream& input, ostream& output) {
  Document in_json = Load(input);
  BusesRouting::Buses routes = CreateRoutes(in_json);
  Save(ReadRoutes(routes, in_json), output);
}

void TestJson() {
  istringstream input(R"(
  {
    "base_requests":
      [
        {
          "type": "Stop",
          "name": "Tolstopaltsevo",
          "latitude": 55.611087,
          "longitude": 37.20829,
          "road_distances": {
            "Marushkino": 3900
          }
        }
      ]
  }
  )");
  Document in_json = Load(input);
  const auto& root = in_json.GetRoot().AsMap();
  const auto& stop = root.at("base_requests").AsArray()[0].AsMap();
  ASSERT_EQUAL(stop.at("type").AsString(), "Stop");
  ASSERT_EQUAL(stop.at("name").AsString(), "Tolstopaltsevo");

  double accuracy = 0.000001;
  ASSERT_NEAR(stop.at("latitude").AsDouble(), 55.611087, accuracy);
  ASSERT_NEAR(stop.at("longitude").AsDouble(), 37.20829, accuracy);
  ASSERT_EQUAL(stop.at("road_distances").AsMap().at("Marushkino").AsInt(), 3900);
}

void TestBusLeak() {
  istringstream input(R"(
  {
    "base_requests":
      [
        {
            "is_roundtrip": true,
            "stops": [
                "Biryulyovo Zapadnoye",
                "Biryusinka",
                "Universam",
                "Biryulyovo Tovarnaya",
                "Biryulyovo Passazhirskaya",
                "Biryulyovo Zapadnoye"
            ],
            "name": "256",
            "type": "Bus"
        }
      ]
  }
  )");
  Document in_json = Load(input);
  BusesRouting::Buses buses;
  auto& requests_by_type = in_json.GetRoot().AsMap();
  if (const auto& settings_node = requests_by_type.find("routing_settings");
      settings_node != requests_by_type.end()) {
    const auto& settings = settings_node->second.AsMap();
    buses.SetBusWaitTime(settings.at("bus_wait_time").AsInt());
    buses.SetBusVelocity(settings.at("bus_velocity").AsDouble());
  }
  if (const auto& requests = requests_by_type.find("base_requests");
      requests != requests_by_type.end()) {
    for (const auto& request : BusesRouting::ReadRequests<BusesRouting::ModifyRequest>(requests->second)) {
      request->Execute(buses);
    }
  }
}

void TestFromTask() {
  istringstream input(R"({"base_requests": [{"type": "Stop", "name": "Tolstopaltsevo", "latitude": 55.611087, "longitude": 37.20829, "road_distances": {"Marushkino": 3900}}, {"type": "Stop", "name": "Marushkino", "latitude": 55.595884, "longitude": 37.209755, "road_distances": {"Rasskazovka": 9900}}, {"type": "Bus", "name": "256", "stops": ["Biryulyovo Zapadnoye", "Biryusinka", "Universam", "Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya", "Biryulyovo Zapadnoye"], "is_roundtrip": true}, {"type": "Bus", "name": "750", "stops": ["Tolstopaltsevo", "Marushkino", "Rasskazovka"], "is_roundtrip": false}, {"type": "Stop", "name": "Rasskazovka", "latitude": 55.632761, "longitude": 37.333324, "road_distances": {}}, {"type": "Stop", "name": "Biryulyovo Zapadnoye", "latitude": 55.574371, "longitude": 37.6517, "road_distances": {"Biryusinka": 1800, "Universam": 2400, "Rossoshanskaya ulitsa": 7500}}, {"type": "Stop", "name": "Biryusinka", "latitude": 55.581065, "longitude": 37.64839, "road_distances": {"Universam": 750}}, {"type": "Stop", "name": "Universam", "latitude": 55.587655, "longitude": 37.645687, "road_distances": {"Biryulyovo Tovarnaya": 900, "Rossoshanskaya ulitsa": 5600}}, {"type": "Stop", "name": "Biryulyovo Tovarnaya", "latitude": 55.592028, "longitude": 37.653656, "road_distances": {"Biryulyovo Passazhirskaya": 1300}}, {"type": "Stop", "name": "Biryulyovo Passazhirskaya", "latitude": 55.580999, "longitude": 37.659164, "road_distances": {"Biryulyovo Zapadnoye": 1200}}, {"type": "Bus", "name": "828", "stops": ["Biryulyovo Zapadnoye", "Universam", "Rossoshanskaya ulitsa", "Biryulyovo Zapadnoye"], "is_roundtrip": true}, {"type": "Stop", "name": "Rossoshanskaya ulitsa", "latitude": 55.595579, "longitude": 37.605757, "road_distances": {}}, {"type": "Stop", "name": "Prazhskaya", "latitude": 55.611678, "longitude": 37.603831, "road_distances": {}}], "stat_requests": [{"id": 92297645, "type": "Bus", "name": "256"}, {"id": 1403850987, "type": "Bus", "name": "750"}, {"id": 197444743, "type": "Bus", "name": "751"}, {"id": 605625806, "type": "Stop", "name": "Samara"}, {"id": 401974628, "type": "Stop", "name": "Prazhskaya"}, {"id": 435078922, "type": "Stop", "name": "Biryulyovo Zapadnoye"}]})");

  BusesRouting::Buses buses;
  Document in_json = Load(input);
  auto& requests_by_type = in_json.GetRoot().AsMap();
  if (const auto& settings_node = requests_by_type.find("routing_settings");
      settings_node != requests_by_type.end()) {
    const auto& settings = settings_node->second.AsMap();
    buses.SetBusWaitTime(settings.at("bus_wait_time").AsInt());
    buses.SetBusVelocity(settings.at("bus_velocity").AsDouble());
  }
  if (const auto& requests = requests_by_type.find("base_requests");
      requests != requests_by_type.end()) {
    for (const auto& request : BusesRouting::ReadRequests<BusesRouting::ModifyRequest>(requests->second)) {
      request->Execute(buses);
    }
  }

  ostringstream output;

  Save(ReadRoutes(buses, in_json), output);

  string expected = R"([
  {
    "curvature": 1.36124,
    "request_id": 92297645,
    "route_length": 5950,
    "stop_count": 6,
    "unique_stop_count": 5
  },
  {
    "curvature": 1.31808,
    "request_id": 1403850987,
    "route_length": 27600,
    "stop_count": 5,
    "unique_stop_count": 3
  },
  {
    "error_message": "not found",
    "request_id": 197444743
  },
  {
    "error_message": "not found",
    "request_id": 605625806
  },
  {
    "buses": [],
    "request_id": 401974628
  },
  {
    "buses": [
      "256",
      "828"
    ],
    "request_id": 435078922
  }
])";
  ASSERT_EQUAL(output.str(), expected);
}

void TestSecondSimple() {
  istringstream input(R"({"base_requests": [{"type": "Stop", "name": "A", "latitude": 0.5, "longitude": -1, "road_distances": {"B": 100000}}, {"type": "Stop", "name": "B", "latitude": 0, "longitude": -1.1, "road_distances": {}}, {"type": "Bus", "name": "256", "stops": ["B", "A"], "is_roundtrip": false}], "stat_requests": [{"id": 2143866354, "type": "Bus", "name": "256"}, {"id": 1759785269, "type": "Stop", "name": "A"}, {"id": 252557929, "type": "Stop", "name": "B"}, {"id": 1839286974, "type": "Stop", "name": "C"}]})");
  ostringstream output;
  ProcessQueries(input, output);
  string expected = R"([
  {
    "curvature": 1.76372,
    "request_id": 2143866354,
    "route_length": 200000,
    "stop_count": 3,
    "unique_stop_count": 2
  },
  {
    "buses": [
      "256"
    ],
    "request_id": 1759785269
  },
  {
    "buses": [
      "256"
    ],
    "request_id": 252557929
  },
  {
    "error_message": "not found",
    "request_id": 1839286974
  }
])";
  ASSERT_EQUAL(output.str(), expected);
}

void TestFirst() {
  istringstream input(R"(
{
  "routing_settings": {
    "bus_wait_time": 6,
    "bus_velocity": 40
  },
  "base_requests": [
    {
      "type": "Bus",
      "name": "297",
      "stops": [
        "Biryulyovo Zapadnoye",
        "Biryulyovo Tovarnaya",
        "Universam",
        "Biryulyovo Zapadnoye"
      ],
      "is_roundtrip": true
    },
    {
      "type": "Bus",
      "name": "635",
      "stops": [
        "Biryulyovo Tovarnaya",
        "Universam",
        "Prazhskaya"
      ],
      "is_roundtrip": false
    },
    {
      "type": "Stop",
      "road_distances": {
        "Biryulyovo Tovarnaya": 2600
      },
      "longitude": 37.6517,
      "name": "Biryulyovo Zapadnoye",
      "latitude": 55.574371
    },
    {
      "type": "Stop",
      "road_distances": {
        "Prazhskaya": 4650,
        "Biryulyovo Tovarnaya": 1380,
        "Biryulyovo Zapadnoye": 2500
      },
      "longitude": 37.645687,
      "name": "Universam",
      "latitude": 55.587655
    },
    {
      "type": "Stop",
      "road_distances": {
        "Universam": 890
      },
      "longitude": 37.653656,
      "name": "Biryulyovo Tovarnaya",
      "latitude": 55.592028
    },
    {
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.603938,
      "name": "Prazhskaya",
      "latitude": 55.611717
    }
  ],
  "stat_requests": [
    {
      "type": "Bus",
      "name": "297",
      "id": 1
    },
    {
      "type": "Bus",
      "name": "635",
      "id": 2
    },
    {
      "type": "Stop",
      "name": "Universam",
      "id": 3
    },
    {
      "type": "Route",
      "from": "Biryulyovo Zapadnoye",
      "to": "Universam",
      "id": 4
    },
    {
      "type": "Route",
      "from": "Biryulyovo Zapadnoye",
      "to": "Prazhskaya",
      "id": 5
    }
  ]
}
)");

  ostringstream output;
  ProcessQueries(input, output);
  string expected = R"([
  {
    "curvature": 1.42963,
    "request_id": 1,
    "route_length": 5990,
    "stop_count": 4,
    "unique_stop_count": 3
  },
  {
    "curvature": 1.30156,
    "request_id": 2,
    "route_length": 11570,
    "stop_count": 5,
    "unique_stop_count": 3
  },
  {
    "buses": [
      "297",
      "635"
    ],
    "request_id": 3
  },
  {
    "items": [
      {
        "stop_name": "Biryulyovo Zapadnoye",
        "time": 6,
        "type": "Wait"
      },
      {
        "bus": "297",
        "span_count": 2,
        "time": 5.235,
        "type": "Bus"
      }
    ],
    "request_id": 4,
    "total_time": 11.235
  },
  {
    "items": [
      {
        "stop_name": "Biryulyovo Zapadnoye",
        "time": 6,
        "type": "Wait"
      },
      {
        "bus": "297",
        "span_count": 1,
        "time": 3.9,
        "type": "Bus"
      },
      {
        "stop_name": "Biryulyovo Tovarnaya",
        "time": 6,
        "type": "Wait"
      },
      {
        "bus": "635",
        "span_count": 2,
        "time": 8.31,
        "type": "Bus"
      }
    ],
    "request_id": 5,
    "total_time": 24.21
  }
])";
  ASSERT_EQUAL(output.str(), expected);
}

void TestSimpleRoute() {
  istringstream input(R"(
{
    "routing_settings": {
        "bus_wait_time": 1,
        "bus_velocity": 0.060
    },
    "base_requests": [
        {
            "type": "Stop",
            "name": "Luhmanovskaya",
            "latitude": 55.574371,
            "longitude": 37.6517,
            "road_distances": {
                "Ulitsa Dmitrievskogo": 2
            }
        },
        {
            "type": "Stop",
            "name": "Ulitsa Dmitrievskogo",
            "latitude": 55.574371,
            "longitude": 37.6517
        },
        {
            "type": "Bus",
            "name": "256",
            "stops": [
                "Luhmanovskaya",
                "Ulitsa Dmitrievskogo"
            ],
            "is_roundtrip": false
        }
    ],
    "stat_requests": [
        {
            "type": "Route",
            "from": "Luhmanovskaya",
            "to": "Ulitsa Dmitrievskogo",
            "id": 1
        }
    ]
}

)");

  ostringstream output;
  ProcessQueries(input, output);
  string expected = R"([
  {
    "items": [
      {
        "stop_name": "Luhmanovskaya",
        "time": 1,
        "type": "Wait"
      },
      {
        "bus": "256",
        "span_count": 1,
        "time": 2,
        "type": "Bus"
      }
    ],
    "request_id": 1,
    "total_time": 3
  }
])";
  ASSERT_EQUAL(output.str(), expected);
}

void TestSecond() {
  istringstream input(R"({
  "routing_settings": {
    "bus_wait_time": 2,
    "bus_velocity": 30
  },
  "base_requests": [
    {
      "type": "Bus",
      "name": "297",
      "stops": [
        "Biryulyovo Zapadnoye",
        "Biryulyovo Tovarnaya",
        "Universam",
        "Biryusinka",
        "Apteka",
        "Biryulyovo Zapadnoye"
      ],
      "is_roundtrip": true
    },
    {
      "type": "Bus",
      "name": "635",
      "stops": [
        "Biryulyovo Tovarnaya",
        "Universam",
        "Biryusinka",
        "TETs 26",
        "Pokrovskaya",
        "Prazhskaya"
      ],
      "is_roundtrip": false
    },
    {
      "type": "Bus",
      "name": "828",
      "stops": [
        "Biryulyovo Zapadnoye",
        "TETs 26",
        "Biryusinka",
        "Universam",
        "Pokrovskaya",
        "Rossoshanskaya ulitsa"
      ],
      "is_roundtrip": false
    },
    {
      "type": "Stop",
      "road_distances": {
        "Biryulyovo Tovarnaya": 2600,
        "TETs 26": 1100
      },
      "longitude": 37.6517,
      "name": "Biryulyovo Zapadnoye",
      "latitude": 55.574371
    },
    {
      "type": "Stop",
      "road_distances": {
        "Biryusinka": 760,
        "Biryulyovo Tovarnaya": 1380,
        "Pokrovskaya": 2460
      },
      "longitude": 37.645687,
      "name": "Universam",
      "latitude": 55.587655
    },
    {
      "type": "Stop",
      "road_distances": {
        "Universam": 890
      },
      "longitude": 37.653656,
      "name": "Biryulyovo Tovarnaya",
      "latitude": 55.592028
    },
    {
      "type": "Stop",
      "road_distances": {
        "Apteka": 210,
        "TETs 26": 400
      },
      "longitude": 37.64839,
      "name": "Biryusinka",
      "latitude": 55.581065
    },
    {
      "type": "Stop",
      "road_distances": {
        "Biryulyovo Zapadnoye": 1420
      },
      "longitude": 37.652296,
      "name": "Apteka",
      "latitude": 55.580023
    },
    {
      "type": "Stop",
      "road_distances": {
        "Pokrovskaya": 2850
      },
      "longitude": 37.642258,
      "name": "TETs 26",
      "latitude": 55.580685
    },
    {
      "type": "Stop",
      "road_distances": {
        "Rossoshanskaya ulitsa": 3140
      },
      "longitude": 37.635517,
      "name": "Pokrovskaya",
      "latitude": 55.603601
    },
    {
      "type": "Stop",
      "road_distances": {
        "Pokrovskaya": 3210
      },
      "longitude": 37.605757,
      "name": "Rossoshanskaya ulitsa",
      "latitude": 55.595579
    },
    {
      "type": "Stop",
      "road_distances": {
        "Pokrovskaya": 2260
      },
      "longitude": 37.603938,
      "name": "Prazhskaya",
      "latitude": 55.611717
    },
    {
      "type": "Bus",
      "name": "750",
      "stops": [
        "Tolstopaltsevo",
        "Rasskazovka"
      ],
      "is_roundtrip": false
    },
    {
      "type": "Stop",
      "road_distances": {
        "Rasskazovka": 13800
      },
      "longitude": 37.20829,
      "name": "Tolstopaltsevo",
      "latitude": 55.611087
    },
    {
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.333324,
      "name": "Rasskazovka",
      "latitude": 55.632761
    }
  ],
  "stat_requests": [
    {
      "type": "Bus",
      "name": "297",
      "id": 1
    },
    {
      "type": "Bus",
      "name": "635",
      "id": 2
    },
    {
      "type": "Bus",
      "name": "828",
      "id": 3
    },
    {
      "type": "Stop",
      "name": "Universam",
      "id": 4
    },
    {
      "type": "Route",
      "from": "Biryulyovo Zapadnoye",
      "to": "Apteka",
      "id": 5
    },
    {
      "type": "Route",
      "from": "Biryulyovo Zapadnoye",
      "to": "Pokrovskaya",
      "id": 6
    },
    {
      "type": "Route",
      "from": "Biryulyovo Tovarnaya",
      "to": "Pokrovskaya",
      "id": 7
    },
    {
      "type": "Route",
      "from": "Biryulyovo Tovarnaya",
      "to": "Biryulyovo Zapadnoye",
      "id": 8
    },
    {
      "type": "Route",
      "from": "Biryulyovo Tovarnaya",
      "to": "Prazhskaya",
      "id": 9
    },
    {
      "type": "Route",
      "from": "Apteka",
      "to": "Biryulyovo Tovarnaya",
      "id": 10
    },
    {
      "type": "Route",
      "from": "Biryulyovo Zapadnoye",
      "to": "Tolstopaltsevo",
      "id": 11
    }
  ]
})");

  string expected = R"([
    {
        "stop_count": 6,
        "route_length": 5880,
        "unique_stop_count": 5,
        "curvature": 1.36159,
        "request_id": 1
    },
    {
        "stop_count": 11,
        "route_length": 14810,
        "unique_stop_count": 6,
        "curvature": 1.12195,
        "request_id": 2
    },
    {
        "stop_count": 11,
        "route_length": 15790,
        "unique_stop_count": 6,
        "curvature": 1.31245,
        "request_id": 3
    },
    {
        "buses": [
            "297",
            "635",
            "828"
        ],
        "request_id": 4
    },
    {
        "items": [
            {
                "time": 2,
                "stop_name": "Biryulyovo Zapadnoye",
                "type": "Wait"
            },
            {
                "time": 3,
                "bus": "828",
                "span_count": 2,
                "type": "Bus"
            },
            {
                "time": 2,
                "stop_name": "Biryusinka",
                "type": "Wait"
            },
            {
                "time": 0.42,
                "bus": "297",
                "span_count": 1,
                "type": "Bus"
            }
        ],
        "request_id": 5,
        "total_time": 7.42
    },
    {
        "items": [
            {
                "time": 2,
                "stop_name": "Biryulyovo Zapadnoye",
                "type": "Wait"
            },
            {
                "time": 9.44,
                "bus": "828",
                "span_count": 4,
                "type": "Bus"
            }
        ],
        "request_id": 6,
        "total_time": 11.44
    },
    {
        "items": [
            {
                "time": 2,
                "stop_name": "Biryulyovo Tovarnaya",
                "type": "Wait"
            },
            {
                "time": 1.78,
                "bus": "635",
                "span_count": 1,
                "type": "Bus"
            },
            {
                "time": 2,
                "stop_name": "Universam",
                "type": "Wait"
            },
            {
                "time": 4.92,
                "bus": "828",
                "span_count": 1,
                "type": "Bus"
            }
        ],
        "request_id": 7,
        "total_time": 10.7
    },
    {
        "items": [
            {
                "time": 2,
                "stop_name": "Biryulyovo Tovarnaya",
                "type": "Wait"
            },
            {
                "time": 6.56,
                "bus": "297",
                "span_count": 4,
                "type": "Bus"
            }
        ],
        "request_id": 8,
        "total_time": 8.56
    },
    {
        "items": [
            {
                "time": 2,
                "stop_name": "Biryulyovo Tovarnaya",
                "type": "Wait"
            },
            {
                "time": 14.32,
                "bus": "635",
                "span_count": 5,
                "type": "Bus"
            }
        ],
        "request_id": 9,
        "total_time": 16.32
    },
    {
        "items": [
            {
                "time": 2,
                "stop_name": "Apteka",
                "type": "Wait"
            },
            {
                "time": 2.84,
                "bus": "297",
                "span_count": 1,
                "type": "Bus"
            },
            {
                "time": 2,
                "stop_name": "Biryulyovo Zapadnoye",
                "type": "Wait"
            },
            {
                "time": 5.2,
                "bus": "297",
                "span_count": 1,
                "type": "Bus"
            }
        ],
        "request_id": 10,
        "total_time": 12.04
    },
    {
        "error_message": "not found",
        "request_id": 11
    }
])";

  ostringstream output;
  ProcessQueries(input, output);

}

void TestThird() {
  istringstream input(R"({
  "routing_settings": {
    "bus_wait_time": 2,
    "bus_velocity": 30
  },
  "base_requests": [
    {
      "type": "Bus",
      "name": "289",
      "stops": [
        "Zagorye",
        "Lipetskaya ulitsa 46",
        "Lipetskaya ulitsa 40",
        "Lipetskaya ulitsa 40",
        "Lipetskaya ulitsa 46",
        "Moskvorechye",
        "Zagorye"
      ],
      "is_roundtrip": true
    },
    {
      "type": "Stop",
      "road_distances": {
        "Lipetskaya ulitsa 46": 230
      },
      "longitude": 37.68372,
      "name": "Zagorye",
      "latitude": 55.579909
    },
    {
      "type": "Stop",
      "road_distances": {
        "Lipetskaya ulitsa 40": 390,
        "Moskvorechye": 12400
      },
      "longitude": 37.682205,
      "name": "Lipetskaya ulitsa 46",
      "latitude": 55.581441
    },
    {
      "type": "Stop",
      "road_distances": {
        "Lipetskaya ulitsa 40": 1090,
        "Lipetskaya ulitsa 46": 380
      },
      "longitude": 37.679133,
      "name": "Lipetskaya ulitsa 40",
      "latitude": 55.584496
    },
    {
      "type": "Stop",
      "road_distances": {
        "Zagorye": 10000
      },
      "longitude": 37.638433,
      "name": "Moskvorechye",
      "latitude": 55.638433
    }
  ],
  "stat_requests": [
    {
      "type": "Bus",
      "name": "289",
      "id": 1
    },
    {
      "type": "Route",
      "from": "Zagorye",
      "to": "Moskvorechye",
      "id": 2
    },
    {
      "type": "Route",
      "from": "Moskvorechye",
      "to": "Zagorye",
      "id": 3
    },
    {
      "type": "Route",
      "from": "Lipetskaya ulitsa 40",
      "to": "Lipetskaya ulitsa 40",
      "id": 4
    }
  ]
})");
  ostringstream output;
  ProcessQueries(input, output);
}

void RunTests() {
  TestRunner tr;
  RUN_TEST(tr, TestJson);
  RUN_TEST(tr, TestBusLeak);
  RUN_TEST(tr, TestFromTask);
  RUN_TEST(tr, TestSecondSimple);
  RUN_TEST(tr, TestSimpleRoute);
  RUN_TEST(tr, TestFirst);
  RUN_TEST(tr, TestSecond);
  RUN_TEST(tr, TestThird);
}

int main() {
  //ADD_DURATION(dur_full);
  RunTests();
  ProcessQueries(cin, cout);
  return 0;
}

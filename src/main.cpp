#include "descriptions.h"
#include "json.h"
#include "requests.h"
#include "sphere.h"
#include "transport_catalog.h"
#include "utils.h"

#include "test_runner.h"

#include <iostream>
#include <sstream>

using namespace std;

void ProcessQueries(std::istream& in, std::ostream& out) {
  const auto input_doc = Json::Load(in);
  const auto& input_map = input_doc.GetRoot().AsMap();

  const TransportCatalog db(
      Descriptions::ReadDescriptions(input_map.at("base_requests").AsArray()),
      input_map.at("routing_settings").AsMap(),
      input_map.at("render_settings").AsMap()
  );

  Json::PrintValue(
      Requests::ProcessAll(db, input_map.at("stat_requests").AsArray()),
      out
  );
  out << endl;
}

void TestFirst() {
  string str_input = R"({
    "routing_settings": {
        "bus_wait_time": 2,
        "bus_velocity": 30
    },
    "render_settings": {
        "width": 1200,
        "height": 1200,
        "padding": 50,
        "stop_radius": 5,
        "line_width": 14,
        "stop_label_font_size": 20,
        "stop_label_offset": [
            7,
            -3
        ],
        "underlayer_color": [
            255,
            255,
            255,
            0.85
        ],
        "underlayer_width": 3,
        "color_palette": [
            "green",
            [
                255,
                160,
                0
            ],
            "red"
        ]
    },
    "base_requests": [
        {
            "type": "Bus",
            "name": "14",
            "stops": [
                "Улица Лизы Чайкиной",
                "Электросети",
                "Ривьерский мост",
                "Гостиница Сочи",
                "Кубанская улица",
                "По требованию",
                "Улица Докучаева",
                "Улица Лизы Чайкиной"
            ],
            "is_roundtrip": true
        },
        {
            "type": "Bus",
            "name": "24",
            "stops": [
                "Улица Докучаева",
                "Параллельная улица",
                "Электросети",
                "Санаторий Родина"
            ],
            "is_roundtrip": false
        },
        {
            "type": "Bus",
            "name": "114",
            "stops": [
                "Морской вокзал",
                "Ривьерский мост"
            ],
            "is_roundtrip": false
        },
        {
            "type": "Stop",
            "name": "Улица Лизы Чайкиной",
            "latitude": 43.590317,
            "longitude": 39.746833,
            "road_distances": {
                "Электросети": 4300,
                "Улица Докучаева": 2000
            }
        },
        {
            "type": "Stop",
            "name": "Морской вокзал",
            "latitude": 43.581969,
            "longitude": 39.719848,
            "road_distances": {
                "Ривьерский мост": 850
            }
        },
        {
            "type": "Stop",
            "name": "Электросети",
            "latitude": 43.598701,
            "longitude": 39.730623,
            "road_distances": {
                "Санаторий Родина": 4500,
                "Параллельная улица": 1200,
                "Ривьерский мост": 1900
            }
        },
        {
            "type": "Stop",
            "name": "Ривьерский мост",
            "latitude": 43.587795,
            "longitude": 39.716901,
            "road_distances": {
                "Морской вокзал": 850,
                "Гостиница Сочи": 1740
            }
        },
        {
            "type": "Stop",
            "name": "Гостиница Сочи",
            "latitude": 43.578079,
            "longitude": 39.728068,
            "road_distances": {
                "Кубанская улица": 320
            }
        },
        {
            "type": "Stop",
            "name": "Кубанская улица",
            "latitude": 43.578509,
            "longitude": 39.730959,
            "road_distances": {
                "По требованию": 370
            }
        },
        {
            "type": "Stop",
            "name": "По требованию",
            "latitude": 43.579285,
            "longitude": 39.733742,
            "road_distances": {
                "Улица Докучаева": 600
            }
        },
        {
            "type": "Stop",
            "name": "Улица Докучаева",
            "latitude": 43.585586,
            "longitude": 39.733879,
            "road_distances": {
                "Параллельная улица": 1100
            }
        },
        {
            "type": "Stop",
            "name": "Параллельная улица",
            "latitude": 43.590041,
            "longitude": 39.732886,
            "road_distances": {}
        },
        {
            "type": "Stop",
            "name": "Санаторий Родина",
            "latitude": 43.601202,
            "longitude": 39.715498,
            "road_distances": {}
        }
    ],
    "stat_requests": [
        {
            "id": 1218663236,
            "type": "Map"
        }
    ]
}
)";
  istringstream input(str_input);
  ostringstream output;
  ProcessQueries(input, output);

  string expected = R"([{"map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?><svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\"><polyline points=\"202.7046433700865,725.1651507897237 99.25163555140999,520.6462422211027 202.7046433700865,725.1651507897237\" fill=\"none\" stroke=\"green\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\" /><polyline points=\"1150.0,432.1126535822507 580.955800223462,137.79639380890507 99.25163555140999,520.6462422211027 491.26376256587264,861.7217169297415 592.7509175043582,846.6267751715245 690.4467847454753,819.3856709748779 695.2561033988173,598.1921174405119 1150.0,432.1126535822507\" fill=\"none\" stroke=\"rgb(255,160,0)\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\" /><polyline points=\"695.2561033988173,598.1921174405119 660.3973192915555,441.8014999201921 580.955800223462,137.79639380890507 50.0,50.0 580.955800223462,137.79639380890507 660.3973192915555,441.8014999201921 695.2561033988173,598.1921174405119\" fill=\"none\" stroke=\"red\" stroke-width=\"14\" stroke-linecap=\"round\" stroke-linejoin=\"round\" /><circle cx=\"491.26376256587264\" cy=\"861.7217169297415\" r=\"5\" fill=\"white\" stroke=\"none\" stroke-width=\"1\" /><circle cx=\"592.7509175043582\" cy=\"846.6267751715245\" r=\"5\" fill=\"white\" stroke=\"none\" stroke-width=\"1\" /><circle cx=\"202.7046433700865\" cy=\"725.1651507897237\" r=\"5\" fill=\"white\" stroke=\"none\" stroke-width=\"1\" /><circle cx=\"660.3973192915555\" cy=\"441.8014999201921\" r=\"5\" fill=\"white\" stroke=\"none\" stroke-width=\"1\" /><circle cx=\"690.4467847454753\" cy=\"819.3856709748779\" r=\"5\" fill=\"white\" stroke=\"none\" stroke-width=\"1\" /><circle cx=\"99.25163555140999\" cy=\"520.6462422211027\" r=\"5\" fill=\"white\" stroke=\"none\" stroke-width=\"1\" /><circle cx=\"50.0\" cy=\"50.0\" r=\"5\" fill=\"white\" stroke=\"none\" stroke-width=\"1\" /><circle cx=\"695.2561033988173\" cy=\"598.1921174405119\" r=\"5\" fill=\"white\" stroke=\"none\" stroke-width=\"1\" /><circle cx=\"1150.0\" cy=\"432.1126535822507\" r=\"5\" fill=\"white\" stroke=\"none\" stroke-width=\"1\" /><circle cx=\"580.955800223462\" cy=\"137.79639380890507\" r=\"5\" fill=\"white\" stroke=\"none\" stroke-width=\"1\" /><text x=\"491.26376256587264\" y=\"861.7217169297415\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" >Гостиница Сочи</text><text x=\"491.26376256587264\" y=\"861.7217169297415\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\" stroke=\"none\" stroke-width=\"1\" >Гостиница Сочи</text><text x=\"592.7509175043582\" y=\"846.6267751715245\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" >Кубанская улица</text><text x=\"592.7509175043582\" y=\"846.6267751715245\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\" stroke=\"none\" stroke-width=\"1\" >Кубанская улица</text><text x=\"202.7046433700865\" y=\"725.1651507897237\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" >Морской вокзал</text><text x=\"202.7046433700865\" y=\"725.1651507897237\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\" stroke=\"none\" stroke-width=\"1\" >Морской вокзал</text><text x=\"660.3973192915555\" y=\"441.8014999201921\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" >Параллельная улица</text><text x=\"660.3973192915555\" y=\"441.8014999201921\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\" stroke=\"none\" stroke-width=\"1\" >Параллельная улица</text><text x=\"690.4467847454753\" y=\"819.3856709748779\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" >По требованию</text><text x=\"690.4467847454753\" y=\"819.3856709748779\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\" stroke=\"none\" stroke-width=\"1\" >По требованию</text><text x=\"99.25163555140999\" y=\"520.6462422211027\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" >Ривьерский мост</text><text x=\"99.25163555140999\" y=\"520.6462422211027\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\" stroke=\"none\" stroke-width=\"1\" >Ривьерский мост</text><text x=\"50.0\" y=\"50.0\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" >Санаторий Родина</text><text x=\"50.0\" y=\"50.0\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\" stroke=\"none\" stroke-width=\"1\" >Санаторий Родина</text><text x=\"695.2561033988173\" y=\"598.1921174405119\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" >Улица Докучаева</text><text x=\"695.2561033988173\" y=\"598.1921174405119\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\" stroke=\"none\" stroke-width=\"1\" >Улица Докучаева</text><text x=\"1150.0\" y=\"432.1126535822507\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" >Улица Лизы Чайкиной</text><text x=\"1150.0\" y=\"432.1126535822507\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\" stroke=\"none\" stroke-width=\"1\" >Улица Лизы Чайкиной</text><text x=\"580.955800223462\" y=\"137.79639380890507\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"rgba(255,255,255,0.85)\" stroke=\"rgba(255,255,255,0.85)\" stroke-width=\"3\" stroke-linecap=\"round\" stroke-linejoin=\"round\" >Электросети</text><text x=\"580.955800223462\" y=\"137.79639380890507\" dx=\"7\" dy=\"-3\" font-size=\"20\" font-family=\"Verdana\" fill=\"black\" stroke=\"none\" stroke-width=\"1\" >Электросети</text></svg>", "request_id": 1218663236}])";

  ASSERT_EQUAL(output.str(), expected);
}

void RunTests() {
  TestRunner tr;
  RUN_TEST(tr, TestFirst);
}

int main() {
  RunTests();
  ProcessQueries(cin, cout);
  return 0;
}

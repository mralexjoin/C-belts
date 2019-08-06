#include "descriptions.h"
#include "json.h"
#include "requests.h"
#include "sphere.h"
#include "transport_catalog.h"
#include "utils.h"

#include "test_runner.h"

#include <iostream>

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

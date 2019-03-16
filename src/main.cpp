#include "test_runner.h"
#include "routes.h"

#include <iostream>
#include <sstream>

using namespace std;

Routes::Routes CreateRoutes(istream& in_stream = cin) {
  Routes::Routes routes;
  const auto requests = Routes::ReadRequests<Routes::ModifyRequest>(in_stream);
  for (const auto& request : requests) {
    request->Execute(routes);
  }
  return routes;
}

void ReadRoutes(const Routes::Routes& routes,
                istream& in_stream = cin,
                ostream& out_stream = cout) {
  for (const auto& request : Routes::ReadRequests<Routes::ReadRequest>(in_stream)) {
    out_stream << *request->Execute(routes) << '\n';
  }
}

void TestTrim() {
  using namespace Routes;
  ASSERT_EQUAL(TrimLeft("  Stop"), "Stop");
  ASSERT_EQUAL(TrimLeft("Stop"), "Stop");
  ASSERT_EQUAL(TrimLeft("    "), "");

  ASSERT_EQUAL(TrimRight("Stop   "), "Stop");
  ASSERT_EQUAL(TrimRight("Stop"), "Stop");
  ASSERT_EQUAL(TrimLeft("    "), "");
}

void TestInput() {
  istringstream input("1\n    Stop    Tolstopaltsevo    :   55.611087,   37.20829   ");
  const auto requests = Routes::ReadRequests<Routes::ModifyRequest>(input);
  ASSERT_EQUAL(requests.size(), 1u);
}

void TestFromTask() {
  istringstream input(
R"(10
Stop Tolstopaltsevo: 55.611087, 37.20829
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517
Stop Biryusinka: 55.581065, 37.64839
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
3
Bus 256
Bus 750
Bus 751)"
  );
  ostringstream output;
  ReadRoutes(CreateRoutes(input), input, output);
ASSERT_EQUAL(output.str(), 
R"(Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length
Bus 751: not found
)"
);
}

void TestComputeDistance() {
  using namespace Routes;
  Position p1 = {56, 38}, p2 = {55, 37};
  ASSERT_EQUAL(DistanceBetweenPositions(p1, p2), 127789.739050);
  ASSERT_EQUAL(DistanceBetweenPositions(p2, p1), 127789.739050);
}

void RunTests() {
  TestRunner tr;
  RUN_TEST(tr, TestTrim);
  RUN_TEST(tr, TestInput);
  RUN_TEST(tr, TestFromTask);
  RUN_TEST(tr, TestComputeDistance);
}

int main() {
  cout.precision(6);
  cerr.precision(6);
  RunTests();
  //ReadRoutes(CreateRoutes());
  return 0;
}

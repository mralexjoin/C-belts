#include "new_trip_manager.h"

#include "test_runner.h"

#include <stdexcept>

using namespace std;

int FlightProvider::capacity = 0;
int FlightProvider::counter = 0;

int HotelProvider::capacity = 0;
int HotelProvider::counter = 0;


void MyTests() {
  FlightProvider::capacity = 2;
  HotelProvider::capacity = 1;
  FlightProvider::counter = 0;
  HotelProvider::counter = 0;
  TripManager tm;
  auto trip = tm.Book({});
  ASSERT_EQUAL(FlightProvider::counter, 0);
  ASSERT_EQUAL(HotelProvider::counter, 0);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, MyTests);
  return 0;
}

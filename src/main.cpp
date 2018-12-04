#include "phone_number.h"
#include "test_runner.h"

void TestPhoneNumber() {
  {
    PhoneNumber p("+7-495-111-22-33");
    AssertEqual(p.GetCountryCode(), "7", "Country code = 7");
    AssertEqual(p.GetCityCode(), "495", "City code = 495");
    AssertEqual(p.GetLocalNumber(), "111-22-33", "Local number = 111-22-33");
    AssertEqual(p.GetInternationalNumber(), "+7-495-111-22-33", "International number = +7-495-111-22-33");
  }

  {
    PhoneNumber p("+7-495-1112233");
    AssertEqual(p.GetCountryCode(), "7", "Country code = 7");
    AssertEqual(p.GetCityCode(), "495", "City code = 495");
    AssertEqual(p.GetLocalNumber(), "1112233", "Local number = 1112233");
    AssertEqual(p.GetInternationalNumber(), "+7-495-1112233", "International number = +7-495-1112233");
  }

}

int main() {
  TestRunner tr;
  tr.RunTest(TestPhoneNumber, "TestPhoneNumber");
}

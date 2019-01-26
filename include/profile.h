#pragma once

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

class LogDuration {
  using steady_clock = std::chrono::steady_clock;
  using string = std::string;

 public:
  explicit LogDuration(const string& msg = "")
      : message(msg + ": "), start(steady_clock::now()) {}

  ~LogDuration() {
    using namespace std;
    using namespace chrono;
    auto finish = steady_clock::now();
    auto dur = finish - start;
    cerr << message << duration_cast<milliseconds>(dur).count() << " ms"
         << std::endl;
  }

 private:
  string message;
  steady_clock::time_point start;
};

struct TotalDuration {
  using steady_clock = std::chrono::steady_clock;
  using string = std::string;
  string message;
  steady_clock::duration value;
  explicit TotalDuration(const string& msg = "")
      : message(msg + ": "), value(0) {}
  ~TotalDuration() {
    using namespace std;
    using namespace chrono;
    ostringstream os;
    os << message << duration_cast<milliseconds>(value).count() << " ms"
       << endl;
    cerr << os.str();
  }
};

class AddDuration {
  using steady_clock = std::chrono::steady_clock;
  using string = std::string;

 public:
  explicit AddDuration(steady_clock::duration& dest)
      : add_to(dest), start(steady_clock::now()) {}
  explicit AddDuration(TotalDuration& dest) : AddDuration(dest.value) {}
  ~AddDuration() { add_to += steady_clock::now() - start; }

 private:
  steady_clock::duration& add_to;
  steady_clock::time_point start;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message) LogDuration UNIQ_ID(__LINE__){message};

#define ADD_DURATION(value) AddDuration UNIQ_ID(__LINE__){value};

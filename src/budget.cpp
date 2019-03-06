#include "test_runner.h"

#include <cmath>
#include <cstdint>
#include <ctime>
#include <exception>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>
#include <type_traits>
#include <unordered_map>
#include <vector>

using namespace std;

template<typename It>
class Range {
public:
  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const { return begin_; }
  It end() const { return end_; }

private:
  It begin_;
  It end_;
};

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter = " ") {
  const size_t pos = s.find(delimiter);
  if (pos == s.npos) {
    return {s, nullopt};
  } else {
    return {s.substr(0, pos), s.substr(pos + delimiter.length())};
  }
}

pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter = " ") {
  const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
  return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view& s, string_view delimiter = " ") {
  const auto [lhs, rhs] = SplitTwo(s, delimiter);
  s = rhs;
  return lhs;
}

int ConvertToInt(string_view str) {
  // use std::from_chars when available to git rid of string copy
  size_t pos;
  const int result = stoi(string(str), &pos);
  if (pos != str.length()) {
    std::stringstream error;
    error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
    throw invalid_argument(error.str());
  }
  return result;
}

template <typename Number>
void ValidateBounds(Number number_to_check, Number min_value, Number max_value) {
  if (number_to_check < min_value || number_to_check > max_value) {
    std::stringstream error;
    error << number_to_check << " is out of [" << min_value << ", " << max_value << "]";
    throw out_of_range(error.str());
  }
}

struct IndexSegment {
  size_t left;
  size_t right;

  size_t length() const {
    return right - left;
  }
  bool empty() const {
    return length() == 0;
  }

  bool Contains(IndexSegment other) const {
    return left <= other.left && other.right <= right;
  }
};

IndexSegment IntersectSegments(IndexSegment lhs, IndexSegment rhs) {
  const size_t left = max(lhs.left, rhs.left);
  const size_t right = min(lhs.right, rhs.right);
  return {left, max(left, right)};
}

bool AreSegmentsIntersected(IndexSegment lhs, IndexSegment rhs) {
  return !(lhs.right <= rhs.left || rhs.right <= lhs.left);
}

struct MoneyState {
  double earned = 0.0;
  double spent = 0.0;

  double ComputeIncome() const {
    return earned - spent;
  }

  MoneyState& operator+=(const MoneyState& other) {
    earned += other.earned;
    spent += other.spent;
    return *this;
  }

  MoneyState operator+(const MoneyState& other) const {
    return MoneyState(*this) += other;
  }

  MoneyState operator*(double factor) const {
    return {earned * factor, spent * factor};
  }
};

struct BulkMoneyAdder {
  MoneyState delta;
};

struct BulkTaxApplier {
  double factor = 1.0;
};

class BulkLinearUpdater {
public:
  BulkLinearUpdater() = default;

  BulkLinearUpdater(const BulkMoneyAdder& add)
      : add_(add)
  {}

  BulkLinearUpdater(const BulkTaxApplier& tax)
      : tax_(tax)
  {}

  void CombineWith(const BulkLinearUpdater& other) {
    tax_.factor *= other.tax_.factor;
    add_.delta.earned = add_.delta.earned * other.tax_.factor;
    add_.delta += other.add_.delta;
  }

  MoneyState Collapse(MoneyState origin, IndexSegment segment) const {
    origin.earned *= tax_.factor;
    return origin + add_.delta * segment.length();
  }


private:
  // apply tax first, then add
  BulkTaxApplier tax_;
  BulkMoneyAdder add_;
};


template <typename Data, typename BulkOperation>
class SummingSegmentTree {
public:
  SummingSegmentTree(size_t size) : root_(Build({0, size})) {}

  Data ComputeSum(IndexSegment segment) const {
    return this->TraverseWithQuery(root_, segment, ComputeSumVisitor{});
  }

  void AddBulkOperation(IndexSegment segment, const BulkOperation& operation) {
    this->TraverseWithQuery(root_, segment, AddBulkOperationVisitor{operation});
  }

private:
  struct Node;
  using NodeHolder = unique_ptr<Node>;

  struct Node {
    NodeHolder left;
    NodeHolder right;
    IndexSegment segment;
    Data data;
    BulkOperation postponed_bulk_operation;
  };

  NodeHolder root_;

  static NodeHolder Build(IndexSegment segment) {
    if (segment.empty()) {
      return nullptr;
    } else if (segment.length() == 1) {
      return make_unique<Node>(Node{
        .left = nullptr,
        .right = nullptr,
        .segment = segment,
      });
    } else {
      const size_t middle = segment.left + segment.length() / 2;
      return make_unique<Node>(Node{
        .left = Build({segment.left, middle}),
        .right = Build({middle, segment.right}),
        .segment = segment,
      });
    }
  }

  template <typename Visitor>
  static typename Visitor::ResultType TraverseWithQuery(const NodeHolder& node, IndexSegment query_segment, Visitor visitor) {
    if (!node || !AreSegmentsIntersected(node->segment, query_segment)) {
      return visitor.ProcessEmpty(node);
    } else {
      PropagateBulkOperation(node);
      if (query_segment.Contains(node->segment)) {
        return visitor.ProcessFull(node);
      } else {
        if constexpr (is_void_v<typename Visitor::ResultType>) {
          TraverseWithQuery(node->left, query_segment, visitor);
          TraverseWithQuery(node->right, query_segment, visitor);
          return visitor.ProcessPartial(node, query_segment);
        } else {
          return visitor.ProcessPartial(
              node, query_segment,
              TraverseWithQuery(node->left, query_segment, visitor),
              TraverseWithQuery(node->right, query_segment, visitor)
          );
        }
      }
    }
  }

  class ComputeSumVisitor {
  public:
    using ResultType = Data;

    Data ProcessEmpty(const NodeHolder&) const {
      return {};
    }

    Data ProcessFull(const NodeHolder& node) const {
      return node->data;
    }

    Data ProcessPartial(const NodeHolder&, IndexSegment, const Data& left_result, const Data& right_result) const {
      return left_result + right_result;
    }
  };

  class AddBulkOperationVisitor {
  public:
    using ResultType = void;

    explicit AddBulkOperationVisitor(const BulkOperation& operation)
        : operation_(operation)
    {}

    void ProcessEmpty(const NodeHolder&) const {}

    void ProcessFull(const NodeHolder& node) const {
      node->postponed_bulk_operation.CombineWith(operation_);
      node->data = operation_.Collapse(node->data, node->segment);
    }

    void ProcessPartial(const NodeHolder& node, IndexSegment) const {
      node->data = (node->left ? node->left->data : Data()) + (node->right ? node->right->data : Data());
    }

  private:
    const BulkOperation& operation_;
  };

  static void PropagateBulkOperation(const NodeHolder& node) {
    for (auto* child_ptr : {node->left.get(), node->right.get()}) {
      if (child_ptr) {
        child_ptr->postponed_bulk_operation.CombineWith(node->postponed_bulk_operation);
        child_ptr->data = node->postponed_bulk_operation.Collapse(child_ptr->data, child_ptr->segment);
      }
    }
    node->postponed_bulk_operation = BulkOperation();
  }
};


class Date {
public:
  static Date FromString(string_view str) {
    const int year = ConvertToInt(ReadToken(str, "-"));
    const int month = ConvertToInt(ReadToken(str, "-"));
    ValidateBounds(month, 1, 12);
    const int day = ConvertToInt(str);
    ValidateBounds(day, 1, 31);
    return {year, month, day};
  }

  // Weird legacy, can't wait for std::chrono::year_month_day
  time_t AsTimestamp() const {
    std::tm t;
    t.tm_sec  = 0;
    t.tm_min  = 0;
    t.tm_hour = 0;
    t.tm_mday = day_;
    t.tm_mon  = month_ - 1;
    t.tm_year = year_ - 1900;
    t.tm_isdst = 0;
    return mktime(&t);
  }

private:
  int year_;
  int month_;
  int day_;

  Date(int year, int month, int day)
      : year_(year), month_(month), day_(day)
  {}
};

int ComputeDaysDiff(const Date& date_to, const Date& date_from) {
  const time_t timestamp_to = date_to.AsTimestamp();
  const time_t timestamp_from = date_from.AsTimestamp();
  static constexpr int SECONDS_IN_DAY = 60 * 60 * 24;
  return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

static const Date START_DATE = Date::FromString("2000-01-01");
static const Date END_DATE = Date::FromString("2100-01-01");
static const size_t DAY_COUNT = ComputeDaysDiff(END_DATE, START_DATE);

size_t ComputeDayIndex(const Date& date) {
  return ComputeDaysDiff(date, START_DATE);
}

IndexSegment MakeDateSegment(const Date& date_from, const Date& date_to) {
  return {ComputeDayIndex(date_from), ComputeDayIndex(date_to) + 1};
}


class BudgetManager : public SummingSegmentTree<MoneyState, BulkLinearUpdater> {
public:
    BudgetManager() : SummingSegmentTree(DAY_COUNT) {}
};


struct Request;
using RequestHolder = unique_ptr<Request>;

struct Request {
  enum class Type {
    COMPUTE_INCOME,
    EARN,
    PAY_TAX,
    SPEND
  };

  Request(Type type) : type(type) {}
  static RequestHolder Create(Type type);
  virtual void ParseFrom(string_view input) = 0;
  virtual ~Request() = default;

  const Type type;
};

const unordered_map<string_view, Request::Type> STR_TO_REQUEST_TYPE = {
    {"ComputeIncome", Request::Type::COMPUTE_INCOME},
    {"Earn", Request::Type::EARN},
    {"PayTax", Request::Type::PAY_TAX},
    {"Spend", Request::Type::SPEND},
};

template <typename ResultType>
struct ReadRequest : Request {
  using Request::Request;
  virtual ResultType Process(const BudgetManager& manager) const = 0;
};

struct ModifyRequest : Request {
  using Request::Request;
  virtual void Process(BudgetManager& manager) const = 0;
};

struct ComputeIncomeRequest : ReadRequest<double> {
  ComputeIncomeRequest() : ReadRequest(Type::COMPUTE_INCOME) {}
  void ParseFrom(string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(input);
  }

  double Process(const BudgetManager& manager) const override {
    return manager.ComputeSum(MakeDateSegment(date_from, date_to)).ComputeIncome();
  }

  Date date_from = START_DATE;
  Date date_to = START_DATE;
};

struct EarnRequest : ModifyRequest {
  EarnRequest() : ModifyRequest(Type::EARN) {}
  void ParseFrom(string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(ReadToken(input));
    income = ConvertToInt(input);
  }

  void Process(BudgetManager& manager) const override {
    const auto date_segment = MakeDateSegment(date_from, date_to);
    const double daily_income = income * 1.0 / date_segment.length();
    manager.AddBulkOperation(date_segment, BulkMoneyAdder{{.earned = daily_income}});
  }

  Date date_from = START_DATE;
  Date date_to = START_DATE;
  size_t income = 0;
};

struct PayTaxRequest : ModifyRequest {
  PayTaxRequest() : ModifyRequest(Type::PAY_TAX) {}
  void ParseFrom(string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(ReadToken(input));
    percentage = ConvertToInt(input);
  }

  void Process(BudgetManager& manager) const override {
    const auto date_segment = MakeDateSegment(date_from, date_to);
    const double factor = 1.0 - percentage / 100.0;
    manager.AddBulkOperation(date_segment, BulkTaxApplier{factor});
  }

  Date date_from = START_DATE;
  Date date_to = START_DATE;
  uint8_t percentage = 0;
};

struct SpendRequest : ModifyRequest {
  SpendRequest() : ModifyRequest(Type::SPEND) {}
  void ParseFrom(string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(ReadToken(input));
    spending = ConvertToInt(input);
  }

  void Process(BudgetManager& manager) const override {
    const auto date_segment = MakeDateSegment(date_from, date_to);
    const double daily_spending = spending * 1.0 / date_segment.length();
    manager.AddBulkOperation(date_segment, BulkMoneyAdder{{.spent = daily_spending}});
  }

  Date date_from = START_DATE;
  Date date_to = START_DATE;
  size_t spending = 0;
};

RequestHolder Request::Create(Request::Type type) {
  switch (type) {
    case Request::Type::COMPUTE_INCOME:
      return make_unique<ComputeIncomeRequest>();
    case Request::Type::EARN:
      return make_unique<EarnRequest>();
    case Request::Type::PAY_TAX:
      return make_unique<PayTaxRequest>();
    case Request::Type::SPEND:
      return make_unique<SpendRequest>();
    default:
      return nullptr;
  }
}

template <typename Number>
Number ReadNumberOnLine(istream& stream) {
  Number number;
  stream >> number;
  string dummy;
  getline(stream, dummy);
  return number;
}

optional<Request::Type> ConvertRequestTypeFromString(string_view type_str) {
  if (const auto it = STR_TO_REQUEST_TYPE.find(type_str);
      it != STR_TO_REQUEST_TYPE.end()) {
    return it->second;
  } else {
    return nullopt;
  }
}

RequestHolder ParseRequest(string_view request_str) {
  const auto request_type = ConvertRequestTypeFromString(ReadToken(request_str));
  if (!request_type) {
    return nullptr;
  }
  RequestHolder request = Request::Create(*request_type);
  if (request) {
    request->ParseFrom(request_str);
  };
  return request;
}

vector<RequestHolder> ReadRequests(istream& in_stream = cin) {
  const size_t request_count = ReadNumberOnLine<size_t>(in_stream);

  vector<RequestHolder> requests;
  requests.reserve(request_count);

  for (size_t i = 0; i < request_count; ++i) {
    string request_str;
    getline(in_stream, request_str);
    if (auto request = ParseRequest(request_str)) {
      requests.push_back(move(request));
    }
  }
  return requests;
}

vector<double> ProcessRequests(const vector<RequestHolder>& requests) {
  vector<double> responses;
  BudgetManager manager;
  for (const auto& request_holder : requests) {
    if (request_holder->type == Request::Type::COMPUTE_INCOME) {
      const auto& request = static_cast<const ComputeIncomeRequest&>(*request_holder);
      responses.push_back(request.Process(manager));
    } else {
      const auto& request = static_cast<const ModifyRequest&>(*request_holder);
      request.Process(manager);
    }
  }
  return responses;
}

void PrintResponses(const vector<double>& responses, ostream& stream = cout) {
  for (const double response : responses) {
    stream << response << endl;
  }
}

void TestFromTask() {
  istringstream input(
    "8\n"
    "Earn 2000-01-02 2000-01-06 20\n"
    "ComputeIncome 2000-01-01 2001-01-01\n"
    "PayTax 2000-01-02 2000-01-03 13\n"
    "ComputeIncome 2000-01-01 2001-01-01\n"
    "Spend 2000-12-30 2001-01-02 14\n"
    "ComputeIncome 2000-01-01 2001-01-01\n"
    "PayTax 2000-12-30 2000-12-30 13\n"
    "ComputeIncome 2000-01-01 2001-01-01\n"
  );
  const auto requests = ReadRequests(input);
  const auto responses = ProcessRequests(requests);
  vector<double> expected = {
    20,
    18.96,
    8.46,
    8.46
  };
  ASSERT_EQUAL(responses, expected);
}

void TestDistibution() {
  istringstream input(
R"(25
Earn 2006-03-13 2051-08-10 798674
Earn 2026-07-09 2039-08-22 714256
Earn 2035-08-17 2093-04-28 393007
Earn 2029-09-22 2089-01-24 860606
Earn 2051-12-02 2057-03-16 915383
Earn 2006-10-09 2066-10-09 971784
Earn 2048-08-08 2055-05-21 563885
ComputeIncome 2010-01-20 2034-03-30
ComputeIncome 2032-12-16 2048-03-02
Earn 2011-03-24 2085-04-12 375555
ComputeIncome 2042-02-16 2098-07-20
ComputeIncome 2005-06-08 2092-09-28
ComputeIncome 2004-11-22 2015-11-03
ComputeIncome 2066-07-25 2083-02-14
Earn 2055-11-26 2090-03-18 7389
ComputeIncome 2004-06-14 2048-02-20
ComputeIncome 2016-08-18 2068-09-21
Earn 2001-05-19 2028-06-03 178349
Earn 2040-02-16 2089-01-27 319670
ComputeIncome 2042-03-21 2087-10-02
ComputeIncome 2006-07-07 2067-07-07
ComputeIncome 2045-11-12 2050-06-27
ComputeIncome 2012-05-29 2093-06-10
ComputeIncome 2035-01-28 2053-09-12
ComputeIncome 2032-10-06 2070-01-23)");
ostringstream output;
  const auto requests = ReadRequests(input);
  const auto responses = ProcessRequests(requests);
  vector<double> expected = {
    1303347.708403596887364984,
    1183690.667366203851997852,
    3293477.720063054468482733,
    5589196.619974760338664055,
    339946.1413161378586664796,
    440377.5377876986749470234,
    2661417.547969063278287649,
    4678810.577843304723501205,
    3535268.160334907937794924,
    5334981.520989582873880863,
    465212.7069902255898341537,
    5819281.313656309619545937,
    2149485.42075772350654006,
    3899073.412191837094724178
  };
  ASSERT_EQUAL(responses, expected);
}

void TestDistibutionMinified() {
  istringstream input(
R"(4
Earn 2006-03-13 2051-08-10 798674
Earn 2006-10-09 2066-10-09 971784
Earn 2011-03-24 2085-04-12 375555
ComputeIncome 2004-11-22 2015-11-03)");
  const auto requests = ReadRequests(input);
  const auto responses = ProcessRequests(requests);
  vector<double> expected = { 339946.1413161378586664796 };
  ASSERT_EQUAL(responses, expected);
}

void TestFullTax() {
  istringstream input(
  "7\n"
  "Earn 2000-01-01 2000-01-04 100\n"
  "ComputeIncome 2000-01-01 2000-01-04\n"
  "ComputeIncome 2000-01-01 2000-01-02\n"
  "PayTax 2000-01-01 2000-01-02 50\n"
  "PayTax 2000-01-01 2000-01-02 20\n"
  "ComputeIncome 2000-01-01 2000-01-02\n"
  "ComputeIncome 2000-01-01 2000-01-04\n");
  vector<double> expected = {
                             100,
                             50,
                             20,
                             70
  };
  const auto requests = ReadRequests(input);
  const auto responses = ProcessRequests(requests);
  ASSERT_EQUAL(responses, expected);
}

void TestNan() {
  istringstream input(
    "16\n"
    "PayTax 2001-02-08 2001-02-11 73\n"
    "PayTax 2001-02-01 2001-02-10 50\n"
    "PayTax 2001-02-14 2001-02-18 56\n"
    "ComputeIncome 2001-02-07 2001-02-09\n"
    "Earn 2001-02-01 2001-02-06 885317\n"
    "Earn 2001-02-12 2001-02-19 134326\n"
    "ComputeIncome 2001-02-18 2001-02-28\n"
    "Earn 2001-02-06 2001-02-09 517864\n"
    "PayTax 2001-02-03 2001-02-28 11\n"
    "PayTax 2001-02-13 2001-02-24 7\n"
    "PayTax 2001-02-02 2001-02-04 35\n"
    "PayTax 2001-02-13 2001-02-23 22\n"
    "ComputeIncome 2001-02-21 2001-02-22\n"
    "PayTax 2001-02-01 2001-02-12 43\n"
    "PayTax 2001-02-11 2001-02-14 23\n"
    "ComputeIncome 2001-02-06 2001-02-28\n");
  vector<double> expected = {
                             0,
                             33581.5,
                             0,
                             415019.7456027801381424069
  };
  const auto requests = ReadRequests(input);
  const auto responses = ProcessRequests(requests);
  ASSERT_EQUAL(responses, expected);
}

void RunTests() {
  TestRunner tr;
  RUN_TEST(tr, TestFromTask);
  // RUN_TEST(tr, TestDistibution);
  // RUN_TEST(tr, TestDistibutionMinified);
  RUN_TEST(tr, TestFullTax);
  RUN_TEST(tr, TestNan);
}

int main() {
  cout.precision(25);
  //RunTests();
  const auto requests = ReadRequests();
  const auto responses = ProcessRequests(requests);
  PrintResponses(responses);

  return 0;
}

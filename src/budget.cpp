#include "test_runner.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace Budget {

  template <typename Iterator>
  class Range {
  public:
    Range(Iterator begin, Iterator end) :
      begin_(begin),
      end_(end) {}
    Iterator begin() { return begin_; }
    Iterator end() { return end_; }
  private:
    Iterator begin_;
    Iterator end_;
  };

  class Date {
  public:
    Date() = default;
    Date(int year, int month, int day) :
      year(year), month(month), day(day) {}
    friend istream& operator >>(istream& input, Date& date);
    friend ostream& operator <<(ostream& output, const Date& date);
    bool operator <(const Date& other) const {
      return tie(year, month, day) < tie(other.year, other.month, other.day);
    }
    time_t AsTimestamp() const;
  private:
    int year;
    int month;
    int day;
  };

  istream& operator >>(istream& input, Date& date) {
    input >> date.year;
    input.ignore(1);
    input >> date.month;
    input.ignore(1);
    return input >> date.day;
  }

  ostream& operator <<(ostream& output, const Date& date) {
    return output << setw(4) << setfill('0') << date.year
                  << '-' << setw(2) << setfill('0') << date.month
                  << '-' << setw(2) << setfill('0') << date.day;
  }

  time_t Date::AsTimestamp() const {
    tm t;
    t.tm_sec   = 0;
    t.tm_min   = 0;
    t.tm_hour  = 0;
    t.tm_mday  = day;
    t.tm_mon   = month - 1;
    t.tm_year  = year - 1900;
    t.tm_isdst = 0;
    return mktime(&t);
  }

  int ComputeDaysDiff(const Date& date_to, const Date& date_from) {
    const time_t timestamp_to = date_to.AsTimestamp();
    const time_t timestamp_from = date_from.AsTimestamp();
    constexpr int SECONDS_IN_DAY = 60 * 60 * 24;
    return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
  }

  enum class TransactionType {
    EARN,
    PAY_TAX,
    SPEND,
    COMPUTE_INCOME
  };

  istream& operator >>(istream& input, TransactionType& query_type) {
    string type;
    input >> type;

    if (type == "Earn")
      query_type = TransactionType::EARN;
    else if (type == "ComputeIncome")
      query_type = TransactionType::COMPUTE_INCOME;
    else if (type == "PayTax")
      query_type = TransactionType::PAY_TAX;
    else if (type == "Spend")
      query_type = TransactionType::SPEND;
    else
      throw invalid_argument("Undefined request type " + type);
    return input;
  }

  class Request;
  class Response;
  class Manager;

  using RequestPtr = unique_ptr<Request>;
  using ResponsePtr = unique_ptr<Response>;

  class Response {
  public:
    virtual void Print(ostream&) const = 0;
    virtual ~Response() {}
  };

  class EmptyResponse : public Response {
  public:
    void Print(ostream&) const override {}
  };

  class ComputeIncomeResponse : public Response {
  public:
    ComputeIncomeResponse(double income) : income(income) {}
    void Print(ostream& output) const override { output << income << '\n'; }
  private:
    double income;
  };

  class Request {
  public:
    friend class Manager;
    Request(istream& input) { input >> begin_date >> end_date; }
    virtual ResponsePtr Execute(Manager& budget_manager) const = 0;
    virtual ~Request() {}
  private:
    Date begin_date;
    Date end_date;
  };

  class EarnRequest : public Request {
  public:
    friend class Manager;
    EarnRequest(istream& input) : Request(input) { input >> sum; }
    ResponsePtr Execute(Manager& budget_manager) const override;
  private:
    double sum = 0;
  };

  class ComputeIncomeRequest : public Request {
  public:
    friend class Manager;
    ComputeIncomeRequest(istream& input) : Request(input) {}
    ResponsePtr Execute(Manager& budget_manager) const override;
  };

  class PayTaxRequest : public Request {
  public:
    friend class Manager;
    PayTaxRequest(istream& input) : Request(input) { input >> percent; }
    ResponsePtr Execute(Manager& budget_manager) const override;
  private:
    int percent = 13;
  };

  class SpendRequest : public Request {
  public:
    friend class Manager;
    SpendRequest(istream& input) : Request(input) { input >> sum; }
    ResponsePtr Execute(Manager& budget_manager) const override;
  private:
    double sum = 0;
  };

  class Manager {
  public:
    void Earn(const EarnRequest* request);
    double ComputeIncome(const ComputeIncomeRequest* request) const;
    void PayTax(const PayTaxRequest* request);
    void Spend(const SpendRequest* request);
  private:
    unordered_map<TransactionType, map<Date, map<Date, double>>> transactions;
    struct Transaction {
      TransactionType type;
      Date begin;
      Date end;
      double sum;
    };
    list<Transaction> GetTransactions(const Request* request,
                                      const vector<TransactionType>& types =
                                        {
                                         TransactionType::EARN,
                                         TransactionType::PAY_TAX,
                                         TransactionType::SPEND
                                        }) const;
    void AddTransaction(const Transaction& transaction);
  };

  ResponsePtr EarnRequest::Execute(Manager& budget_manager) const {
    budget_manager.Earn(this);
    return make_unique<EmptyResponse>();
  }

  ResponsePtr ComputeIncomeRequest::Execute(Manager& budget_manager) const {
    return make_unique<ComputeIncomeResponse>(
      budget_manager.ComputeIncome(this));
  }

  ResponsePtr PayTaxRequest::Execute(Manager& budget_manager) const {
    budget_manager.PayTax(this);
    return make_unique<EmptyResponse>();
  }

  ResponsePtr SpendRequest::Execute(Manager& budget_manager) const {
    budget_manager.Spend(this);
    return make_unique<EmptyResponse>();
  }

  void Manager::AddTransaction(const Transaction& transaction) {
    transactions[transaction.type][transaction.begin][transaction.end]
      += transaction.sum;
  }

  list<Manager::Transaction>
  Manager::GetTransactions(const Request* request,
                           const vector<TransactionType>& types) const {
    list<Transaction> requested_transactions;
    for (TransactionType type : types) {
      if (auto it = transactions.find(type);
          it != transactions.end()) {
        auto transactions_by_begin = it->second;
        for (const auto& [begin_date, transactions_by_end] : Range(
            transactions_by_begin.begin(),
            transactions_by_begin.upper_bound(request->end_date))) {
          for (const auto& [end_date, sum] : Range(
              transactions_by_end.lower_bound(request->begin_date),
              transactions_by_end.end())) {
            Date max_begin = max(begin_date, request->begin_date);
            Date min_end = min(end_date, request->end_date);
            requested_transactions.push_back({
                type,
                move(max_begin),
                move(min_end),
                sum
                  / (ComputeDaysDiff(end_date, begin_date) + 1)
                  * (ComputeDaysDiff(min_end, max_begin) + 1)
            });
          }
        }
      }
    }
    return requested_transactions;
  }

  void Manager::Earn(const EarnRequest* request) {
    AddTransaction({
                    TransactionType::EARN,
                    request->begin_date,
                    request->end_date,
                    request->sum
    });
  }

  double Manager::ComputeIncome(const ComputeIncomeRequest* request) const {
    auto transactions = GetTransactions(request);
    return accumulate(transactions.begin(),
                      transactions.end(),
                      .0,
                      [](double sum, const Transaction& transaction) {
                        return sum + transaction.sum;
                      });
  }

  void Manager::PayTax(const PayTaxRequest* request) {
    if (request->percent > 0) {
      double tax_percent = request->percent / 100.0;
      for (auto& transaction : GetTransactions(
                                               request,
                                               {
                                                TransactionType::EARN,
                                                TransactionType::PAY_TAX
                                               })) {
        transaction.type = TransactionType::PAY_TAX;
        transaction.sum = -transaction.sum * tax_percent;
        AddTransaction(transaction);
      }
    }
  }

  void Manager::Spend(const SpendRequest* request) {
    AddTransaction({
                    TransactionType::SPEND,
                    request->begin_date,
                    request->end_date,
                    -request->sum
    });
  }

  RequestPtr ReadRequest(istream& input) {
    TransactionType type;
    input >> type;

    RequestPtr request;
    switch (type) {
    case TransactionType::EARN:
      return make_unique<EarnRequest>(input);
    case TransactionType::COMPUTE_INCOME:
      return make_unique<ComputeIncomeRequest>(input);
    case TransactionType::PAY_TAX:
      return make_unique<PayTaxRequest>(input);
    case TransactionType::SPEND:
      return make_unique<SpendRequest>(input);
    default:
      throw invalid_argument("Request type is not implemented");
    }
  }

}

using namespace Budget;

void ProcessBudgetQueries(istream& input, ostream& output) {
  size_t query_count;
  input >> query_count;

  output << setprecision(25);
  Manager budget_manager;
  for (size_t i = 0; i < query_count; i++) {
    ReadRequest(input)->Execute(budget_manager)->Print(output);
  }
}

template<class T, class U, class P>
void AssertNearEqual(const T& t, const U& u, const P& p, const std::string& hint = {}) {
  if (!(t - p <= u && t + p >= u)) {
    std::ostringstream os;
    os << std::setprecision(25)
       << "Assertion failed: " << t << " != " << u;
    if (!hint.empty()) {
       os << " hint: " << hint;
    }
    throw std::runtime_error(os.str());
  }
}

#define ASSERT_EQUAL_WITH_PRECISION(x, y, p) {      \
    std::ostringstream _special_var;                \
    _special_var << std::setprecision(25)           \
                 << #x << " != " << #y              \
                 << " with precision = "            \
                 << p << ", "                       \
                 << __FILE__ << ":" << __LINE__;    \
    AssertNearEqual(x, y, p, _special_var.str());   \
}                                                   \

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
  ostringstream output;
  ProcessBudgetQueries(input, output);
  vector<double> expected = {
    20,
    18.96,
    8.46,
    8.46
  };
  istringstream result(output.str());
  for (double x : expected) {
    double g;
    result >> g;
    ASSERT_EQUAL_WITH_PRECISION(x, g, 0.00001);
  }
}

void TestDateDiff() {
  ASSERT_EQUAL(ComputeDaysDiff(Date(2020, 2, 1), Date(2019, 2, 1)), 365);
  ASSERT_EQUAL(ComputeDaysDiff(Date(2020, 2, 22), Date(2019, 2, 22)), 365);
  ASSERT_EQUAL(ComputeDaysDiff(Date(2021, 2, 22), Date(2020, 2, 22)), 366);
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
  ProcessBudgetQueries(input, output);
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
  istringstream result(output.str());
  for (double x : expected) {
    double g;
    result >> g;
    ASSERT_EQUAL_WITH_PRECISION(x, g, 0.00001);
  }
}

void TestDistibutionMinified() {
  istringstream input(
R"(4
Earn 2006-03-13 2051-08-10 798674
Earn 2006-10-09 2066-10-09 971784
Earn 2011-03-24 2085-04-12 375555
ComputeIncome 2004-11-22 2015-11-03)");
  ostringstream output;
  ProcessBudgetQueries(input, output);
  istringstream result(output.str());
  vector<double> expected = { 339946.1413161378586664796 };
  for (double x : expected) {
    double g;
    result >> g;
    ASSERT_EQUAL_WITH_PRECISION(x, g, 0.00001);
  }
}

void TestFullTax() {
  istringstream input(
  "7\n"
  "Earn 2000-01-01 2000-01-04 100\n"
  "ComputeIncome 2000-01-01 2000-01-04\n"
  "ComputeIncome 2000-01-01 2000-01-02\n"
  "PayTax 2000-01-01 2000-01-02 100\n"
  "PayTax 2000-01-01 2000-01-02 100\n"
  "ComputeIncome 2000-01-01 2000-01-02\n"
  "ComputeIncome 2000-01-01 2000-01-04\n");
  ostringstream output;
  ProcessBudgetQueries(input, output);
  istringstream result(output.str());
  vector<double> expected = {
                             100,
                             50,
                             0,
                             50
  };
  for (double expected_income : expected) {
    double income;
    result >> income;
    ASSERT_EQUAL_WITH_PRECISION(income, expected_income, 0.00001);
  }
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
  ostringstream output;
  ProcessBudgetQueries(input, output);
  istringstream result(output.str());
  vector<double> expected = {
                             0,
                             33581.5,
                             0,
                             415019.7456027801381424069
  };
  for (double expected_income : expected) {
    double income;
    result >> income;
    ASSERT_EQUAL_WITH_PRECISION(income, expected_income, 0.00001);
  }
}

void RunTests() {
  TestRunner tr;
  RUN_TEST(tr, TestFromTask);
  RUN_TEST(tr, TestDistibution);
  RUN_TEST(tr, TestDistibutionMinified);
  RUN_TEST(tr, TestDateDiff);
  RUN_TEST(tr, TestFullTax);
  RUN_TEST(tr, TestNan);
}

int main() {
  //RunTests();
  ProcessBudgetQueries(cin, cout);
  return 0;
}

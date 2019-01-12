#include "test_runner.h"
#include <cstdint>
#include <numeric>
#include <future>
#include <vector>
using namespace std;

template <typename Iterator>
class Page {
public:
  Page(Iterator begin, Iterator end) :
    first(begin),
    last(end),
    page_size(distance(begin, end)) {}
  Iterator begin() {
    return first;
  }
  auto end() {
    return last;
  }
  Iterator begin() const {
    return first;
  }
  Iterator end() const {
    return last;
  }
  size_t size() const {
    return page_size;
  }
private:
  Iterator first, last;
  const size_t page_size;
};

template <typename Iterator>
class Paginator {
public:
  Paginator(Iterator begin, Iterator end, const size_t page_size) {
    auto it = begin;
    while (it != end) {
      auto prev = it;
      size_t i = 0;
      while (i < page_size && it != end) {
        it = next(it);
        i++;
      }
      pages.push_back({prev, it});
    }
  }
  auto begin() {
    return pages.begin();
  }
  auto end() {
    return pages.end();
  }
  size_t size() const {
    return pages.size();
  }
private:
  vector<Page<Iterator>> pages;
};

template <typename C>
auto Paginate(C& c, size_t page_size) {
  return Paginator(c.begin(), c.end(), page_size);
}

int64_t CalculateMatrixSum(const vector<vector<int>>& matrix) {
  auto paginator = Paginate(matrix, 2000);
  vector<future<int64_t>> futures;
  for (auto& page : paginator) {
    futures.push_back(async([&page] {
                              return accumulate(
                                                page.begin(),
                                                page.end(),
                                                static_cast<int64_t>(0),
                                                [](const int64_t sum, const vector<int>& item) {
                                                  return sum + accumulate(item.begin(),
                                                                          item.end(),
                                                                          static_cast<int64_t>(0));
                                                });
                            })
      );
  }
  return accumulate(futures.begin(),
                    futures.end(),
                    static_cast<int64_t>(0),
                    [](const int64_t sum, future<int64_t>& f) {
                      return sum + f.get();
                    });
}
void TestCalculateMatrixSum() {
  const vector<vector<int>> matrix = {
                                      {1, 2, 3, 4},
                                      {5, 6, 7, 8},
                                      {9, 10, 11, 12},
                                      {13, 14, 15, 16},
                                      {1, 1, 1, 1}
  };
  ASSERT_EQUAL(CalculateMatrixSum(matrix), 140);

  const vector<vector<int>> matrix2;
  ASSERT_EQUAL(CalculateMatrixSum(matrix2), 0);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestCalculateMatrixSum);
}

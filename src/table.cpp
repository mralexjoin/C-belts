#include <utility>
#include <vector>
#include "test_runner.h"

using namespace std;

// Реализуйте здесь шаблонный класс Table
template<typename T>
class Table {
public:
  Table(const size_t rows, const size_t columns) :
    rows(rows),
    columns(columns),
    table(rows, vector<T>(columns)) {};
  const vector<T>& operator[](const size_t row) const {
    return table[row];
  }
  vector<T>& operator[](const size_t row) {
    return table[row];
  }
  void Resize(const size_t _rows, const size_t _columns) {
    rows = _rows;
    columns = _columns;
    table.resize(rows, vector<T>(columns));
    for (auto& row : table) {
      row.resize(columns);
    }
  }
  pair<size_t, size_t> Size() const {
    return {rows, columns};
  }
private:
  size_t rows;
  size_t columns;
  vector<vector<T>> table;
};

void TestTable() {
  Table<int> t(1, 1);
  ASSERT_EQUAL(t.Size().first, 1u);
  ASSERT_EQUAL(t.Size().second, 1u);
  t[0][0] = 42;
  ASSERT_EQUAL(t[0][0], 42);
  t.Resize(3, 4);
  ASSERT_EQUAL(t.Size().first, 3u);
  ASSERT_EQUAL(t.Size().second, 4u);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestTable);
  return 0;
}

#include "sum_reverse_sort.h"
#include "test_runner.h"

void TestSum() {
  AssertEqual(Sum(2, 3), 5, "Sum(2, 3) = 5");
}

void TestReverse() {
  AssertEqual(Reverse("asdf"), "fdsa", "Reverse(\"asdf\") = \"fdsa\"");
}

void TestSort() {
  vector<int> v = {5, 1, 3, 2};
  Sort(v);
  vector<int> sorted = {1, 2, 3, 5};
  AssertEqual(v, sorted, "Sort({5, 1, 3, 2}) = {1, 2, 3, 5}");
}

int main() {
  TestRunner tr;
  tr.RunTest(TestSum, "TestSum");
  tr.RunTest(TestReverse, "TestReverse");
  tr.RunTest(TestSort, "TestSort");
}

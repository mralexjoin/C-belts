#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

using namespace std;

template <typename T>
class PriorityCollection {
private:
  map<size_t, pair<T, int>> items;
  map<int, set<size_t>> priorities;

  size_t MaxIndex() const;
  size_t NextIndex() const;
  void InsertNew(size_t index, T object);

public:
  using Id = size_t;

  // Добавить объект с нулевым приоритетом
  // с помощью перемещения и вернуть его идентификатор
  Id Add(T object);

  // Добавить все элементы диапазона [range_begin, range_end)
  // с помощью перемещения, записав выданные им идентификаторы
  // в диапазон [ids_begin, ...)
  template <typename ObjInputIt, typename IdOutputIt>
  void Add(ObjInputIt range_begin, ObjInputIt range_end,
           IdOutputIt ids_begin);

  // Определить, принадлежит ли идентификатор какому-либо
  // хранящемуся в контейнере объекту
  bool IsValid(Id id) const;

  // Получить объект по идентификатору
  const T& Get(Id id) const;

  // Увеличить приоритет объекта на 1
  void Promote(Id id);

  // Получить объект с максимальным приоритетом и его приоритет
  pair<const T&, int> GetMax() const;

  // Аналогично GetMax, но удаляет элемент из контейнера
  pair<T, int> PopMax();
};

template <typename T>
typename PriorityCollection<T>::Id PriorityCollection<T>::Add(T object) {
  size_t index = NextIndex();
  InsertNew(index, move(object));
  return index;
}

template <typename T>
template <typename ObjInputIt, typename IdOutputIt>
void PriorityCollection<T>::Add(ObjInputIt range_begin, ObjInputIt range_end,
         IdOutputIt ids_begin) {
  size_t index = NextIndex();
  for (auto it = range_begin; it != range_end; it++, ids_begin++) {
    InsertNew(index, move(*it));
    *ids_begin = index++;
  }
}

template <typename T>
bool PriorityCollection<T>::IsValid(typename PriorityCollection<T>::Id id) const {
  return items.count(id) > 0;
}

template <typename T>
const T& PriorityCollection<T>::Get(typename PriorityCollection<T>::Id id) const {
  return items.at(id).first;
}

template <typename T>
pair<const T&, int> PriorityCollection<T>::GetMax() const {
  return items.at(MaxIndex());
}

template <typename T>
void PriorityCollection<T>::Promote(PriorityCollection<T>::Id id) {
  int& priority = items[id].second;
  auto previous_priority_iterator = priorities.find(priority);
  previous_priority_iterator->second.erase(id);
  if (previous_priority_iterator->second.empty()) {
    priorities.erase(previous_priority_iterator);
  }
  priorities[++priority].insert(id);
}

template <typename T>
pair<T, int> PriorityCollection<T>::PopMax() {
  auto max_priority_iterator = priorities.rbegin();
  auto max_index_iterator = max_priority_iterator->second.rbegin();

  auto max_item_iterator = items.find(*max_index_iterator);

  max_priority_iterator->second.erase(*max_index_iterator);
  if (max_priority_iterator->second.empty()) {
    priorities.erase(max_priority_iterator->first);
  }

  pair<T, int> max_item = move(max_item_iterator->second);
  items.erase(max_item_iterator);
  return max_item;
}

template <typename T>
size_t PriorityCollection<T>::MaxIndex() const {
  return *(priorities.rbegin()->second.rbegin());
}

template <typename T>
size_t PriorityCollection<T>::NextIndex() const {
  if (!items.empty()) {
    return items.rbegin()->first + 1;
  }
  return 0;
}

template <typename T>
void PriorityCollection<T>::InsertNew(size_t index, T object) {
  items[index] = {move(object), 0};
  priorities[0].insert(index);
}

class StringNonCopyable : public string {
public:
  using string::string;  // Позволяет использовать конструкторы строки
  StringNonCopyable(const StringNonCopyable&) = delete;
  StringNonCopyable(StringNonCopyable&&) = default;
  StringNonCopyable& operator=(const StringNonCopyable&) = delete;
  StringNonCopyable& operator=(StringNonCopyable&&) = default;
};

void TestNoCopy() {
  PriorityCollection<StringNonCopyable> strings;
  //const auto white_id =
    strings.Add("white");
  const auto yellow_id = strings.Add("yellow");
  const auto red_id = strings.Add("red");

  strings.Promote(yellow_id);

  for (int i = 0; i < 2; ++i) {
    strings.Promote(red_id);
  }
  strings.Promote(yellow_id);

  {
    const auto item = strings.GetMax();
    ASSERT_EQUAL(item.first, "red");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "red");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "yellow");
    ASSERT_EQUAL(item.second, 2);
  }
  {
    const auto item = strings.PopMax();
    ASSERT_EQUAL(item.first, "white");
    ASSERT_EQUAL(item.second, 0);
  }

  {
    vector<StringNonCopyable> strings2;
    strings2.push_back("1");
    strings2.push_back("2");
    strings2.push_back("3");
    vector<PriorityCollection<StringNonCopyable>::Id> indices;
    strings.Add(strings2.begin(), strings2.end(), back_inserter(indices));
    vector<size_t> expected = {0, 1, 2};
    ASSERT_EQUAL(indices, expected);
  }

}

void MyTests() {
  PriorityCollection<StringNonCopyable> strings;
  {
    const auto white_id = strings.Add("white");
    const auto yellow_id = strings.Add("yellow");
    const auto red_id = strings.Add("red");

    strings.Promote(white_id);
    strings.Promote(red_id);
    strings.Promote(yellow_id);
    {
      const auto item = strings.GetMax();
      ASSERT_EQUAL(item.first, "red");
      ASSERT_EQUAL(item.second, 1);
    }
    {
      const auto item = strings.PopMax();
      ASSERT_EQUAL(item.first, "red");
      ASSERT_EQUAL(item.second, 1);
    }
    {
      const auto item = strings.GetMax();
      ASSERT_EQUAL(item.first, "yellow");
      ASSERT_EQUAL(item.second, 1);
    }
    {
      const auto item = strings.PopMax();
      ASSERT_EQUAL(item.first, "yellow");
      ASSERT_EQUAL(item.second, 1);
    }
    {
      const auto item = strings.PopMax();
      ASSERT_EQUAL(item.first, "white");
      ASSERT_EQUAL(item.second, 1);
    }

  }
  {
    strings.Add("white");
    strings.Add("yellow");
    strings.Add("red");
    {
      const auto item = strings.GetMax();
      ASSERT_EQUAL(item.first, "red");
      ASSERT_EQUAL(item.second, 0);
    }
    {
      const auto item = strings.PopMax();
      ASSERT_EQUAL(item.first, "red");
      ASSERT_EQUAL(item.second, 0);
    }
    {
      const auto item = strings.GetMax();
      ASSERT_EQUAL(item.first, "yellow");
      ASSERT_EQUAL(item.second, 0);
    }
    {
      const auto item = strings.PopMax();
      ASSERT_EQUAL(item.first, "yellow");
      ASSERT_EQUAL(item.second, 0);
    }
    {
      const auto item = strings.GetMax();
      ASSERT_EQUAL(item.first, "white");
      ASSERT_EQUAL(item.second, 0);
    }
    {
      const auto item = strings.PopMax();
      ASSERT_EQUAL(item.first, "white");
      ASSERT_EQUAL(item.second, 0);
    }

    strings.Add("white");
    const auto yellow_id = strings.Add("yellow");
    strings.Add("red");
    strings.Promote(yellow_id);
    {
      const auto item = strings.GetMax();
      ASSERT_EQUAL(item.first, "yellow");
      ASSERT_EQUAL(item.second, 1);
    }
    {
      const auto item = strings.PopMax();
      ASSERT_EQUAL(item.first, "yellow");
      ASSERT_EQUAL(item.second, 1);
    }
    {
      const auto item = strings.GetMax();
      ASSERT_EQUAL(item.first, "red");
      ASSERT_EQUAL(item.second, 0);
    }
    {
      const auto item = strings.PopMax();
      ASSERT_EQUAL(item.first, "red");
      ASSERT_EQUAL(item.second, 0);
    }
  }
}


int main() {
  TestRunner tr;
  RUN_TEST(tr, TestNoCopy);
  RUN_TEST(tr, MyTests);
  return 0;
}

#include "test_runner.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

using namespace std;

template <typename T>
class PriorityCollection {
private:
  struct Item {
    T object;
    int priority;
    size_t index;
  };

  map<size_t, Item> items;
  // Приватные поля и методы
  vector<size_t> priority_queue;

  void Exch(const size_t lhs, const size_t rhs);
  bool Less(const size_t lhs, const size_t rhs) const;
  void Swim(const size_t id);
  void Sink(const size_t id);
public:
  using Id = size_t;

  PriorityCollection() {
    priority_queue.push_back(0);
  }

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
  size_t index = 1;
  if (!items.empty()) {
    index = items.rbegin()->first + 1;
  }
  items[index] = {move(object), 0, index};
  priority_queue.push_back(index);
  Swim(index);
  return index;
}

template <typename T>
template <typename ObjInputIt, typename IdOutputIt>
void PriorityCollection<T>::Add(ObjInputIt range_begin, ObjInputIt range_end,
         IdOutputIt ids_begin) {
  vector<size_t> indices;
  for (auto it = range_begin; it != range_end; it = next(it)) {
    indices.push_back(Add(move(*it)));
  }
  std::copy(indices.begin(), indices.end(), ids_begin);
}

template <typename T>
bool PriorityCollection<T>::IsValid(typename PriorityCollection<T>::Id id) const {
  return items.find(id) != items.end();
}

template <typename T>
const T& PriorityCollection<T>::Get(typename PriorityCollection<T>::Id id) const {
  return items.at(id).object;
}

template <typename T>
pair<const T&, int> PriorityCollection<T>::GetMax() const {
  const Item& item = items.at(priority_queue[1]);
  return {item.object, item.priority};
}

template <typename T>
void PriorityCollection<T>::Promote(PriorityCollection<T>::Id id) {
  Item& item = items[id];
  item.priority++;
  Swim(item.index);
}

template <typename T>
pair<T, int> PriorityCollection<T>::PopMax() {
  size_t max_index = priority_queue[1];
  if (priority_queue.size() > 2) {
    items[priority_queue[priority_queue.size() - 1]].index = 1;
    priority_queue[1] = priority_queue[priority_queue.size() - 1];
    priority_queue.pop_back();

    Sink(1);
  }
  else {
    priority_queue.pop_back();
  }

  auto max_item_iterator = items.find(max_index);
  pair<T, int> max_item = {move(max_item_iterator->second.object),
                           max_item_iterator->second.priority};
  items.erase(max_item_iterator);
  return max_item;
}

template <typename T>
void PriorityCollection<T>::Swim(size_t index) {
  while (index > 1 && Less(index / 2, index)) {
    Exch(index / 2, index);
    index = index / 2;
  }
}

template <typename T>
void PriorityCollection<T>::Sink(size_t id) {
  while (2 * id <= priority_queue.size() - 1) {
    size_t child = 2 * id;
    if (child < priority_queue.size() - 1 && Less(child, child + 1)) {
      child++;
    }
    if (!Less(id, child)) {
      break;
    }
    Exch(child, id);
    id = child;
  }
}

template <typename T>
bool PriorityCollection<T>::Less(const size_t lhs, const size_t rhs) const {
  int left_priority  = items.at(priority_queue[lhs]).priority;
  int right_priority = items.at(priority_queue[rhs]).priority;

  if (left_priority == right_priority) {
    return priority_queue[lhs] < priority_queue[rhs];
  }
  return left_priority < right_priority;
}

template <typename T>
void PriorityCollection<T>::Exch(const size_t lhs, const size_t rhs) {
  Item& left_item = items[priority_queue[lhs]];
  Item& right_item = items[priority_queue[rhs]];
  swap(left_item.index, right_item.index);
  swap(priority_queue[lhs], priority_queue[rhs]);
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
    vector<size_t> expected = {1, 2, 3};
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

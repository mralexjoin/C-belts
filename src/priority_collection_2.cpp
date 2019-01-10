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
public:
  using Items = map<size_t, pair<T, int>>;
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

private:
  // Приватные поля и методы
  Items items;

  void Exch(const size_t lhs, const size_t rhs) {
    swap(items[lhs], items[rhs]);
  }
  bool Less(const size_t lhs, const size_t rhs) const {
    return items.at(lhs).second < items.at(rhs).second;
  }
  void Swim(const size_t id);
  void Sink(const size_t id);
};

template <typename T>
typename PriorityCollection<T>::Id PriorityCollection<T>::Add(T object) {
  size_t index = items.size() + 1;
  items[index] = {move(object), 0};
  return index;
}

template <typename T>
template <typename ObjInputIt, typename IdOutputIt>
void PriorityCollection<T>::Add(ObjInputIt range_begin, ObjInputIt range_end,
         IdOutputIt ids_begin) {
  for (auto it = range_begin; it != range_end; it = next(it), ids_begin = next(ids_begin)) {
    *ids_begin = Add(*it);
  }
}

template <typename T>
bool PriorityCollection<T>::IsValid(typename PriorityCollection<T>::Id id) const {
  return items.find(id) != items.end();
}

template <typename T>
const T& PriorityCollection<T>::Get(typename PriorityCollection<T>::Id id) const {
  return items[id].first;
}

template <typename T>
pair<const T&, int> PriorityCollection<T>::GetMax() const {
  return items[1];
}

template <typename T>
void PriorityCollection<T>::Promote(PriorityCollection<T>::Id id) {
  items[id].second++;
  Swim(id);

  for (const auto& [key, value] : items) {
    cout << key << ": " << value.first << ", " << value.second << endl;
  }
  cout << endl;
}

template <typename T>
pair<T, int> PriorityCollection<T>::PopMax() {
  auto max = move(items[1]);
  items.erase(1);
  if (!items.empty()) {
    items[1] = move(items[items.size() + 1]);
    items.erase(items.size() + 1);
    Sink(1);
  }

  return max;
}

template <typename T>
void PriorityCollection<T>::Swim(size_t index) {
  while (index > 1 && Less(index / 2, index)) {
    Exch(index / 2, index);
  }
}

template <typename T>
void PriorityCollection<T>::Sink(size_t id) {
  while (2 * id <= items.size()) {
    size_t child = 2 * id;
    if (child < items.size() && Less(child, child + 1)) {
      child++;
    }
    if (!Less(id, child)) {
      break;
    }
    Exch(child, id);
    id = child;
  }
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
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestNoCopy);
  return 0;
}

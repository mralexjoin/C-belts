#include <map>
#include <utility>
#include <vector>

using namespace std;

template<typename T> T Sqr(const T& x);

template<typename First, typename Second>
pair<First, Second> Sqr(const pair<First, Second>& p);

template<typename Key, typename Value>
map<Key, Value> Sqr(map<Key, Value> m);

template<typename T>
vector<T> Sqr(vector<T> v);

template<typename T> T Sqr(const T& x) {
  return x * x;
}

template<typename First, typename Second>
pair<First, Second> Sqr(const pair<First, Second>& p) {
  return {Sqr(p.first), Sqr(p.second)};
}

template<typename Key, typename Value>
map<Key, Value> Sqr(map<Key, Value> m) {
  for (auto& item : m)
    item.second = Sqr(item.second);
  return m;
}

template<typename T>
vector<T> Sqr(vector<T> v) {
  for (auto& item : v)
    item = Sqr(item);
  return v;
}

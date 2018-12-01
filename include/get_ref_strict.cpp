#include <exception>
#include <map>

template<typename Key, typename Value>
Value& GetRefStrict(std::map<Key, Value>& m, const Key& k) {
  if (m.count(k) == 0)
    throw std::runtime_error("No such element");
  return m.at(k);
}

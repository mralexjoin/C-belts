#include <algorithm>
#include <cstdint>

using namespace std;

// Реализуйте SimpleVector в этом файле
// и отправьте его на проверку

template <typename T>
class SimpleVector {
public:
  SimpleVector() = default;
  explicit SimpleVector(size_t size);
  SimpleVector(SimpleVector& other);
  ~SimpleVector();

  T& operator[](size_t index);

  T* begin();
  T* end();

  size_t Size() const;
  size_t Capacity() const;
  void PushBack(T value);

private:
  T* data = nullptr;
  size_t size = 0;
  size_t capacity = 0;
};

template <typename T>
SimpleVector<T>::SimpleVector(size_t size)
  : data(new T[size])
  , size(size)
  , capacity(size)
{
}

template <typename T>
SimpleVector<T>::SimpleVector(SimpleVector<T>& other) :
  data(new T[other.capacity]),
  size(other.size),
  capacity(other.capacity)
{
  move(other.begin(), other.end(), begin());
}

template <typename T>
SimpleVector<T>::~SimpleVector() {
  delete[] data;
}

template <typename T>
T& SimpleVector<T>::operator[](size_t index) {
  return data[index];
}

template <typename T>
size_t SimpleVector<T>::Size() const {
  return size;
}

template <typename T>
size_t SimpleVector<T>::Capacity() const {
  return capacity;
}

template <typename T>
void SimpleVector<T>::PushBack(T value) {
  if (size >= capacity) {
    auto new_cap = capacity == 0 ? 1 : 2 * capacity;
    auto new_data = new T[new_cap];
    move(begin(), end(), new_data);
    delete[] data;
    data = new_data;
    capacity = new_cap;
  }
  data[size++] = move(value);
}

template <typename T>
T* SimpleVector<T>::begin() {
  return data;
}

template <typename T>
T* SimpleVector<T>::end() {
  return data + size;
}

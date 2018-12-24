#pragma once

#include <algorithm>
#include <cstdlib>

// Реализуйте шаблон SimpleVector
template <typename T>
class SimpleVector {
 public:
  SimpleVector() {}

  explicit SimpleVector(size_t size) :
  size(size) {
    Reallocate();
  }
  ~SimpleVector() {
    delete[] data;
  }

  T& operator[](size_t index) {
    return data[index];
  }

  T* begin() {
    return data;
  }
  T* end() {
    return data + size;
  }

  size_t Size() const {
    return size;
  }
  size_t Capacity() const {
    return capacity;
  }
  void PushBack(const T& value) {
    if (Size() == Capacity()) {
      Reallocate();
    }
    data[size++] = value;
  }

 private:
  void Reallocate() {
    if (size == 0) {
      capacity = 2;
    }
    else {
      capacity = 2 * size;
    }
    T* new_data = new T[capacity];
    if (data != nullptr) {
      std::copy(begin(), end(), new_data);
      delete[] data;
    }
    data = new_data;
  }
  size_t size = 0, capacity = 0;
  T* data = nullptr;
};

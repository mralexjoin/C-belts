#include <stdexcept>
#include <vector>

//#include "test_runner.h"

template<typename T>
class Deque {
public:
  bool Empty() const {
    return tail.empty() && head.empty();
  }
  size_t Size() const {
    return tail.size() + head.size();
  }
  T& operator[](const size_t index) {
    if (head.size() > index)
      return head[BackwardIndex(index)];
    else
      return tail[ForwardIndex(index)];
  }
  const T& operator[](const size_t index) const {
    if (head.size() > index)
      return head[BackwardIndex(index)];
    else
      return tail[ForwardIndex(index)];
  }
  T& At(const size_t index) {
    if (head.size() > index)
      return head.at(BackwardIndex(index));
    else
      return tail.at(ForwardIndex(index));
  }
  const T& At(const size_t index) const {
    if (head.size() > index)
      return head.at(BackwardIndex(index));
    else
      return tail.at(ForwardIndex(index));
  }
  T& Front() {
    if (head.empty())
      return tail.front();
    else
      return head.back();
  }
  const T& Front() const {
    if (head.empty())
      return tail.front();
    else
      return head.back();
  }
  T& Back() {
    if (tail.empty())
      return head.front();
    else
      return tail.back();
  }
  const T& Back() const {
    if (tail.empty())
      return head.front();
    else
      return tail.back();
  }
  void PushFront(const T& item) {
    head.push_back(item);
  }
  void PushBack(const T& item) {
    tail.push_back(item);
  }

private:
  size_t BackwardIndex(const size_t index) const {
    return head.size() - index - 1;
  }
  size_t ForwardIndex(const size_t index) const {
    return index - head.size();
  }
  std::vector<T> head;
  std::vector<T> tail;
};

int main() {
  return 0;
}

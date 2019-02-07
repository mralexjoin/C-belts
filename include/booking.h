#pragma once

#include <utility>

namespace RAII {

template<typename Provider>
class Booking {
public:
  Booking(Provider* provider, int&) : provider(provider) {}
  Booking(const Booking& other) {
    if (other.provider) {
      *this = other.provider->Book({});
    }
  }
  Booking(Booking&& other) : provider(std::move(other.provider)) {
    other.provider = nullptr;
  }
  Booking& operator=(const Booking& other) = delete;
  Booking& operator=(Booking&& other) {
    this->provider = std::move(other.provider);
    other.provider = nullptr;
    return *this;
  }
  ~Booking() {
    Erase();
  }
private:
  void Erase() {
    if (provider)
      provider->CancelOrComplete(*this);
    provider = nullptr;
  }
  Provider* provider = nullptr;
};

}
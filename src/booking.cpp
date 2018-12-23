#include <deque>
#include <cstdint>
#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace std;

struct BookingEvent {
  const int64_t time;
  const string hotel_name;
  const uint32_t client_id;
  const uint16_t room_count;
};

class BookingManager {
public:
  void Book(const int64_t& time,
            const string& hotel_name,
            const uint32_t& client_id,
            const uint16_t& room_count) {
    const int64_t start_of_a_day = time - DAY_LENGTH;
    while (!events.empty()
           && events.front().time <= start_of_a_day) {
      const BookingEvent& event = events.front();

      rooms[event.hotel_name] -= event.room_count;

      auto& hotel_clients_id_count = clients[event.hotel_name];
      auto& client_id_count = hotel_clients_id_count[event.client_id];
      if (--client_id_count == 0) {
        hotel_clients_id_count.erase(event.client_id);
      }

      events.pop_front();
    }
    events.push_back({time, hotel_name, client_id, room_count});
    rooms[hotel_name] += room_count;
    clients[hotel_name][client_id]++;
  }
  size_t Clients(const string& hotel_name) const {
    if (clients.count(hotel_name) == 0)
      return 0;
    return clients.at(hotel_name).size();
  }
  uint64_t Rooms(const string& hotel_name) const {
    if (rooms.count(hotel_name) == 0)
      return 0;
    return rooms.at(hotel_name);
  }
private:
  static const uint32_t DAY_LENGTH = 86400u;
  deque<BookingEvent> events;
  map<string, uint64_t> rooms;
  map<string, map<uint32_t, uint32_t>> clients;
};

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  BookingManager booking_manager;
  uint32_t q;
  cin >> q;
  for (uint32_t i = 0; i < q; i++) {
    string query_type;
    cin >> query_type;
    if (query_type == "BOOK") {
      int64_t time;
      string hotel_name;
      uint32_t client_id;
      uint16_t room_count;
      cin >> time >> hotel_name >> client_id >> room_count;
      booking_manager.Book(time, hotel_name, client_id, room_count);
    }
    else if (query_type == "CLIENTS") {
      string hotel_name;
      cin >> hotel_name;
      cout << booking_manager.Clients(hotel_name) << '\n';
    }
    else if (query_type == "ROOMS") {
      string hotel_name;
      cin >> hotel_name;
      cout << booking_manager.Rooms(hotel_name) << '\n';
    }
  }

  return 0;
}

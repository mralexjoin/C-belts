#include "test_runner.h"
#include "profile.h"

#include <iostream>
#include <list>
#include <map>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>

 TotalDuration put("Put");
 TotalDuration erase("Erase");
 TotalDuration get_by_id("Get by id");
 TotalDuration by_timestamp("Range by timestamp");
 TotalDuration by_karma("Range by karma");
 TotalDuration by_user("Range by user");

using namespace std;

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;
};

class Database {
public:
  bool Put(const Record& record);
  const Record* GetById(const string& id) const;
  bool Erase(const string& id);

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) const;

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const;

  template <typename Callback>
  void AllByUser(const string& user, Callback callback) const;
private:
  using Records = list<Record>;
  using IntIndex = multimap<int, Records::iterator>;
  using StringIndex = multimap<string_view, Records::iterator>;
  using IndexIterators = tuple<Records::iterator, IntIndex::iterator, IntIndex::iterator, StringIndex::iterator>;
  using PrimaryIndex = unordered_map<string_view, IndexIterators>;

  Records records;
  PrimaryIndex primary_index;
  IntIndex timestamp_index;
  IntIndex karma_index;
  StringIndex user_index;
};

bool Database::Put(const Record& record) {
   ADD_DURATION(put);
  if (primary_index.find(record.id) != primary_index.end()) {
    return false;
  }
  records.push_front(record);
  auto it = records.begin();
  primary_index.insert(
    {
      it->id, 
      {
        it,
        timestamp_index.insert({it->timestamp, it}),
        karma_index.insert({it->karma, it}),
        user_index.insert({it->user, it})
      }
    }
  );
  return true;
}

const Record* Database::GetById(const string& id) const {
   ADD_DURATION(get_by_id);
  auto it = primary_index.find(id);
  if (it == primary_index.end()) {
    return nullptr;
  }
  return &(*(get<0>(it->second)));
}

bool Database::Erase(const string& id) {
   ADD_DURATION(erase);
  auto it = primary_index.find(id);
  if (it == primary_index.end()) {
    return false;
  }

  IndexIterators& iterators = it->second;

  timestamp_index.erase(get<1>(iterators));
  karma_index.erase(get<2>(iterators));
  user_index.erase(get<3>(iterators));
  records.erase(get<0>(iterators));
  primary_index.erase(it);
  return true;
}

template <typename Callback>
void Database::RangeByTimestamp(int low, int high, Callback callback) const {
   ADD_DURATION(by_timestamp);
  if (low <= high) {
    auto first = timestamp_index.lower_bound(low);
    auto last = timestamp_index.upper_bound(high);
    for (auto it = first; it != last && callback(*it->second); it++);
  }
}

template <typename Callback>
void Database::RangeByKarma(int low, int high, Callback callback) const {
   ADD_DURATION(by_karma);
  if (low <= high) {
    auto first = karma_index.lower_bound(low);
    auto last = karma_index.upper_bound(high);
    for (auto it = first; it != last && callback(*it->second); it++);
  }
}

template <typename Callback>
void Database::AllByUser(const string& user, Callback callback) const {
   ADD_DURATION(by_user);
  auto [first, last] = user_index.equal_range(user);
  for (auto it = first; it != last && callback(*it->second); it++);
}

void TestRangeBoundaries() {
  const int good_karma = 1000;
  const int bad_karma = -10;

  Database db;
  db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
  db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

  int count = 0;
  db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestSameUser() {
  Database db;
  db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
  db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

  int count = 0;
  db.AllByUser("master", [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestReplacement() {
  const string final_body = "Feeling sad";

  Database db;
  db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
  db.Erase("id");
  db.Put({"id", final_body, "not-master", 1536107260, -10});

  auto record = db.GetById("id");
  ASSERT(record != nullptr);
  ASSERT_EQUAL(final_body, record->title);
}

void TestEfficiency() {
  Database db;

  size_t N = 5'000'000;

  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> dis(1, N);
  uniform_int_distribution<> dis2(0, 2);
  string dict = "abcdefghijklmnopqrstuvwxyz";
  for (size_t i = 0; i < 1'000'000; i++) {
    switch (dis2(gen)) {
      case 0: {
        Record record = {to_string(dis(gen)), to_string(dis(gen)), to_string(dis(gen)), dis(gen), dis(gen)};
        db.Put(record);
        break;
      }
      case 1:
        db.Erase(to_string(dis(gen)));
        break;
      case 2:
        db.GetById(to_string(dis(gen)));
        break;
    }
  }
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestRangeBoundaries);
  RUN_TEST(tr, TestSameUser);
  RUN_TEST(tr, TestReplacement);
  RUN_TEST(tr, TestEfficiency);
  return 0;
}

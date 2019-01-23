#include "test_runner.h"

#include <memory>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;
};

// Реализуйте этот класс
class Database {
public:
  enum class IndexFields {
    TIMESTAMP,
    KARMA,
    USER,
    END
  };

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
  
  class Index {
  private:
    class IndexKey {
    public:
      IndexKey(const string& value) : string_value(value) {}
      IndexKey(const int value) : int_value(value) {}
      bool operator<(const IndexKey& other) const {
        return tuple(string_value, int_value) < tuple(other.string_value, other.int_value);
      }
    private:
      const string string_value;
      const int int_value = 0;
    };

    IndexKey CreateIndexKey(const Record& record, IndexFields field);
    array<multimap<IndexKey, string>, static_cast<size_t>(IndexFields::END)> index;
  public:
    void Put(const Record& record);
    void Erase(const Record& record);
    const multimap<IndexKey, string>& Get(const IndexFields field) const {
      return index[static_cast<size_t>(field)];
    }
    template<typename T>
    static IndexKey CreateIndexKey(const T& value, IndexFields field);
  };

  template <typename Callback>
  void RangeByIndexField(IndexFields field, int low, int high, Callback callback) const;

  template <typename Callback>
  void AllByIndexField(IndexFields field, const string& user, Callback callback) const;

  Index index;
  unordered_map<string, Record> records;
};

Database::Index::IndexKey Database::Index::CreateIndexKey(const Record& record,
                                                           IndexFields field) {
  switch(field) {
    case IndexFields::TIMESTAMP:
      return IndexKey(record.timestamp);
    case IndexFields::KARMA:
      return IndexKey(record.karma);
    case IndexFields::USER:
      return IndexKey(record.user);
    case IndexFields::END:
    default:
      throw invalid_argument("Not implemented");
  }
}

template<typename T>
Database::Index::IndexKey Database::Index::CreateIndexKey(const T& value, IndexFields field) {
  switch(field) {
    case IndexFields::TIMESTAMP:
    case IndexFields::KARMA:
      return IndexKey(value);
    case IndexFields::USER:
      return IndexKey(value);
    case IndexFields::END:
    default:
      throw invalid_argument("Not implemented");
  }
}

void Database::Index::Put(const Record& record) {
  for (size_t i = 0; i < static_cast<size_t>(IndexFields::END); i++) {
    index[i].insert(
        { CreateIndexKey(record, static_cast<IndexFields>(i)), record.id }
        );
  }
}

void Database::Index::Erase(const Record& record) {
  for (size_t i = 0; i < static_cast<size_t>(IndexFields::END); i++) {
    auto [begin, end] = index[i].equal_range(
        CreateIndexKey(record, static_cast<IndexFields>(i)));
    for (auto it = begin; it != end; it++) {
      if (it->second == record.id) {
        index[i].erase(it);
        break;
      }
    }
  }
}

bool Database::Put(const Record& record) {
  if (records.count(record.id) > 0) {
    return false;
  }
  records[record.id] = record;
  index.Put(record);
  return true;
}

const Record* Database::GetById(const string& id) const {
  auto it = records.find(id);
  if (it == records.end()) {
    return nullptr;
  }
  return &(it->second);
}

bool Database::Erase(const string& id) {
  auto it = records.find(id);
  if (it == records.end()) {
    return false;
  }
  index.Erase(it->second);
  records.erase(it);
  return true;
}

template <typename Callback>
void Database::RangeByIndexField(IndexFields field, int low, int high, Callback callback) const {
  if (low <= high) {
    auto current_index = index.Get(field);
    auto first = current_index.lower_bound(Index::CreateIndexKey(low, field));
    auto last = current_index.upper_bound(Index::CreateIndexKey(high, field));
    for (auto it = first; it != last && callback(records.at(it->second)); it++);
  }
}

template <typename Callback>
void Database::AllByIndexField(IndexFields field, const string& value, Callback callback) const {
  auto current_index = index.Get(field);
  auto [first, last] = current_index.equal_range(Index::CreateIndexKey(value, field));
  for (auto it = first; it != last && callback(records.at(it->second)); it++);
}

template <typename Callback>
void Database::RangeByTimestamp(int low, int high, Callback callback) const {
  RangeByIndexField(IndexFields::TIMESTAMP, low, high, callback);
}

template <typename Callback>
void Database::RangeByKarma(int low, int high, Callback callback) const {
  RangeByIndexField(IndexFields::KARMA, low, high, callback);
}

template <typename Callback>
void Database::AllByUser(const string& user, Callback callback) const {
  AllByIndexField(IndexFields::USER, user, callback);
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

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestRangeBoundaries);
  RUN_TEST(tr, TestSameUser);
  RUN_TEST(tr, TestReplacement);
  return 0;
}

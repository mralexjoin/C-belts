#include "test_runner.h"

#include <algorithm>
#include <forward_list>
#include <iterator>

using namespace std;

template <typename Type, typename Hasher>
class HashSet {
public:
  using BucketList = forward_list<Type>;

public:
  explicit HashSet(
      size_t num_buckets,
      const Hasher& hasher = {}
  );

  void Add(const Type& value);
  bool Has(const Type& value) const;
  void Erase(const Type& value);
  const BucketList& GetBucket(const Type& value) const;

private:
  size_t GetIndex(const Type& value) const;
  vector<BucketList> buckets;
  Hasher hasher;
};

template <typename Type, typename Hasher>
size_t HashSet<Type, Hasher>::GetIndex(const Type& value) const {
  return hasher(value) % buckets.size();
}

template <typename Type, typename Hasher>
HashSet<Type, Hasher>::HashSet(size_t num_backets, const Hasher& hasher) :
    buckets(num_backets),
    hasher(hasher) { }

template <typename Type, typename Hasher>
const typename HashSet<Type, Hasher>::BucketList& 
    HashSet<Type, Hasher>::GetBucket(const Type& value) const {
  return buckets[GetIndex(value)];
}

template <typename Type, typename Hasher>
void HashSet<Type, Hasher>::Add(const Type& value) {
  BucketList& bucket_list = buckets[GetIndex(value)];
  auto it = find(bucket_list.begin(), bucket_list.end(), value);
  if (it == bucket_list.end()) {
    bucket_list.push_front(value);
  }
}

template <typename Type, typename Hasher>
bool HashSet<Type, Hasher>::Has(const Type& value) const {
  const BucketList& bucket_list = buckets[GetIndex(value)];
  return find(bucket_list.begin(), bucket_list.end(), value) != bucket_list.end();
}

template <typename Type, typename Hasher>
void HashSet<Type, Hasher>::Erase(const Type& value) {
  buckets[GetIndex(value)].remove(value);
}

struct IntHasher {
  size_t operator()(int value) const {
    // Это реальная хеш-функция из libc++, libstdc++.
    // Чтобы она работала хорошо, std::unordered_map
    // использует простые числа для числа бакетов
    return value;
  }
};

struct TestValue {
  int value;

  bool operator==(TestValue other) const {
    return value / 2 == other.value / 2;
  }
};

struct TestValueHasher {
  size_t operator()(TestValue value) const {
    return value.value / 2;
  }
};

void TestSmoke() {
  HashSet<int, IntHasher> hash_set(2);
  hash_set.Add(3);
  hash_set.Add(4);

  ASSERT(hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(!hash_set.Has(5));

  hash_set.Erase(3);

  ASSERT(!hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(!hash_set.Has(5));

  hash_set.Add(3);
  hash_set.Add(5);

  ASSERT(hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(hash_set.Has(5));
}

void TestEmpty() {
  HashSet<int, IntHasher> hash_set(10);
  for (int value = 0; value < 10000; ++value) {
    ASSERT(!hash_set.Has(value));
  }
}

void TestIdempotency() {
  HashSet<int, IntHasher> hash_set(10);
  hash_set.Add(5);
  ASSERT(hash_set.Has(5));
  hash_set.Add(5);
  ASSERT(hash_set.Has(5));
  hash_set.Erase(5);
  ASSERT(!hash_set.Has(5));
  hash_set.Erase(5);
  ASSERT(!hash_set.Has(5));
}

void TestEquivalence() {
  HashSet<TestValue, TestValueHasher> hash_set(10);
  hash_set.Add(TestValue{2});
  hash_set.Add(TestValue{3});

  ASSERT(hash_set.Has(TestValue{2}));
  ASSERT(hash_set.Has(TestValue{3}));

  const auto& bucket = hash_set.GetBucket(TestValue{2});
  const auto& three_bucket = hash_set.GetBucket(TestValue{3});
  ASSERT_EQUAL(&bucket, &three_bucket);

  ASSERT_EQUAL(1, distance(begin(bucket), end(bucket)));
  ASSERT_EQUAL(2, bucket.front().value);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSmoke);
  RUN_TEST(tr, TestEmpty);
  RUN_TEST(tr, TestIdempotency);
  RUN_TEST(tr, TestEquivalence);
  return 0;
}

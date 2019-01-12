#include "test_runner.h"
#include "profile.h"

#include <future>
#include <map>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>
using namespace std;

template <typename Iterator>
class Page {
public:
  Page(Iterator begin, Iterator end) :
    first(begin),
    last(end),
    page_size(distance(begin, end)) {}
  Iterator begin() {
    return first;
  }
  auto end() {
    return last;
  }
  Iterator begin() const {
    return first;
  }
  Iterator end() const {
    return last;
  }
  size_t size() const {
    return page_size;
  }
private:
  Iterator first, last;
  const size_t page_size;
};

template <typename Iterator>
class Paginator {
public:
  Paginator(Iterator begin, Iterator end, const size_t page_size) {
    auto it = begin;
    while (it != end) {
      auto prev = it;
      size_t i = 0;
      while (i < page_size && it != end) {
        it = next(it);
        i++;
      }
      pages.push_back({prev, it});
    }
  }
  auto begin() {
    return pages.begin();
  }
  auto end() {
    return pages.end();
  }
  size_t size() const {
    return pages.size();
  }
private:
  vector<Page<Iterator>> pages;
};

template <typename C>
auto Paginate(C& c, size_t page_size) {
  return Paginator(c.begin(), c.end(), page_size);
}

struct Stats {
  map<string, int> word_frequences;

  void operator += (const Stats& other) {
    for (const auto& [key, value] : other.word_frequences) {
      word_frequences[key] += value;
    }
  }
};

Stats ExploreLine(const set<string>& key_words, const string& line) {
  constexpr char space = ' ';
  Stats stats;

  string_view line_view(line);
  size_t pos = 0;
  while (pos != line_view.npos) {
    size_t word_start = line_view.find_first_not_of(space, pos);
    if (word_start == line_view.npos) {
      break;
    }
    size_t word_end = line_view.find(space, word_start);
    string word = string(line_view.substr(word_start, word_end - word_start));
    if (key_words.count(word) > 0) {
      stats.word_frequences[move(word)]++;
    }
    pos = word_end;
  }

  return stats;
}

Stats ExploreKeyWordsSingleThread(
  const set<string>& key_words, istream& input
) {
  Stats result;
  for (string line; getline(input, line); ) {
    result += ExploreLine(key_words, line);
  }
  return result;
}

Stats ExploreKeyWords(const set<string>& key_words, istream& input) {
  vector<string> lines;
  for (string line; getline(input, line); ) {
    lines.push_back(move(line));
  }

  auto paginator = Paginate(lines, 2000);
  vector<future<Stats>> futures;
  for (const auto& page : paginator) {
    futures.push_back(async([&key_words, &page] {
                              Stats result;
                              for (const string& line : page) {
                                result += ExploreLine(key_words, line);
                              }
                              return result;
                            })
                      );
  }

  Stats result;
  for (auto& f : futures) {
    result += f.get();
  }
  return result;
}

void TestBasic() {
  const set<string> key_words = {"yangle", "rocks", "sucks", "all"};

  stringstream ss;
  ss << "this new yangle service really rocks\n";
  ss << "It sucks when yangle isn't available\n";
  ss << "10 reasons why yangle is the best IT company\n";
  ss << "yangle rocks others suck\n";
  ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

  const auto stats = ExploreKeyWords(key_words, ss);
  const map<string, int> expected = {
    {"yangle", 6},
    {"rocks", 2},
    {"sucks", 1}
  };
  ASSERT_EQUAL(stats.word_frequences, expected);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestBasic);
}

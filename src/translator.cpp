#include <iostream>
#include <set>
#include <map>
#include <string>
#include <string_view>
#include "test_runner.h"

using namespace std;

class Translator {
public:
  void Add(const string_view source, const string_view target) {
    const string_view new_source = GetUniqueStringView(source);
    const string_view new_target = GetUniqueStringView(target);
    forward[new_source] = new_target;
    backward[new_target] = new_source;
  }
  string_view TranslateForward(const string_view source) const {
    return Translate(forward, source);
  }
  string_view TranslateBackward(const string_view target) const {
    return Translate(backward, target);
  }

private:
  typedef map<string_view, string_view> Words;

  string_view GetUniqueStringView(const string_view view) {
    return *(words.emplace(string(view)).first);
  }

  string_view Translate(const Words& words, const string_view word) const {
    if (auto it = words.find(word); it == words.end()) {
      return "";
    }
    else {
      return it->second;
    }
  }

  set<string> words;
  Words forward, backward;
};

void TestSimple() {
  Translator translator;
  translator.Add(string("okno"), string("window"));
  translator.Add(string("stol"), string("table"));
  translator.Add(string("stol"), string("table1"));

  ASSERT_EQUAL(translator.TranslateForward("okno"), "window");
  ASSERT_EQUAL(translator.TranslateBackward("table1"), "stol");
  ASSERT_EQUAL(translator.TranslateForward("stol"), "table1");
  ASSERT_EQUAL(translator.TranslateBackward(""), "");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSimple);
  return 0;
}

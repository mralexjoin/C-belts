#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> SplitIntoWords(const string& s) {
  vector<string> words;
  for (auto start_of_word = begin(s); start_of_word != end(s);) {
    auto end_of_word = find(start_of_word, end(s), ' ');
    words.push_back({start_of_word, end_of_word});
    start_of_word = end_of_word;
    if (start_of_word != end(s))
      start_of_word++;
  }
  return words;
}

int main() {
  string s = "C Cpp Java Python";

  vector<string> words = SplitIntoWords(s);
  cout << words.size() << " ";
  for (auto it = begin(words); it != end(words); ++it) {
    if (it != begin(words)) {
      cout << "/";
    }
    cout << *it;
  }
  cout << endl;

  return 0;
}

#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Learner {
 private:
  set<string> dict;

 public:
  int Learn(const vector<string>& words) {
    size_t size = dict.size();
    dict.insert(words.begin(), words.end());
    return dict.size() - size;
  }

  vector<string> KnownWords() const {
    return {dict.begin(), dict.end()};
  }
};

int main() {
  Learner learner;
  string line;
  while (getline(cin, line)) {
    vector<string> words;
    stringstream ss(line);
    string word;
    while (ss >> word) {
      words.push_back(word);
    }
    cout << learner.Learn(words) << "\n";
  }
  cout << "=== known words ===\n";
  for (auto word : learner.KnownWords()) {
    cout << word << "\n";
  }
  return 0;
}

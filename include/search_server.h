#pragma once

#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <string_view>
using namespace std;

class InvertedIndex {
public:
  void Add(string& document);
  bool WordIndexed(const string word) const {
    return index.count(word) > 0;
  }
  const map<size_t, size_t>& Lookup(const string& word) const {
    return index.at(word);
  }

  const string& GetDocument(size_t id) const {
    return docs[id];
  }

private:
  vector<string> docs;
  map<string, map<size_t, size_t>> index;
};

class SearchServer {
public:
  static constexpr char SPACE = ' ';
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
  InvertedIndex index;
};

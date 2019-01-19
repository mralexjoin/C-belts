#pragma once

#include <future>
#include <cstdint>
#include <istream>
#include <ostream>
#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>
using namespace std;

class InvertedIndex {
public:
  void Add(string& current_document);
  const vector<pair<uint32_t, uint32_t>>& Lookup(const string& word) const;
  uint32_t Size() const {
    return size;
  }

private:
  map<string, vector<pair<uint32_t, uint32_t>>> index;
  vector<pair<uint32_t, uint32_t>> empty_index;
  uint32_t size = 0;
};

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
  uint32_t RESULTS_SIZE = 5;
  mutex m;
  InvertedIndex index;
  void SingleQuery(string& current_query, 
                   array<pair<uint32_t, uint32_t>, 50'000>& docid_count) const;
};

#pragma once

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
  const map<uint32_t, uint32_t>& Lookup(const string& word) const;
  uint32_t Size() const {
    return size;
  }

private:
  map<string, map<uint32_t, uint32_t>> index;
  map<uint32_t, uint32_t> empty_index;
  uint32_t size = 0;
};

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);
  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);

private:
  InvertedIndex index;
  auto SingleQuery(string& query);
};

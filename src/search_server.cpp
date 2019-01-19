#include "search_server.h"
#include "iterator_range.h"
//#include "profile.h"

#include <algorithm>
#include <array>
#include <future>
#include <iterator>
#include <sstream>
#include <iostream>
#include <queue>

//TotalDuration parse("Total parse");

map<string, uint32_t> SplitIntoWords(string& line) {
  istringstream iss(line);
  map<string, uint32_t> unique_words;
  for (string& word : vector<string>{istream_iterator<string>(iss), istream_iterator<string>()}) {
    unique_words[move(word)]++;
  }
  return unique_words;
}

void InvertedIndex::Add(string& current_document) {
  for (auto& [word, count] : SplitIntoWords(current_document)) {
    index[move(word)][size] += count;
  }
  size++;
}

const map<uint32_t, uint32_t>& InvertedIndex::Lookup(const string& word) const {
  auto it = index.find(word);
  if (it != index.end()) {
    return it->second;
  }
  return empty_index;
}

SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  InvertedIndex new_index;

  for (string current_document; getline(document_input, current_document); ) {
    new_index.Add(current_document);
  }

  index = move(new_index);
}

void SearchServer::AddQueriesStream(
  istream& query_input, ostream& search_results_output
) {
  constexpr size_t RESULTS_SIZE = 5;
  constexpr auto COMPARATOR = 
                [] (const pair<uint32_t, uint32_t>& lhs, const pair<uint32_t, uint32_t>& rhs) {
                   return lhs.second == rhs.second ? lhs.first < rhs.first : lhs.second > rhs.second;
                 };
  vector<pair<uint32_t, uint32_t>> docid_count;
  for (string current_query; getline(query_input, current_query); ) {
    docid_count.assign(index.Size(), {0, 0});
    for (const auto& [word, count] : SplitIntoWords(current_query)) {
      for (const auto& [docid, hits] : index.Lookup(word)) {
        docid_count[docid].first = docid;
        docid_count[docid].second += count * hits;
      }
    }

    if (docid_count.size() < RESULTS_SIZE) {
      sort(docid_count.begin(), docid_count.end(), COMPARATOR);
    }
    else {
      partial_sort(docid_count.begin(),
                   docid_count.begin() + RESULTS_SIZE, 
                   docid_count.end(),
                   COMPARATOR);
    }
    search_results_output << current_query << ":";
    for (uint32_t i = 0; i < min(RESULTS_SIZE, docid_count.size()); i++) {
      if (docid_count[i].second == 0) {
        break;
      }
      search_results_output << " {"
                            << "docid: " << docid_count[i].first << ", "
                            << "hitcount: " << docid_count[i].second << '}';
    }
    search_results_output << endl;
  }
}

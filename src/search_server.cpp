#include "search_server.h"
#include "iterator_range.h"
#include "profile.h"

#include <algorithm>
#include <array>
#include <future>
#include <iterator>
#include <sstream>
#include <iostream>
#include <queue>

// TotalDuration assign_docid_count("Assign docid_count");
// TotalDuration sort_partial("Partial sort");

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
    index[move(word)].push_back({ size, count });
  }
  size++;
}

const vector<pair<uint32_t, uint32_t>>& InvertedIndex::Lookup(const string& word) const {
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

  lock_guard guard(m);
  index = move(new_index);
}

void SearchServer::SingleQuery(
  string& current_query, array<pair<uint32_t, uint32_t>, 50'000>& docid_count) const {
  constexpr auto COMPARATOR = 
                [] (const pair<uint32_t, uint32_t>& lhs, const pair<uint32_t, uint32_t>& rhs) {
                  if (lhs.second > rhs.second) {
                    return true;
                  }
                  return lhs.second == rhs.second && lhs.first < rhs.first;
                 };
  for (const auto& [word, count] : SplitIntoWords(current_query)) {
    for (const auto& [docid, hits] : index.Lookup(word)) {
      docid_count[docid].first = docid;
      docid_count[docid].second += count * hits;
    }
  }

  if (RESULTS_SIZE >= index.Size()) {
    sort(docid_count.begin(), docid_count.end(), COMPARATOR);
  }
  else {
    partial_sort(docid_count.begin(),
                docid_count.begin() + RESULTS_SIZE,
                docid_count.begin() + index.Size(),
                COMPARATOR);
  }
}

void SearchServer::AddQueriesStream(
  istream& query_input, ostream& search_results_output
) {
  vector<string> queries;
  for (string current_query; getline(query_input, current_query); ) {
    queries.push_back(current_query);
  }

  vector<future<string>> futures;
  for (auto& page : Paginate(queries, 2'000)) {
    futures.push_back(async([this, page]() {
      ostringstream ss;
      array<pair<uint32_t, uint32_t>, 50'000> docid_count;
      for (string& current_query : page) {
        docid_count.fill({0, 0});
        {
          lock_guard guard(m);
        }
        SingleQuery(current_query, docid_count);
        ss << current_query << ":";
        for (uint32_t i = 0; i < min(RESULTS_SIZE, index.Size()); i++) {
          if (docid_count[i].second == 0) {
            break;
          }
          ss << " {"
            << "docid: " << docid_count[i].first << ", "
            << "hitcount: " << docid_count[i].second << '}';
        }
        ss << endl;        
      }
      return ss.str();
    }));
  }

  for (auto& f : futures) {
    search_results_output << f.get();
  }
}

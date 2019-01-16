#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <sstream>
#include <iostream>
#include <queue>
#include <list>

list<string> SplitIntoWords(string_view line) {
  list<string> words;
  while (!line.empty()) {
    const size_t word_start = line.find_first_not_of(SearchServer::SPACE);
    if (word_start == line.npos) {
      break;
    }
    if (word_start > 0) {
      line.remove_prefix(word_start);
    }
    const size_t word_end = line.find(SearchServer::SPACE);
    string_view word = line.substr(0, word_end);
    words.push_back(string(word));
    if (word_end == line.npos) {
      break;
    }
    line.remove_prefix(word_end);
  }
  return words;
}

void InvertedIndex::Add(string& document) {
  docs.push_back(move(document));

  const size_t docid = docs.size() - 1;
  for (string& word : SplitIntoWords(docs[docid])) {
    index[move(word)][docid]++;
  }
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
  for (string current_query; getline(query_input, current_query); ) {
    map<size_t, size_t> docid_count;

    for (const string& word : SplitIntoWords(current_query)) {
      if (index.WordIndexed(word)) {
        for (const auto& [docid, hits] : index.Lookup(word)) {
          docid_count[docid] += hits;
        }
      }
    }

    auto comparator = [](const pair<size_t, size_t>& lhs,
                         const pair<size_t, size_t>& rhs) {
                        if (lhs.second == rhs.second) {
                          return lhs.first > rhs.first;
                        }
                        return lhs.second < rhs.second;
                      };
    vector<pair<size_t, size_t>> queue_container;
    queue_container.reserve(docid_count.size());
    priority_queue<pair<size_t, size_t>,
                   vector<pair<size_t, size_t>>,
                   decltype(comparator)>
      search_results(move_iterator(docid_count.begin()),
                     move_iterator(docid_count.end()),
                     comparator,
                     move(queue_container));

    search_results_output << current_query << ':';
    for (size_t i = 0; i < 5 && !search_results.empty(); i++) {
      pair<size_t, size_t> item = search_results.top();
      search_results_output << " {"
                            << "docid: " << item.first << ", "
                            << "hitcount: " << item.second << '}';
      search_results.pop();
    }
    search_results_output << endl;
  }
}

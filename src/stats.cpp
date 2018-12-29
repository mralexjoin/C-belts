#include "stats.h"
using namespace std;

void Stats::AddMethod(string_view method) {
  auto it = method_stats.find(method);
  if (it == method_stats.end()) {
    it = unknown_method;
  }
  it->second++;
}
void Stats::AddUri(string_view uri) {
  auto it = uri_stats.find(uri);
  if (it == uri_stats.end()) {
    it = unknown_uri;
  }
  it->second++;
}
const map<string_view, int>& Stats::GetMethodStats() const {
  return method_stats;
}
const map<string_view, int>& Stats::GetUriStats() const {
  return uri_stats;
}

HttpRequest ParseRequest(string_view line) {
  const char SPACE = ' ';

  size_t pos = line.find_first_not_of(SPACE, 0);
  HttpRequest request;
  for (auto* field_ptr : {&request.method, &request.uri, &request.protocol}) {
    size_t new_pos = line.find(SPACE, pos);
    *field_ptr = line.substr(pos, new_pos - pos);
    pos = line.find_first_not_of(SPACE, new_pos);
  }

  return request;
}

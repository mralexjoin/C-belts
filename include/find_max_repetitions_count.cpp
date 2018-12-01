#include <algorithm>
#include <map>
#include <vector>
//#include "region.cpp"

bool operator<(const Region& lhs, const Region& rhs) {
  return tie(lhs.std_name, lhs.parent_std_name, lhs.names, lhs.population)
    < tie(rhs.std_name, rhs.parent_std_name, rhs.names, rhs.population);
}

int FindMaxRepetitionCount(const std::vector<Region>& regions) {
  if (regions.empty())
    return 0;
  map<Region, int> counter;
  for (const auto& region : regions)
    counter[region]++;
  return max_element(begin(counter), end(counter),
                     [](const pair<Region, int>& lhs, const pair<Region, int>& rhs) {
                       return lhs.second < rhs.second;
                     })->second;
}

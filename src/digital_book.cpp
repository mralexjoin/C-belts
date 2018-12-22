#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <vector>
#include <utility>

using namespace std;

class ReadingManager {
public:
  ReadingManager() {}

  void Read(int user_id, int page_count) {
    if (users_page_counts_.count(user_id)) {
      const int old_page = users_page_counts_[user_id];
      set<int>& users_on_old_page = users_on_pages_[old_page];
      users_on_old_page.erase(user_id);
      if (users_on_old_page.size() == 0) {
        users_on_pages_.erase(old_page);
      }
    }
    users_page_counts_[user_id] = page_count;
    users_on_pages_[page_count].insert(user_id);
  }

  double Cheer(int user_id) const {
    if (users_page_counts_.count(user_id) == 0) {
      return 0;
    }
    if (users_page_counts_.size() == 1) {
      return 1;
    }
    const int page_count = users_page_counts_.at(user_id);
    auto lower_bound = users_on_pages_.lower_bound(page_count);
    if (lower_bound == users_on_pages_.begin()) {
      return 0;
    }
    if (lower_bound == users_on_pages_.end()) {
      return 1;
    }

    size_t user_count = accumulate(users_on_pages_.begin(),
                                   lower_bound,
                                   0u,
                                   [] (const size_t user_count, const pair<const int, set<int>>& users_on_page) {
                                     return user_count + users_on_page.second.size();
                                   });
    // По умолчанию деление целочисленное, поэтому
    // нужно привести числитель к типу double.
    // Простой способ сделать это — умножить его на 1.0.
    return user_count * 1.0 / (users_page_counts_.size() - 1);
  }

private:
  map<int, int> users_page_counts_;
  map<int, set<int>> users_on_pages_;
};


int main() {
  // Для ускорения чтения данных отключается синхронизация
  // cin и cout с stdio,
  // а также выполняется отвязка cin от cout
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  ReadingManager manager;

  int query_count;
  cin >> query_count;

  for (int query_id = 0; query_id < query_count; ++query_id) {
    string query_type;
    cin >> query_type;
    int user_id;
    cin >> user_id;

    if (query_type == "READ") {
      int page_count;
      cin >> page_count;
      manager.Read(user_id, page_count);
    } else if (query_type == "CHEER") {
      cout << setprecision(6) << manager.Cheer(user_id) << "\n";
    }
  }

  return 0;
}

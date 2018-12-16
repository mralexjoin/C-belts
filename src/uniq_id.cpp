#include <string>
#include <vector>
using namespace std;

#define CONCAT2(prefix, suffix) prefix ## suffix
#define CONCAT(prefix, suffix) CONCAT2(prefix, suffix)
#define UNIQ_ID CONCAT(_, __LINE__)

int main() {
  int UNIQ_ID = 0;
  string UNIQ_ID = "hello";
  vector<string> UNIQ_ID = {"hello", "world"};
  vector<int> UNIQ_ID = {1, 2, 3, 4};
}

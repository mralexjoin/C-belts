#include <exception>
#include <sstream>
#include <string>

void EnsureEqual(const std::string& left, const std::string& right) {
  if (left != right) {
    std::stringstream stream;
    stream << left << " != " << right;
    throw std::runtime_error(stream.str());
  }
}

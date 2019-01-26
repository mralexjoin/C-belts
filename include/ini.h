#pragma once

#include <istream>
#include <string>
#include <unordered_map>

namespace Ini {

using Section = std::unordered_map<std::string, std::string>;

class Document {
public:
  inline Section& AddSection(std::string name) {
    return sections[name];
  }
  inline const Section& GetSection(const std::string& name) const {
    return sections.at(name);
  }
  std::size_t SectionCount() const {
    return sections.size();
  }

private:
  std::unordered_map<std::string, Section> sections;
};

Document Load(std::istream& input);

}
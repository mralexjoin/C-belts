#include "ini.h"

#include <string_view>

namespace Ini {

std::string_view LStrip(std::string_view line) {
  std::size_t start = line.find_first_not_of(' ');
  if (start > 0) {
    line.remove_prefix(start == line.npos ? line.size() : start - 1);
  }
  return line;
};

std::string ReadSection(std::string_view line) {
  line.remove_prefix(1);
  std::size_t end = line.find(']');
  if (end != line.npos) {
    line.remove_suffix(line.size() - end);
  }
  return std::string(line);
}

std::pair<std::string, std::string> ReadKeyValue(std::string_view line) {
  std::size_t splitter_pos = line.find('=');
  std::string key(line.substr(0, splitter_pos));
  line.remove_prefix(splitter_pos + 1);
  return {move(key), std::string(line)};
}

Document Load(std::istream& input) {
  Document document;
  Section* section;
  bool section_readed = false;
  for (std::string line; std::getline(input, line); ) {
    std::string_view line_view = LStrip(line);
    if (!line_view.empty()) {
      if (line_view[0] == '[') {
        section = &(document.AddSection(ReadSection(line_view)));
        section_readed = true;
      }
      else {
        if (!section_readed) {
          section = &(document.AddSection(""));
        }
        section->insert(ReadKeyValue(line_view));
      }
    }
  }
  return document;
}

}

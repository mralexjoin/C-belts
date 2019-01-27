#include "test_runner.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <numeric>
#include <unordered_map>
#include <sstream>

using namespace std;

enum class Gender {
  MALE,
  FEMALE,
  END
};

istream& operator>>(istream& input, Gender& gender) {
  constexpr char MALE_CHAR = 'M';
  char gender_char;
  input >> gender_char;
  gender = gender_char == MALE_CHAR ? Gender::MALE : Gender::FEMALE;
  return input;
}

ostream& operator<<(ostream& output, const Gender& gender) {
  switch (gender) {
    case Gender::MALE:
      output << 'M';
      break;
    case Gender::FEMALE:
      output << 'W';
      break;
    case Gender::END:
      throw invalid_argument("Not implemented");
  }
  return output;
}

class People {
public:
  int64_t UpperAge(int age) {
    return upper_age_count[age];
  }
  int64_t Wealhiest(size_t count) {
    return max_income[count >= max_income.size() ? max_income.size() - 1 : count - 1];
  }
  const string& MostPopularName(Gender gender) {
    return popular_names[static_cast<size_t>(gender)];
  }

private:
  struct Person {
    string name;
    int age, income;
    Gender gender;
    friend istream& operator>>(istream& input, People::Person& person);
  };

  vector<Person> persons;
  vector<int64_t> upper_age_count;
  vector<int64_t> max_income;
  array<string, static_cast<size_t>(Gender::END)> popular_names;

  friend istream& operator>>(istream& input, People::Person& person);
  friend istream& operator>>(istream& input, People& people);
  void UpdateStats();
};

istream& operator>>(istream& input, People::Person& person) {
  return input >> person.name >> person.age >> person.income >> person.gender;
}

istream& operator>>(istream& input, People& people) {
  size_t count;
  input >> count;

  people.persons.resize(count);

  for (People::Person& person : people.persons) {
    input >> person;
  }

  people.UpdateStats();
  return input;
}

void People::UpdateStats() {
  map<int, int64_t> age_counter;
  for (const Person& person : persons) {
    age_counter[person.age]++;
  }
  if (!age_counter.empty()) {
    upper_age_count.resize(age_counter.rbegin()->first + 1, 0);
  }
  int64_t sum = 0;
  for (int64_t i = upper_age_count.size() - 1; i >= 0; i--) {
    sum += age_counter[i];
    upper_age_count[i] = sum;
  }
  
  sort(persons.begin(), persons.end(), [] (const Person& lhs, const Person& rhs) {
    return lhs.income > rhs.income;
  });
  sum = 0;
  max_income.resize(persons.size(), 0);
  for (size_t i = 0; i < persons.size(); i++) {
    sum += persons[i].income;
    max_income[i] = sum;
  }

  array<unordered_map<string_view, size_t>, static_cast<size_t>(Gender::END)> names;
  for (const Person& person : persons) {
    names[static_cast<size_t>(person.gender)][person.name]++;
  }

  for (size_t i = 0; i < static_cast<size_t>(Gender::END); i++) {
    auto it = max_element(names[i].begin(), 
                          names[i].end(), 
                          [](const pair<string_view, size_t>& lhs, 
                             const pair<string_view, size_t>& rhs){
      if (lhs.second < rhs.second) {
        return true;
      }
      if (rhs.first > rhs.first) {
        return true;
      }
      return false;
    });
    if (it != names[i].end()) {
      popular_names[i] = string(it->first);
    }
  }
}

enum class Commands {
  AGE,
  WEALTHY,
  POPULAR_NAME
};

istream& operator>>(istream& input, Commands& command) {
  string str_command;
  input >> str_command;
  if (str_command == "AGE") {
    command = Commands::AGE;
  }
  else if (str_command == "WEALTHY") {
    command = Commands::WEALTHY;
  }
  else if (str_command == "POPULAR_NAME") {
    command = Commands::POPULAR_NAME;
  }
  return input;
}

void PrintStats(istream& input, ostream& output) {
  People people;
  input >> people;

  for (Commands command; input >> command; ) {
    switch (command) {
      case Commands::AGE:
        int adult_age;
        input >> adult_age;
        output << "There are " << people.UpperAge(adult_age) 
            << " adult people for maturity age "
            << adult_age << '\n';
        break;
      case Commands::WEALTHY:
        size_t count;
        input >> count;
        output << "Top-" << count << " people have total income "
             << people.Wealhiest(count) << '\n';
      break;
      case Commands::POPULAR_NAME:
        Gender gender;
        input >> gender;
        const string& name = people.MostPopularName(gender);
        if (name.empty()) {
          output << "No people of gender " << gender;
        }
        else {
          output << "Most popular name among people of gender "
                 << gender << " is " << name;
        }
        output << '\n';
      break;
    }
  }
}

int main() {
  PrintStats(cin, cout);
  return 0;
}

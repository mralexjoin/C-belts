#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <experimental/ranges/ranges>
#include <experimental/ranges/algorithm>

using namespace std;
namespace rng = std::experimental::ranges::v1;
namespace view = rng::view;

enum class Gender {
  FEMALE,
  MALE
};

struct Person {
  int age;
  Gender gender;
  bool is_employed;
};

struct AgeStats {
  int total;
  int females;
  int males;
  int employed_females;
  int unemployed_females;
  int employed_males;
  int unemployed_males;
};

template <typename Range>
using range_value_t = rng::iter_value_t<rng::iterator_t<Range>>;
// range_value_t<Range> позволяет получить
// тип элементов, получаемых при итерировании по Range

template <rng::InputRange Range>
range_value_t<Range> ComputeMedian(Range&& range) {
  vector range_copy(rng::begin(range), rng::end(range));
  if (range_copy.empty()) {
    return {};
  }
  const auto middle = range_copy.begin() + range_copy.size() / 2;
  rng::nth_element(range_copy, middle);
  return *middle;
}

template <rng::InputRange Range>
int ComputeMedianAge(Range&& persons) {
  return ComputeMedian(
      persons
      | view::transform([](const Person& person) { return person.age; })
  );
}

vector<Person> ReadPersons(istream& in_stream = cin) {
  int person_count;
  in_stream >> person_count;
  vector<Person> persons;
  persons.reserve(person_count);
  for (int i = 0; i < person_count; ++i) {
    int age, gender, is_employed;
    in_stream >> age >> gender >> is_employed;
    Person person{
        age,
        static_cast<Gender>(gender),
        is_employed == 1
    };
    persons.push_back(person);
  }
  return persons;
}

auto ByGender(Gender gender) {
  return view::filter([gender](const Person& person) {
    return person.gender == gender; 
  });
}

auto ByEmployment(bool is_employed) {
  return view::filter([is_employed](const Person& person) {
    return person.is_employed == is_employed; 
  });
};

template <rng::ForwardRange Range>
AgeStats ComputeStats(Range&& persons) {
  auto females = ByGender(Gender::FEMALE);
  auto males = ByGender(Gender::MALE);
  
  auto employed = ByEmployment(true);
  auto unemployed = ByEmployment(false);

  return {
       ComputeMedianAge(persons),
       ComputeMedianAge(persons | females),
       ComputeMedianAge(persons | males),
       ComputeMedianAge(persons | females | employed),
       ComputeMedianAge(persons | females | unemployed),
       ComputeMedianAge(persons | males | employed),
       ComputeMedianAge(persons | males | unemployed)
  };
}

void PrintStats(const AgeStats& stats,
                ostream& out_stream = cout) {
  out_stream << "Median age = "
             << stats.total              << endl
             << "Median age for females = "
             << stats.females            << endl
             << "Median age for males = "
             << stats.males              << endl
             << "Median age for employed females = "
             << stats.employed_females   << endl
             << "Median age for unemployed females = "
             << stats.unemployed_females << endl
             << "Median age for employed males = "
             << stats.employed_males     << endl
             << "Median age for unemployed males = "
             << stats.unemployed_males   << endl;
}

int main() {
  PrintStats(ComputeStats(ReadPersons()));
  return 0;
}
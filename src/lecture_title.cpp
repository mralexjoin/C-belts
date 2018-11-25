#include <string>

struct Specialization {
  std::string value;
  explicit Specialization(const std::string& new_value) {
    value = new_value;
  }
};

struct Course {
  std::string value;
  explicit Course(const std::string& new_value) {
    value = new_value;
  }
};

struct Week {
  std::string value;
  explicit Week(const std::string& new_value) {
    value = new_value;
  }
};

struct LectureTitle {
  LectureTitle(const Specialization& new_specialization,
               const Course& new_course,
               const Week& new_week) {
    specialization = new_specialization.value;
    course = new_course.value;
    week = new_week.value;
  }
  std::string specialization;
  std::string course;
  std::string week;
};

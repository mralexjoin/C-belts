#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Human {
public:
  Human(const string& name, const string& profession)
    : Name(name),
      Profession(profession) {}
  string GetProfession() const {
    return Profession;
  }
  string GetName() const {
    return Name;
  }
  virtual void Walk(const string& destination) const {
    DoSomething("walks to: " + destination);
  }
protected:
  void DoSomething(const string& doing) const {
    cout << GetProfession() << ": " << GetName() << " "
         << doing << endl;
  }
private:
  const string Name;
  const string Profession;
};

class Student : public Human {
public:
  Student(const string& name, const string& favouriteSong)
    : Human(name, "Student"),
      FavouriteSong(favouriteSong) {}
  void Learn() const {
    DoSomething("learns");
  }
  void SingSong() const {
    DoSomething("sings a song: " + FavouriteSong);
  }
  void Walk(const string& destination) const override {
    Human::Walk(destination);
    SingSong();
  }

private:
  const string FavouriteSong;
};

class Teacher : public Human {
public:
  Teacher(const string& name, const string& subject) :
    Human(name, "Teacher"),
    Subject(subject) {}
  void Teach() const {
    DoSomething("teaches: " + Subject);
  }

private:
  const string Subject;
};

class Policeman : public Human {
public:
  Policeman(const string& name) :
    Human(name, "Policeman") {}
  void Check(const Human& human) {
    string doing = "checks " + human.GetProfession() + ". "
      + human.GetProfession() + "'s name is: "
      + human.GetName();
    DoSomething(doing);
  }
};

void VisitPlaces(const Human& human, vector<string> places) {
  for (const auto& p : places) {
    human.Walk(p);
  }
}

int main() {
    Teacher t("Jim", "Math");
    Student s("Ann", "We will rock you");
    Policeman p("Bob");

    VisitPlaces(t, {"Moscow", "London"});
    p.Check(s);
    VisitPlaces(s, {"Moscow", "London"});
    return 0;
}

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Person {
public:
    Person(const string& name, const string& role) : Name(name), Role(role) {}
    virtual void Walk(const string& destination) const {
        cout << Role << ": " << Name << " walks to: " << destination << "\n";
    }

    const string Name;
    const string Role;
};

class Student : public Person {
public:
    Student(const string& name, const string& favouriteSong) :
        Person(name, "Student"), FavouriteSong(favouriteSong) {}

    void Learn() const {
        cout << Role << ": " << Name << " learns\n";
    }

    void Walk(const string& destination) const override {
        Person::Walk(destination);
        SingSong();
    }

    void SingSong() const {
        cout << Role << ": " << Name << " sings a song: " << FavouriteSong << "\n";
    }

private:
    string FavouriteSong;
};


class Teacher : public Person {
public:
    Teacher(const string& name, const string& subject) :
        Person(name, "Teacher"), Subject(subject) {}

    void Teach() const {
        cout << Role << ": " << Name << " teaches: " << Subject << "\n";
    }
private:
    string Subject;
};


class Policeman : public Person {
public:
    explicit Policeman(const string& name) : Person(name, "Policeman") {}

    void Check(const Person& p) const {
        cout << Role << ": " << Name << " checks " << p.Role << ". "
            << p.Role << "'s name is: " << p.Name << "\n";
    }
};

void VisitPlaces(const Person& p, const vector<string>& places) {
    for (const auto& e : places) { p.Walk(e); }
}

int main() {
    Teacher t("Jim", "Math");
    Student s("Ann", "We will rock you");
    Policeman p("Bob");

    VisitPlaces(t, { "Moscow", "London" });
    p.Check(s);
    VisitPlaces(s, { "Moscow", "London" });
    return 0;
}

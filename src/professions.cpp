/*
 * main.cpp
 *
 *  Created on: Dec 9, 2018
 *      Author: Anton Dogadaev
 */
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Citizen {
public:
    Citizen(const string& name, const string& profession) :
        _name(name), _profession(profession) {}

    const string GetName() const {
        return _name;
    }
    const string GetProfession() const {
        return _profession;
    }

    virtual void Walk(const string& destination) const = 0;
private:
    const string _name;
    const string _profession;
};

class Student : public Citizen {
public:

    Student(const string& name, const string& favouriteSong) :
        Citizen(name, "Student"), _favouriteSong(favouriteSong) {}

    void Learn() const {
        cout << GetProfession() << ": " << GetName() << " learns" << endl;
    }

    void Walk(const string& destination) const override {
        cout << GetProfession() << ": " << GetName() << " walks to: "
             << destination << endl;
        SingSong();
    }

    void SingSong() const {
        cout << GetProfession() << ": " << GetName() << " sings a song: "
             << _favouriteSong << endl;
    }

private:
    const string _favouriteSong;
};


class Teacher : public Citizen {
public:

    Teacher(const string& name, const string& subject) :
        Citizen(name, "Teacher"), _subject(subject) {}

    void Teach() const {
        cout << GetProfession() << ": " << GetName() << " teaches: "
             << _subject << endl;
    }

    void Walk(const string& destination) const override {
        cout << GetProfession() << ": " << GetName() << " walks to: "
             << destination << endl;
    }

private:
    const string _subject;
};


class Policeman : public Citizen {
public:
    Policeman(const string& name) : Citizen(name, "Policeman") {}

    void Check(const Citizen& c) const {
        cout << GetProfession() << ": " << GetName() << " checks "
             << c.GetProfession() << ". " << c.GetProfession() << "'s name is: "
             << c.GetName() << endl;
    }

    void Walk(const string& destination) const override {
        cout << GetProfession() << ": " << GetName() << " walks to: "
             << destination << endl;
    }
};


void VisitPlaces(const Citizen& c, const vector<string>& places) {
    for (const auto& p : places) {
        c.Walk(p);
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

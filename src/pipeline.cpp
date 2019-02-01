#include "test_runner.h"
#include <forward_list>
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
  string from;
  string to;
  string body;
};

istream& operator>>(istream& input, Email& email) {
  getline(input, email.from);
  getline(input, email.to);
  getline(input, email.body);
  return input;
}

ostream& operator<<(ostream& output, Email& email) {
  return output << email.from << '\n'
                << email.to << '\n'
                << email.body << '\n';
}

class Worker {
public:
  virtual ~Worker() = default;
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run() {
    // только первому worker-у в пайплайне нужно это имплементировать
    throw logic_error("Unimplemented");
  }

protected:
  // реализации должны вызывать PassOn, чтобы передать объект дальше
  // по цепочке обработчиков
  void PassOn(unique_ptr<Email> email) const {
    if (next) {
      next->Process(move(email));
    }
  }

public:
  void SetNext(unique_ptr<Worker> next_) {
    next = move(next_);
  }

private:
  unique_ptr<Worker> next;
};


class Reader : public Worker {
public:
  Reader(istream& in) : input(in) {} 
  void Run() override {
    while (input) {
      auto email = make_unique<Email>();
      input >> *(email.get());
      if (input) {
        Process(move(email));
      }
    }
  }
  void Process(unique_ptr<Email> email) override {
    PassOn(move(email));
  }

private:
  istream& input;
};


class Filter : public Worker {
public:
  using Function = function<bool(const Email&)>;
  Filter(Function predicate) : predicate(move(predicate)) {} 
  void Process(unique_ptr<Email> email) override {
    if (predicate(*email.get())) {
      PassOn(move(email));
    }
  }

private:
  const Function predicate;
};


class Copier : public Worker {
public:
  Copier(const string& recipient) : recipient(recipient) {}
  void Process(unique_ptr<Email> email) override {
    unique_ptr<Email> new_email;
    if (email->to != recipient) {
      new_email = make_unique<Email>(*email);
      new_email->to = recipient;
    }

    PassOn(move(email));
    if (new_email) {
      PassOn(move(new_email));
    }
  }
private:
  const string recipient;
};


class Sender : public Worker {
public:
  Sender(ostream& out) : output(out) {} 
  void Process(unique_ptr<Email> email) override {
    output << *(email.get());
    PassOn(move(email));
  }

private:
  ostream& output;
};


// реализуйте класс
class PipelineBuilder {
public:
  // добавляет в качестве первого обработчика Reader
  explicit PipelineBuilder(istream& in) {
    workers.push_front(make_unique<Reader>(in));
  }

  // добавляет новый обработчик Filter
  PipelineBuilder& FilterBy(Filter::Function filter) {
    workers.push_front(make_unique<Filter>(filter));
    return *this;
  }

  // добавляет новый обработчик Copier
  PipelineBuilder& CopyTo(const string& recipient) {
    workers.push_front(make_unique<Copier>(recipient));
    return *this;
  }

  // добавляет новый обработчик Sender
  PipelineBuilder& Send(ostream& out) {
    workers.push_front(make_unique<Sender>(out));
    return *this;
  }

  // возвращает готовую цепочку обработчиков
  unique_ptr<Worker> Build() {
    auto it = workers.begin();
    for (auto next_it = next(it); next_it != workers.end(); next_it = next(next_it)) {
      (*next_it)->SetNext(move(*it));
      it = next_it;
    }
    return move(*it);
  }
private:
  forward_list<unique_ptr<Worker>> workers;
};


void TestSanity() {
  string input = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "ralph@example.com\n"
    "erich@example.com\n"
    "I do not make mistakes of that kind\n"
  );
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy([](const Email& email) {
    return email.from == "erich@example.com";
  });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "erich@example.com\n"
    "richard@example.com\n"
    "Are you sure you pressed the right button?\n"
  );

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}

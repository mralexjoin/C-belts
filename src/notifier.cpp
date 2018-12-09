#include <string>

void SendSms(const std::string& number, const std::string& message);
void SendEmail(const std::string& email, const std::string& message);

class INotifier {
public:
  virtual void Notify(const std::string& message) = 0;
};

class SmsNotifier : public INotifier {
public:
  SmsNotifier(std::string number) : number(number) {}
  void Notify(const std::string& message) override {
    SendSms(number, message);
  }
private:
  std::string number;
};

class EmailNotifier : public INotifier {
public:
  EmailNotifier(std::string email) : email(email) {}
  void Notify(const std::string& message) override {
    SendEmail(email, message);
  }
private:
  std::string email;
};

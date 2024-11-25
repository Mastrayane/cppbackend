#pragma once
#ifndef __MENU_H__
#define __MENU_H__

#include <functional>
#include <iomanip>
#include <map>
#include <sstream>

class Menu {
 public:
  using Handler = std::function<bool(std::istream&, std::ostream&)>;

  Menu(std::istream& input, std::ostream& output);
  
  void AddAction(std::string action_name, std::string args, std::string description, Handler handler);

  void Run();

  void ShowInstructions() const;

 private:
  struct ActionInfo {
    Handler handler;
    std::string args;
    std::string description;
  };

  [[nodiscard]] bool ParseCommand(std::istream& input);

  std::istream& input_;
  std::ostream& output_;
  std::map<std::string, ActionInfo> actions_;
};

#endif  // __MENU_H__
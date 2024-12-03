#include "view.h"

#include <boost/algorithm/string/trim.hpp>
#include <iostream>
#include <string>

#include "menu.h"
#include "use_cases.h"
#include "author.h"

using namespace std::literals;
namespace ph = std::placeholders;

namespace ui {

View::View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output)
    : menu_{menu}, use_cases_{use_cases}, input_{input}, output_{output} {
  menu_.AddAction(  //
      "AddAuthor"s, "name"s, "Adds author"s, std::bind(&View::AddAuthor, this, ph::_1)
      // ����
      // [this](auto& cmd_input) { return AddAuthor(cmd_input); }
  );

  menu_.AddAction(
      "AddBook"s, "year title"s, "Add book"s, std::bind(&View::AddBook, this, ph::_1));

  menu_.AddAction(
      "ShowAuthors"s, {}, "Show Authors"s, std::bind(&View::ShowAuthors, this, ph::_1));

  menu_.AddAction(
      "ShowAuthorBooks"s, {}, "Show Author Books"s, std::bind(&View::ShowAuthorBooks, this, ph::_1));

  menu_.AddAction(
      "ShowBooks"s, {}, "Show Books"s, std::bind(&View::ShowBooks, this, ph::_1));
}

bool View::AddAuthor(std::istream& cmd_input) {
  try {
    std::string name;
    std::getline(cmd_input, name);
    boost::algorithm::trim(name);
    if (name.empty()) {
      throw std::logic_error("invalid input");
    }
    use_cases_.AddAuthor(std::move(name));
  } catch (const std::exception&) {
    output_ << "Failed to add author"s << std::endl;
  }
  return true;
}

bool View::ShowAuthors(std::istream& cmd_input) {
  std::string ignore;
  std::getline(cmd_input, ignore);
  use_cases_.ShowAuthors(output_);
  return true;
}

bool View::AddBook(std::istream& cmd_input) {
  try {
    int year{0};
    std::string title;
    cmd_input >> year;
    std::getline(cmd_input, title);
    boost::algorithm::trim(title);
    if (year == 0 || title.empty()) {
      throw std::logic_error("invalid input");
    }

    // show question
    output_ << "Select author:" << std::endl;
    auto auth =  use_cases_.GetAuthors();

    for(size_t i = 0; i < auth.size(); ++i) {
      output_ << i+1 << " " << auth[i].GetName() << std::endl;
    }

    int id{0};
    output_ << "Enter author # or empty line to cancel" << std::endl;
    std::string str;
    std::getline(input_, str);
    boost::algorithm::trim(str);
    if (str.empty()) {
      // output_ << "cancel" << std::endl;
      return true;
    }

    try {
      id = std::stoi(str);
      use_cases_.AddBook(year, title, auth.at(id-1).GetId());
    } catch (const std::exception& e) {
      // output_ << e.what() << '\n';
      output_ << "Failed to add book" << std::endl;
      return true;
    }

 
  } catch (const std::exception& e) {
    output_ << "Failed to add book" << std::endl;
    // output_ << e.what() << '\n';
  }

  return true;
}

bool View::ShowAuthorBooks(std::istream& cmd_input) {
  // show question
  output_ << "Select author:" << std::endl;
  use_cases_.ShowAuthors(output_);

  int id{0};
  output_ << "Enter author # or empty line to cancel" << std::endl;
  std::string str;
  std::getline(input_, str);
  boost::algorithm::trim(str);
  if (str.empty()) {
    // output_ << "cancel" << std::endl;
    return true;
  }

  try {
    id = std::stoi(str);
  } catch (const std::exception& e) {
    // output_ << e.what() << '\n';
    return true;
  }

  use_cases_.ShowAuthorBooks(output_, id);

  return true;
}

bool View::ShowBooks(std::istream& cmd_input) {
  std::string ignore;
  std::getline(cmd_input, ignore);
  use_cases_.ShowBooks(output_);
  return true;
}

}  // namespace ui

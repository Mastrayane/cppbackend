#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include "../src/author.h"
#include "../src/book.h"
#include "../src/use_cases_impl.h"

namespace {

struct MockAuthorRepository : domain::AuthorRepository {
  std::vector<domain::Author> saved_authors;

  void Save(const domain::Author& author) override {
    saved_authors.emplace_back(author);
  }

  void ShowAuthors(std::ostream& output) override {
    for (int i = 0; i < saved_authors.size(); ++i) {
      output << i + 1 << ". " << saved_authors[i].GetName() << std::endl;
    }
  }

  std::optional<domain::AuthorId> GetAuthorIdByIndex(int index) override { return std::nullopt; }

  virtual std::vector<domain::Author> GetAuthors() override {
    return saved_authors;
  }

};

struct MockBookRepository : domain::BookRepository {
  std::vector<domain::Book> saved_books;

  void Save(const domain::Book& book) override {
    saved_books.emplace_back(book);
  }

  virtual void ShowBooks(std::ostream& ostream) override {}

  void ShowAuthorBooks(std::ostream& ostream, const domain::AuthorId& id) override {
  }
};

using namespace std::literals;

struct Fixture {
  MockAuthorRepository authors;
  MockBookRepository books;
};

}  // namespace

SCENARIO_METHOD(Fixture, "Book Adding") {
  GIVEN("Use cases") {
    app::UseCasesImpl use_cases{authors, books};
    std::stringstream ss;
    WHEN("Adding an author") {
      const auto author_name = "Joanne Rowling";
      const auto show_author = "1. Joanne Rowling"s;
      use_cases.AddAuthor(author_name);
      use_cases.ShowAuthors(ss);
      std::string out_str;
      std::getline(ss, out_str);
      THEN("author with the specified name is saved to repository") {
        REQUIRE(authors.saved_authors.size() == 1);
        CHECK(authors.saved_authors.at(0).GetName() == author_name);
        CHECK(authors.saved_authors.at(0).GetId() != domain::AuthorId{});
      }

      AND_THEN("author with the specified name is showed from repository") {
        REQUIRE(authors.saved_authors.size() == 1);
        CHECK(show_author == out_str);
      }
    }
  }
}
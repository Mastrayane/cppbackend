#pragma once
#include "author_fwd.h"
#include "book_fwd.h"
#include "use_cases.h"

namespace app {

class UseCasesImpl : public UseCases {
 public:
  explicit UseCasesImpl(domain::AuthorRepository& authors, domain::BookRepository& books)
      : authors_{authors},
        books_(books) {
  }

  void AddAuthor(const std::string& name) override;
  bool AddBook(int year, const std::string& title, domain::AuthorId id) override;

  void ShowAuthors(std::ostream &ostream) override;
  void ShowAuthorBooks(std::ostream &ostream, int id) override;
  void ShowBooks(std::ostream &ostream) override;

  std::vector<domain::Author> GetAuthors() override;


 private:
  domain::AuthorRepository& authors_;
  domain::BookRepository& books_;
};

}  // namespace app

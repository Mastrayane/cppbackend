#include "use_cases_impl.h"

#include <optional>

#include "author.h"
#include "book.h"

namespace app {
using namespace domain;

void UseCasesImpl::AddAuthor(const std::string& name) {
  authors_.Save({AuthorId::New(), name});
}

bool UseCasesImpl::AddBook(int year, const std::string& title, domain::AuthorId id) {

  if(year == 0 || title.empty()) {
    throw std::invalid_argument("wrong args for add book");
  }

  //std::optional<AuthorId> id = authors_.GetAuthorIdByIndex(author_id);


  books_.Save({BookId::New(), id, year, title});

  return true;
}

void UseCasesImpl::ShowAuthors(std::ostream &ostream) {
  authors_.ShowAuthors(ostream);
}

void UseCasesImpl::ShowAuthorBooks(std::ostream &ostream, int author_id) {
  std::optional<AuthorId> id = authors_.GetAuthorIdByIndex(author_id);
  if(id) {
    books_.ShowAuthorBooks(ostream, *id);
  }
}

void UseCasesImpl::ShowBooks(std::ostream &ostream) {
  books_.ShowBooks(ostream);
}

std::vector<domain::Author> app::UseCasesImpl::GetAuthors() {
  return authors_.GetAuthors();
}

}  // namespace app

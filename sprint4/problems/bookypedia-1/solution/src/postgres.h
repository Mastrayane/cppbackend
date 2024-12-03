#pragma once
#include <pqxx/connection>
#include <pqxx/transaction>

#include "author.h"
#include "book.h"

namespace postgres {

class AuthorRepositoryImpl : public domain::AuthorRepository {
 public:
  explicit AuthorRepositoryImpl(pqxx::work& work)
      : work_{work} {
  }

  void Save(const domain::Author& author) override;
  void ShowAuthors(std::ostream& cmd_output) override;
  std::optional<domain::AuthorId> GetAuthorIdByIndex(int index) override;
  virtual std::vector<domain::Author> GetAuthors() override;


 private:
  pqxx::work& work_;
};

class BookRepositoryImpl : public domain::BookRepository {
 public:
  explicit BookRepositoryImpl(pqxx::work& work)
      : work_{work} {
  }

  void Save(const domain::Book& book) override;
  void ShowBooks(std::ostream& ostream) override;
  void ShowAuthorBooks(std::ostream& ostream, const domain::AuthorId& id) override;

 private:
  pqxx::work& work_;
};

class Database {
 public:
  explicit Database(pqxx::connection connection);

  AuthorRepositoryImpl& GetAuthors() & {
    return authors_;
  }

  BookRepositoryImpl& GetBooks() & {
    return books_;
  }

 private:
  pqxx::connection connection_;
  pqxx::work work_{connection_};
  AuthorRepositoryImpl authors_{work_};
  BookRepositoryImpl books_{work_};
};

}  // namespace postgres
#include "postgres.h"

#include <pqxx/pqxx>
#include <pqxx/zview.hxx>

#include <iostream>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void AuthorRepositoryImpl::Save(const domain::Author& author) {
  work_.exec_params(R"(
INSERT INTO authors (id, name) VALUES ($1, $2)
ON CONFLICT (id) DO UPDATE SET name=$2;
)"_zv,
                    author.GetId().ToString(), author.GetName());
  work_.commit();
}

void AuthorRepositoryImpl::ShowAuthors(std::ostream& output) {
  pqxx::read_transaction r(work_.conn());
  int counter = 0;
  for (const auto& [name] : r.query<std::string>("SELECT name FROM authors ORDER BY name;"_zv)) {
    output << ++counter << " " << name << std::endl;
  }
  // if(counter == 0) {
  //   output << "" << std::endl;
  // } 
}

std::optional<domain::AuthorId> AuthorRepositoryImpl::GetAuthorIdByIndex(int index) {
  pqxx::read_transaction r(work_.conn());
  index--;

  auto result = r.exec("SELECT * FROM authors ORDER BY name;"_zv);
  if (index < 0 || index >= result.size()) {
    return std::nullopt;
  }

  auto row = result.at(index);
  return domain::AuthorId::FromString(row.at(0).c_str());
}

std::vector<domain::Author> postgres::AuthorRepositoryImpl::GetAuthors() {
  std::vector<domain::Author> author;

  pqxx::read_transaction r(work_.conn());
  int counter = 0;
  for (const auto& [id_str, name] : r.query<std::string, std::string>("SELECT id, name FROM authors ORDER BY name;"_zv)) {
    /// std::cout << id_str << " " <<  name << std::endl;
    auto id = domain::AuthorId::FromString(id_str);
    /// std::cout << id_str << " | " << id.ToString() << std::endl;
    author.emplace_back(id, name);
  }
  return author;
}

void BookRepositoryImpl::Save(const domain::Book& book) {
  work_.exec_params(R"(
INSERT INTO books (id, author_id, title, publication_year) VALUES ($1, $2, $3, $4)
ON CONFLICT (id) DO UPDATE SET author_id=$2, title=$3, publication_year=$4;
)"_zv,
                    book.GetId().ToString(), book.GetAuthorId().ToString(), book.GetTitle(), book.GetYear());
  work_.commit();
}

void BookRepositoryImpl::ShowBooks(std::ostream& output) {
  pqxx::read_transaction r(work_.conn());
  int counter = 1;
  for (const auto& [title, year] : r.query<std::string, int>("SELECT title, publication_year FROM books ORDER BY title;"_zv)) {
    output << counter++ << " " << title << ", " << year << std::endl;
  }
}

void BookRepositoryImpl::ShowAuthorBooks(std::ostream& output, const domain::AuthorId& id) {
  pqxx::read_transaction r(work_.conn());
  int counter = 0;
  for (const auto& [title, year] : r.query<std::string, int>("SELECT title, publication_year FROM books WHERE author_id="s + r.quote(id.ToString()) + " ORDER BY publication_year, title ASC;"s)) {
    output << ++counter << " " << title << ", " << year << std::endl;
  }
  // if(counter == 0) {
  //   output << "" << std::endl;
  // } 
}

Database::Database(pqxx::connection connection)
    : connection_{std::move(connection)} {
  work_.exec(R"(
CREATE TABLE IF NOT EXISTS authors (
    id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
    name varchar(100) UNIQUE NOT NULL
);
)"_zv);
  // ... создать другие таблицы

  work_.exec(R"(
CREATE TABLE IF NOT EXISTS books (
    id UUID CONSTRAINT book_id_constraint PRIMARY KEY,
    author_id UUID NOT NULL,
    title varchar(100) NOT NULL,
    publication_year integer NOT NULL
);
)"_zv);

  // коммитим изменения
  work_.commit();
}

}  // namespace postgres
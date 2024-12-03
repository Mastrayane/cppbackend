#include <iostream>

#include <pqxx/pqxx>
#include <boost/json/src.hpp>

using namespace std::literals;
namespace json = boost::json;

// libpqxx использует zero-terminated символьные литералы вроде "abc"_zv;
using pqxx::operator"" _zv;

int main(int argc, const char* argv[]) {
  try {
    if (argc == 1) {
      std::cout << "Usage: connect_db <conn-string>\n"sv;
      return EXIT_SUCCESS;
    } else if (argc != 2) {
      std::cerr << "Invalid command line\n"sv;
      return EXIT_FAILURE;
    }

    // Подключаемся к БД, указывая её параметры в качестве аргумента
    pqxx::connection conn{argv[1]};

    // Создаём транзакцию. Это понятие будет разобрано в следующих уроках.
    // Транзакция нужна, чтобы выполнять запросы.
    pqxx::work w(conn);

    // constants
    constexpr auto tag_add_book = "add_book"_zv;
    constexpr auto tag_all_books = "all_books"_zv;
    constexpr auto tag_exit = "exit"_zv;

    auto result_true = []() { std::cout << "{\"result\":true}" << std::endl; };
    auto result_false = []() { std::cout << "{\"result\":false}" << std::endl; };

    // Используя транзакцию создадим таблицу в выбранной базе данных:
    w.exec(
        "CREATE TABLE IF NOT EXISTS books ("
        "id SERIAL PRIMARY KEY,"
        "title varchar(100) NOT NULL,"
        "author varchar(100) NOT NULL,"
        "year integer NOT NULL,"
        "ISBN char(13) UNIQUE);"_zv);

    w.exec("DELETE FROM books;"_zv);
    w.commit();

    // prepare
    conn.prepare(tag_add_book, "INSERT INTO books (title, author, year, ISBN) VALUES ($1, $2, $3, $4)"_zv);
    // conn.prepare(tag_all_books, "SELECT * FROM books ORDER BY year DESC, ORDER BY title, ORDER BY author, ORDER BY ISBN"_zv);
    constexpr auto query_text = "SELECT * FROM books ORDER BY year DESC, title ASC, author ASC, ISBN ASC;"_zv;

    std::string input;

    while (std::getline(std::cin, input)) {
      const json::value jv = json::parse(input);
      const std::string req = std::string(jv.as_object().at("action").as_string());
      const json::object payload = jv.as_object().at("payload").as_object();
      if (tag_add_book == req) {
        // add book
        const std::string title{payload.at("title").as_string()};
        const std::string author{payload.at("author").as_string()};
        const int year = static_cast<int>(payload.at("year").as_int64());
        const auto* isbn = payload.at("ISBN").if_string();
        std::optional<std::string> isbn_opt{std::nullopt};
        if (isbn != nullptr) {
          isbn_opt = *isbn;
        }

        try {
          w.exec_prepared(tag_add_book, title, author, year, isbn_opt);
          w.commit();
        } catch (const pqxx::sql_error& e) {
          result_false();
          continue;
        }
        result_true();

      } else if (tag_all_books == req) {
        // read db
        pqxx::read_transaction r(conn);
        json::array arr;
        for (auto const& [id, title, author, year, isbn] : r.query<int, std::string, std::string, int, std::optional<std::string>>(query_text)) {
          json::object obj = {
              {"id", id},
              {"title", title},
              {"author", author},
              {"year", year},
              {"ISBN", isbn ? *isbn : "null"s}};
          arr.push_back(obj);
        }
        std::cout << json::serialize(arr) << std::endl;
      } else if (tag_exit == req) {
        // exit
        // std::cout << "Exit command" << std::endl;
        break;
      } else {
        // wrong
        break;
      }
    }

    conn.close();

  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
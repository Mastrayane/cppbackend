#ifdef WIN32
#include <sdkddkver.h>
#endif

#include "seabattle.h"

#include <atomic>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <string_view>

namespace net = boost::asio;
using net::ip::tcp;
using namespace std::literals;

void PrintFieldPair(const SeabattleField& left, const SeabattleField& right) {
    auto left_pad = "  "s;
    auto delimeter = "    "s;
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
    for (size_t i = 0; i < SeabattleField::field_size; ++i) {
        std::cout << left_pad;
        left.PrintLine(std::cout, i);
        std::cout << delimeter;
        right.PrintLine(std::cout, i);
        std::cout << std::endl;
    }
    std::cout << left_pad;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << delimeter;
    SeabattleField::PrintDigitLine(std::cout);
    std::cout << std::endl;
}

template <size_t sz>
static std::optional<std::string> ReadExact(tcp::socket& socket) {
    boost::array<char, sz> buf;
    boost::system::error_code ec;

    net::read(socket, net::buffer(buf), net::transfer_exactly(sz), ec);

    if (ec) {
        return std::nullopt;
    }

    return { {buf.data(), sz} };
}

static bool WriteExact(tcp::socket& socket, std::string_view data) {
    boost::system::error_code ec;

    net::write(socket, net::buffer(data), net::transfer_exactly(data.size()), ec);

    return !ec;
}

class SeabattleAgent {
public:
    SeabattleAgent(const SeabattleField& field)
        : my_field_(field) {
    }

    void StartGame(tcp::socket& socket, bool my_initiative) {
        bool my_turn = my_initiative;

        while (!IsGameEnded()) {
            PrintFields();

            if (my_turn) {
                // Ход игрока
                auto move = GetPlayerMove();
                if (!SendMove(socket, move)) {
                    std::cerr << "Failed to send move." << std::endl;
                    break;
                }

                auto result = ReadResult(socket);
                if (!result) {
                    std::cerr << "Failed to read result." << std::endl;
                    break;
                }

                ProcessResult(*result, move);
                my_turn = (*result != SeabattleField::ShotResult::MISS);
            }
            else {
                // Ход соперника
                auto move = ReadMove(socket);
                if (!move) {
                    std::cerr << "Failed to read move." << std::endl;
                    break;
                }

                auto result = my_field_.Shoot(move->first, move->second);
                if (!SendResult(socket, result)) {
                    std::cerr << "Failed to send result." << std::endl;
                    break;
                }

                my_turn = (result == SeabattleField::ShotResult::MISS);
            }
        }

        PrintFields();
        std::cout << "Game over." << std::endl;
    }

private:
    static std::optional<std::pair<int, int>> ParseMove(const std::string_view& sv) {
        if (sv.size() != 2) return std::nullopt;

        int p1 = sv[0] - 'A', p2 = sv[1] - '1';

        if (p1 < 0 || p1 > 7) return std::nullopt;
        if (p2 < 0 || p2 > 7) return std::nullopt;

        return { {p1, p2} };
    }

    static std::string MoveToString(std::pair<int, int> move) {
        char buff[] = { static_cast<char>(move.first) + 'A', static_cast<char>(move.second) + '1' };
        return { buff, 2 };
    }

    void PrintFields() const {
        PrintFieldPair(my_field_, other_field_);
    }

    bool IsGameEnded() const {
        return my_field_.IsLoser() || other_field_.IsLoser();
    }

    std::pair<int, int> GetPlayerMove() {
        std::string input;
        std::optional<std::pair<int, int>> move;
        do {
            std::cout << "Enter your move (e.g., A1): ";
            std::cin >> input;
            move = ParseMove(input);
        } while (!move.has_value());
        return *move;
    }

    bool SendMove(tcp::socket& socket, const std::pair<int, int>& move) {
        return WriteExact(socket, MoveToString(move));
    }

    std::optional<std::pair<int, int>> ReadMove(tcp::socket& socket) {
        auto move_str = ReadExact<2>(socket);
        if (!move_str) return std::nullopt;
        return ParseMove(*move_str);
    }

    bool SendResult(tcp::socket& socket, SeabattleField::ShotResult result) {
        char result_char = static_cast<char>(result);
        return WriteExact(socket, std::string_view(&result_char, 1));
    }

    std::optional<SeabattleField::ShotResult> ReadResult(tcp::socket& socket) {
        auto result_str = ReadExact<1>(socket);
        if (!result_str) return std::nullopt;
        return static_cast<SeabattleField::ShotResult>((*result_str)[0]);
    }

    void ProcessResult(SeabattleField::ShotResult result, const std::pair<int, int>& move) {
        switch (result) {
        case SeabattleField::ShotResult::MISS:
            other_field_.MarkMiss(move.first, move.second);
            break;
        case SeabattleField::ShotResult::HIT:
            other_field_.MarkHit(move.first, move.second);
            break;
        case SeabattleField::ShotResult::KILL:
            other_field_.MarkKill(move.first, move.second);
            break;
        }
    }

private:
    SeabattleField my_field_;
    SeabattleField other_field_;
};

void StartServer(const SeabattleField& field, unsigned short port) {
    SeabattleAgent agent(field);

    net::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    tcp::socket socket(io_context);

    std::cout << "Waiting for connection on port " << port << "..." << std::endl;
    acceptor.accept(socket);
    std::cout << "Connection established." << std::endl;

    agent.StartGame(socket, false);
}

void StartClient(const SeabattleField& field, const std::string& ip_str, unsigned short port) {
    SeabattleAgent agent(field);

    net::io_context io_context;
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(ip_str, std::to_string(port));

    std::cout << "Connecting to " << ip_str << ":" << port << "..." << std::endl;
    net::connect(socket, endpoints);
    std::cout << "Connected." << std::endl;

    agent.StartGame(socket, true);
}

int main(int argc, const char** argv) {
    if (argc != 3 && argc != 4) {
        std::cout << "Usage: program <seed> [<ip>] <port>" << std::endl;
        return 1;
    }

    std::mt19937 engine(std::stoi(argv[1]));
    SeabattleField fieldL = SeabattleField::GetRandomField(engine);

    if (argc == 3) {
        StartServer(fieldL, std::stoi(argv[2]));
    }
    else if (argc == 4) {
        StartClient(fieldL, argv[2], std::stoi(argv[3]));
    }
}
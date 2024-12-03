#include "player_list.h"

namespace model {

PlayerList::PlayerList() {
}

PlayerList::~PlayerList() {
}

std::optional<std::reference_wrapper<Player>> PlayerList::FindPlayer(const Token& token) {
  if (m_players.contains(token)) {
    return m_players.at(token);
  }
  return std::nullopt;
}

Player& PlayerList::CreatePlayer(const std::string& name, size_t id) {
  Token token = GenerateToken();
  auto pr = m_players.emplace(token, Player(token, name, Player::Id(id)));
  if (pr.second) {
    return m_players.at(token);
  }
  throw std::runtime_error("Fail to add or create Player");
}

void PlayerList::AddPlayer(Player&& player) {
  const Token token = player.GetToken();
  m_players.emplace(token, player);
}

const PlayerList::Container& PlayerList::GetContainer() const {
  return m_players;
}

}  // namespace model
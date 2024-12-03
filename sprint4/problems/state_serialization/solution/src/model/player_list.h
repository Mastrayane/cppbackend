#ifndef __PLAYER_LIST_H__
#define __PLAYER_LIST_H__

#include <memory>
#include <random>
#include <unordered_map>
#include <vector>
#include <optional>

#include "../tagged.h"
#include "player.h"

namespace model {

class PlayerList {
 public:
  PlayerList();
  ~PlayerList();

  using Container = std::unordered_map<Token, Player, TokenHasher>;

  std::optional<std::reference_wrapper<Player>> FindPlayer(const Token& token);

  Player& CreatePlayer(const std::string& name, size_t id);
  void AddPlayer(Player&& player);

  const Container& GetContainer() const;

  auto cbegin() { return m_players.cbegin(); }
  auto cend() { return m_players.cend(); }

 private:
  Container m_players;
};

}  // namespace model

#endif  // __PLAYER_LIST_H__
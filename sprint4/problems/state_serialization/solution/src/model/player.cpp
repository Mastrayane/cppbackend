#include "player.h"

namespace model {

Player::Player(const Token& token, const std::string& name, const Player::Id& id) : m_id(id), m_token(token), m_name(name), m_dog{} {
}

Player::Id Player::GetId() const noexcept {
  return m_id;
}

const std::string& Player::GetName() const noexcept {
  return m_name;
}

void Player::SetDog(DogPtr dog) {
  m_dog = dog;
  m_dog->SetToken(m_token);
}

DogPtr Player::GetDog() {
  return m_dog;
}

DogConstPtr Player::GetDog() const {
  return m_dog;
}

const Token& Player::GetToken() const {
  return m_token;
}

std::shared_ptr<GameSession> Player::GetSession() {
  return m_gsess;
}

const std::shared_ptr<GameSession> Player::GetSession() const {
  return m_gsess;
}

void Player::SetSession(std::shared_ptr<GameSession> sess) {
  m_gsess = sess;
  m_gsess->AddDog(m_dog);
}

}  // namespace model

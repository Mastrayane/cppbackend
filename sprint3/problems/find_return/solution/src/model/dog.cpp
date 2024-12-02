#include "dog.h"

namespace model {

int Dog::ID_COUNTER = 0;

Dog::Dog(const Token& token)
    : GameObject({0.0, 0.0}, 0.6),
      m_token(token),
      m_id(ID_COUNTER++),
      m_speed(0.f, 0.f),
      m_dir("U"s),
      m_default_speed(0.0),
      m_bag_size(0),
      m_points(0) {
}

int Dog::GetId() const {
  return m_id;
}

Token Dog::GetToken() const {
  return m_token;
}

const Point2d& Dog::GetSpeed() const noexcept {
  return m_speed;
}

void Dog::Stop() {
  m_speed.x = 0.0;
  m_speed.y = 0.0;
}

void Dog::SetDefaultSpeed(const double& speed) {
  m_default_speed = speed;
}

const std::string& Dog::GetDir() const noexcept {
  return m_dir;
}

bool model::Dog::IsFull() const {
  return m_bag_size == m_loots.size();
}

void Dog::AddLoot(const Loot& loot) {
  if (m_loots.size() >= m_bag_size) {
    throw std::logic_error("Bag is full");
  }
  m_loots.push_back(loot);
}

std::list<Loot> Dog::UnloadLoots() {
  return std::move(m_loots);
}

const std::list<Loot>& Dog::GetLoots() const noexcept {
  return m_loots;
}

void Dog::AddPoints(int points) {
  m_points += points;
}

const int& Dog::GetPoinst() const noexcept {
  return m_points;
}


void Dog::SetDir(const std::string& dir) {
  if (dir == "U") {
    m_speed = {0.0, -m_default_speed};
  } else if (dir == "R") {
    m_speed = {m_default_speed, 0.0};
  } else if (dir == "D") {
    m_speed = {0.0, m_default_speed};
  } else if (dir == "L") {
    m_speed = {-m_default_speed, 0.0};
  } else if (dir == "") {
    m_speed = {0.0, 0.0};
    return;
  } else {
    throw std::invalid_argument("Argumen\'"s + dir + "\' must be 'U', 'R', 'D', 'L' or \"\"."s);
  }
  m_dir = dir;
}

void Dog::SetBagSize(const int& size) {
  m_bag_size = size;
}


}  // namespace model
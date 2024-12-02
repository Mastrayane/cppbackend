#ifndef __DOG_H__
#define __DOG_H__

#include <list> 

#include "geometry.h"
#include "token_generator.h"
#include "game_object.h"
#include "loot.h"

namespace model {

using namespace std::literals;

class Dog : public GameObject {
 public:
  explicit Dog(const Token& token = Token(""s));

  int GetId() const;

  Token GetToken() const;

  /**
   * @brief get current speed
   * @return Point2d with sp
   */
  const Point2d& GetSpeed() const noexcept;

  /**
   * @brief set current speed to (0.0, 0.0)
   */
  void Stop();

  void SetDefaultSpeed(const double& speed);

  /**
   * @brief set direction as "U", "D", "L", "R", "".
   * @exception  std::invalid_argument
   */
  void SetDir(const std::string& dir);

  /**
   * @brief get direction.
   * @return std::string  with  "U", "D", "L", "R".
   */
  const std::string& GetDir() const noexcept;

  void AddLoot(const Loot& loot);

  bool IsFull() const;

  std::list<Loot> UnloadLoots();

  const std::list<Loot>& GetLoots() const noexcept;

  void AddPoints(int points);
  int GetPoinst() const noexcept;

  void SetBagSize(size_t size);

 private:
  Token m_token;
  int m_id;
  static int ID_COUNTER;

  Point2d m_speed;
  std::string m_dir;
  double m_default_speed;

  std::list<Loot> m_loots;
  size_t m_bag_size;

  int m_points;
};

}  // namespace model

#endif  // __DOG_H__
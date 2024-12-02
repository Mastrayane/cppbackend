#include "game.h"

#include <stdexcept>
#include <string>
#include <algorithm>  // Для std::find_if

namespace model {
    using namespace std::string_literals;

    void Game::AddMap(Map map) {
        // if dog speed is not set on the map
        if (map.GetDogSpeed() == 0.0) {
            map.SetDogSpeed(GetDefaultSpeed());
        }

        // if bag size not set, set default size from game
        if (map.GetBagCapacity() == 0) {
            map.SetBagCapacity(GetDefaultBagCapacity());
        }

        const size_t index = maps_.size();
        if (auto [it, inserted] = map_id_to_index_.emplace(map.GetId(), index); !inserted) {
            throw std::invalid_argument("Map with id "s + *map.GetId() + " already exists"s);
        }
        else {
            try {
                maps_.emplace_back(std::move(map));
            }
            catch (...) {
                map_id_to_index_.erase(it);
                throw;
            }
        }
    }

    std::shared_ptr<GameSession> Game::GetSession(const model::Map::Id& id) {
        auto map = FindMap(id);
        if (!map) {
            throw std::invalid_argument("Map"s + *id + "id not exist"s);
        }

        // Используем std::find_if для поиска существующей сессии
        auto sess_it = std::find_if(m_sess.begin(), m_sess.end(), [&id](const std::shared_ptr<GameSession>& sess) {
            return sess->GetMap().GetId() == id;
            });

        if (sess_it != m_sess.end()) {
            return *sess_it;
        }

        std::chrono::milliseconds ms(static_cast<int>(m_period_loot_gen * 1000));

        // Создаем новую сессию, если существующая не найдена
        auto sess = std::make_shared<GameSession>(*map, LootGenerator(ms, m_probability_loot_gen));
        sess->SetDogRandomSpawn(m_random_dog_spawn);
        m_sess.push_back(sess);

        return sess;
    }

    void Game::Update(double delta_time) {
        for (const auto& sess : m_sess) {
            sess->Update(delta_time);
        }
    }

    void Game::SetRandomSpawn(bool enable) {
        m_random_dog_spawn = enable;
    }

    bool Game::IsRandomSpawn() const {
        return m_random_dog_spawn;
    }

    void Game::SetLootGeneratorConfig(double period, double probability) {
        m_period_loot_gen = period;
        m_probability_loot_gen = probability;
    }

    void Game::SetDefaultBagCapacity(size_t size) {
        m_default_bag_capacity = size;
    }

    size_t Game::GetDefaultBagCapacity() const noexcept {
        return m_default_bag_capacity;
    }

}  // namespace model
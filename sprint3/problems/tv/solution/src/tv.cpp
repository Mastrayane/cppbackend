#include "tv.h"

/*
 * Возвращает информацию о том, включен телевизор или нет.
 */
bool TV::IsTurnedOn() const noexcept {
  return m_is_turned_on;
}

/*
 * Возвращает номер выбранного канала или std::nullopt, если телевизор выключен.
 */
std::optional<int> TV::GetChannel() const noexcept {
  return m_is_turned_on ? std::optional{m_channel} : std::nullopt;
}

/*
 * Включает телевизор, если он был выключен. Если телевизор уже включен, ничего не делает.
 * При включении выбирается тот номер канала, который был выбран перед последним выключением.
 * При самом первом включении выбирает канал #1.
 */
void TV::TurnOn() noexcept {
  m_is_turned_on = true;
}

/*
 * Выключает телевизор, если он был включен. Если телевизор уже выключен, ничего не делает.
 */
void TV::TurnOff() noexcept {
  m_is_turned_on = false;
}

/*
 * Выбирает канал channel.
 * Ранее выбранный канал запоминается и может быть восстановлен методом SelectLastViewedChannel.
 * Если номер канала совпадает с ранее выбранным каналом, метод ничего не делает.
 * Если телевизор выключен, выбрасывает исключение std::logic_error.
 * Если номер канала за пределами диапазона MIN_CHANNEL, MAX_CHANNEL, выбрасывает out_of_range.
 */
void TV::SelectChannel(int channel) {
  if (!m_is_turned_on) {
    throw std::logic_error("TV is turn off");
  }

  if (channel > MAX_CHANNEL || channel < MIN_CHANNEL) {
    throw std::out_of_range("Channel is out of range");
  }

  if (m_channel == channel) {
    return;
  }

  m_prev_channel = m_channel;
  m_channel = channel;
}

/*
 * Выбирает номер канала, который был выбран перед последним вызовом SelectChannel.
 * Многократный последовательный вызов SelectChannel переключает два последних выбранных канала.
 * Если телевизор выключен, выбрасывает исключение std::logic_error.
 */
void TV::SelectLastViewedChannel() {
  if (!m_is_turned_on) {
    throw std::logic_error("TV is turn off");
  }

  std::swap(m_channel, m_prev_channel);
}
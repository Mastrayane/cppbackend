#pragma once
#ifndef __TV_H__
#define __TV_H__

#include <cassert>
#include <optional>
#include <stdexcept>
class TV {
 public:
  constexpr static int MIN_CHANNEL = 1;
  constexpr static int MAX_CHANNEL = 99;

  /*
   * Возвращает информацию о том, включен телевизор или нет.
   */
  [[nodiscard]] bool IsTurnedOn() const noexcept;

  /*
   * Возвращает номер выбранного канала или std::nullopt, если телевизор выключен.
   */
  [[nodiscard]] std::optional<int> GetChannel() const noexcept;
  /*
   * Включает телевизор, если он был выключен. Если телевизор уже включен, ничего не делает.
   * При включении выбирается тот номер канала, который был выбран перед последним выключением.
   * При самом первом включении выбирает канал #1.
   */
  void TurnOn() noexcept;

  /*
   * Выключает телевизор, если он был включен. Если телевизор уже выключен, ничего не делает.
   */
  void TurnOff() noexcept;

  /*
   * Выбирает канал channel.
   * Ранее выбранный канал запоминается и может быть восстановлен методом SelectLastViewedChannel.
   * Если номер канала совпадает с ранее выбранным каналом, метод ничего не делает.
   * Если телевизор выключен, выбрасывает исключение std::logic_error.
   * Если номер канала за пределами диапазона MIN_CHANNEL, MAX_CHANNEL, выбрасывает out_of_range.
   */
  void SelectChannel(int channel);
  /*
   * Выбирает номер канала, который был выбран перед последним вызовом SelectChannel.
   * Многократный последовательный вызов SelectChannel переключает два последних выбранных канала.
   * Если телевизор выключен, выбрасывает исключение std::logic_error.
   */
  void SelectLastViewedChannel();

 private:
  bool m_is_turned_on = false;
  int m_channel = 1;
  int m_prev_channel = 1;
};

#endif  // __TV_H__
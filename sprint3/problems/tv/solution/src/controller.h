#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include <iostream>
#include <cassert>

#include "tv.h"
#include "menu.h"

class Controller {
 public:
  Controller(TV& tv, Menu& menu);

 private:
  /*
   * Обрабатывает команду Info, выводя информацию о состоянии телевизора.
   * Если телевизор выключен, выводит "TV is turned off"
   * Если телевизор включен, выводит две строки:
   * TV is turned on
   * Channel number is <номер канала>
   * Если в input содержатся какие-либо параметры, выводит сообщение об ошибке:
   * Error: the Info command does not require any arguments
   */
  [[nodiscard]] bool ShowInfo(std::istream& input, std::ostream& output) const;

  /*
   * Обрабатывает команду TurnOn, включая телевизор
   * Если в input содержатся какие-либо параметры, не включает телевизор и выводит сообщение
   * об ошибке:
   * Error: the TurnOff command does not require any arguments
   */
  [[nodiscard]] bool TurnOn(std::istream& input, std::ostream& output) const;
  /*
   * Обрабатывает команду TurnOff, выключая телевизор
   * Если в input содержатся какие-либо параметры, не выключает телевизор и выводит сообщение
   * об ошибке:
   * Error: the TurnOff command does not require any arguments
   */
  [[nodiscard]] bool TurnOff(std::istream& input, std::ostream& output) const;

  /*
   * Обрабатывает команду SelectChannel <номер канала>
   * Выбирает заданный номер канала на tv_.
   * Если номер канала - не целое число, выводит в output ошибку "Invalid channel"
   * Обрабатывает ошибки переключения каналов на телевизор и выводит в output сообщения:
   * - "Channel is out of range", если TV::SelectChannel выбросил std::out_of_range
   * - "TV is turned off", если TV::SelectChannel выбросил std::logic_error
   */
  [[nodiscard]] bool SelectChannel(std::istream& input, std::ostream& output) const;

  /*
   * Обрабатывает команду SelectPreviousChannel, выбирая предыдущий канал на tv_.
   * Если TV::SelectLastViewedChannel выбросил std::logic_error, выводит в output сообщение:
   * "TV is turned off"
   */
  [[nodiscard]] bool SelectPreviousChannel(std::istream& input, std::ostream& output) const;

  [[nodiscard]] bool EnsureNoArgsInInput(std::string_view command,
                                         std::istream& input,
                                         std::ostream& output) const;

  constexpr static std::string_view INFO_COMMAND = "Info";
  constexpr static std::string_view TURN_ON_COMMAND = "TurnOn";
  constexpr static std::string_view TURN_OFF_COMMAND = "TurnOff";
  constexpr static std::string_view SELECT_CHANNEL_COMMAND = "SelectChannel";
  constexpr static std::string_view SELECT_PREVIOUS_CHANNEL_COMMAND = "SelectPreviousChannel";

  TV& m_tv;
  Menu& m_menu;
};

#endif  // __CONTROLLER_H__
//
// Created by kamil on 8/22/20.
//

#ifndef SPINMONITOR_TOOLS_HH
#define SPINMONITOR_TOOLS_HH

#include "config.hh"
#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <ostream>

auto getCurrentDateStr() -> std::string;



// human-readable std::chrono::duration ostream
template<typename T>
std::ostream &operator<<(std::ostream &os, std::chrono::duration<T> duration) {
  auto days = std::chrono::duration_cast<std::chrono::days>(duration);
  duration -= days;
  auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
  duration -= hours;
  auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
  duration -= minutes;
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
  if (days.count() != 0) {
    os << days.count() << " days ";
  }
  os << std::setfill('0') << std::setw(2) << hours.count() << ':'
     << std::setw(2) << minutes.count() << ':'
     << std::setw(2) << seconds.count();
  return os;
}


template<typename F>
constexpr inline auto debug(F f) -> void {
  if constexpr (CFG::DEBUG) {
    f();
  }
}


template<typename S>
constexpr inline auto log(S str, uint log_level = 0u) -> void {
  if (log_level <= CFG::LOG_LEVEL) {
    std::clog << getCurrentDateStr() << " " << str << std::endl;
  }
}


#endif//SPINMONITOR_TOOLS_HH

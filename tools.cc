//
// Created by kamil on 8/22/20.
//
#include <string>
#include <ctime>
#include <array>

auto getCurrentDateStr() -> std::string {
  auto now = time_t{time(0)};
  auto tstruct = tm{};
  auto buf = std::array<char, 40>{};
  tstruct = *localtime(&now);
  strftime(buf.data(), sizeof(buf), "%Y-%m-%d %X", &tstruct);
  return buf.data();
}
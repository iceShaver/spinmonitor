//
// Created by kamil on 8/11/20.
//

#ifndef SPINMONITOR_CLIENT_HH
#define SPINMONITOR_CLIENT_HH
#include "disk.hh"
#include <vector>
#include <chrono>

using namespace std::chrono_literals;

class Client final {
public:
  Client() = delete;
  static auto Init() -> void;
  static auto Run() -> void;
private:
  static auto getDisks() -> std::vector<std::unique_ptr<Disk>>;
  static inline bool isRunning = true;
  static inline std::chrono::milliseconds const interval = 1000ms;
};


#endif//SPINMONITOR_CLIENT_HH

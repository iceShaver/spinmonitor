//
// Created by kamil on 8/4/20.
//

#ifndef SPINMONITOR_DAEMON_HH
#define SPINMONITOR_DAEMON_HH

#include "disk.hh"
#include <chrono>
#include <vector>
using namespace std::chrono_literals;
/*
 * Daemons tasks:
 * * monitors disks activity and logs info to file
 */
class Daemon final {
public:
  Daemon() = delete;

  static auto Init() -> void;
  static auto Run() -> void;

  static auto Exec(std::string const &cmd) -> std::string;

private:
  static auto probeDisks() -> void;
  static auto createInPipe() -> void;
  static auto createOutPipe() -> void;
  static auto deleteOutPipe() -> void;
  static auto createPipeResponder() -> void;
  static auto sendDisks() -> void;
  static auto resetCounters() -> void;
  static auto ensureDaemonIsNotRunningAlready() -> void;

  static inline bool running{};
  static inline std::chrono::milliseconds const interval = 1000ms;
  static inline std::vector<std::unique_ptr<Disk>> disks;
  static inline int inPipeFd = {};
  static inline int outPipeFd = {};
};


#endif//SPINMONITOR_DAEMON_HH

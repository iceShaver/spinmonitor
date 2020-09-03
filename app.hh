//
// Created by kamil on 8/4/20.
//

#ifndef SPINMONITOR_APP_HH
#define SPINMONITOR_APP_HH

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>

class App final {
public:
  App() = delete;

  static auto Init(int argc, char **argv) -> void;
  static auto Run() -> void;
  static auto Stop() -> void;


private:
  static auto parseArgs() -> void;
  static auto exit(int exitCode) -> void;
  static auto pingDaemon()-> void;
  static auto set_stdlog_to_file(std::filesystem::path const&path) -> void;
  static auto restore_stdlog_default() -> void;

  static inline std::ofstream log_file;
  static inline int argc;
  static inline char const *const * argv;
  static inline enum class RunMode { DAEMON, CLIENT} runMode = RunMode::CLIENT;
  static inline std::streambuf * const clog_buf = std::clog.rdbuf();
};


#endif//SPINMONITOR_APP_HH

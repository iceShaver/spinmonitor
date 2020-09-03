//
// Created by kamil on 8/4/20.
//

#include "app.hh"
#include "client.hh"
#include "config.hh"
#include "daemon.hh"
#include "tools.hh"
#include <boost/program_options.hpp>
#include <cerrno>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <poll.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std::string_literals;
namespace po = boost::program_options;


auto App::parseArgs() -> void {
  po::options_description desc{"Allowed options"};
  // clang-format off
  desc.add_options()
    ("help,h", "print help")
    ("version,v", "print program version")
    ("daemon,d", "run SpinMonitor daemon for logging spinning status")
    ("current,c", "Show current spin status of disks")
    ("ping,p", "Ping running daemon");
  // clang-format on
  po::variables_map vm;
  po::store(po::parse_command_line(App::argc, App::argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    App::exit(0);
  }

  if (vm.count("daemon")) {
    App::runMode = RunMode::DAEMON;
  }

  if (vm.count("ping")) {
    App::pingDaemon();
  }
}


auto App::Init(int argc, char **argv) -> void {
  App::argc = argc;
  App::argv = argv;
  log("Initializing...");
  umask(0000);
  App::parseArgs();
}


auto App::exit(int exitCode) -> void {
  log("Exiting application");
  ::exit(exitCode);
}


auto App::Run() -> void {
  if (App::runMode == RunMode::DAEMON) {
    auto euid = geteuid();
    if (euid != 0) {
      std::cerr << "App has to be ran as root" << std::endl;
      throw std::runtime_error("Unsufficient priviledges");
    }
    log("Running as daemon");
    App::set_stdlog_to_file("daemon_log.txt");
    Daemon::Init();
    Daemon::Run();
    App::exit(0);
  }
  log("Running as client");
  App::set_stdlog_to_file("client_log.txt");
  Client::Init();
  Client::Run();
}


auto App::pingDaemon() -> void {
  log("Pinging daemon");
  auto pipeFdPoll = pollfd{.fd = open(CFG::DAEMON_IN_PIPE_FILE, O_RDWR | O_NONBLOCK),
                           .events = POLLOUT,
                           .revents = 0};
  if (pipeFdPoll.fd == -1)
    throw std::runtime_error("Opening pipe failed. "s + strerror(errno));
  poll(&pipeFdPoll, 1, -1);
  auto dataHeader = DataHeader{};
  auto bytesSent = write(pipeFdPoll.fd, &dataHeader, sizeof(dataHeader));
  log("Sent bytes: " + std::to_string(bytesSent));
  close(pipeFdPoll.fd);
  App::exit(0);
}


auto App::set_stdlog_to_file(std::filesystem::path const &path) -> void {
  App::log_file.open(path, std::ios_base::out | std::ios_base::app);
  std::clog.rdbuf(log_file.rdbuf());
}


auto App::restore_stdlog_default() -> void {
  std::clog.rdbuf(App::clog_buf);
  App::log_file.close();
}

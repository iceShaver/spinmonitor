//
// Created by kamil on 8/4/20.
//

#include "daemon.hh"
#include "config.hh"
#include "sata_disk.hh"
#include "scsi_disk.hh"
#include "tools.hh"
#include <algorithm>
#include <csignal>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <parted/device.h>
#include <poll.h>
#include <regex>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

using namespace std::string_literals;

auto Daemon::Init() -> void {
  log("________________daemon_init__________________");
  std::filesystem::remove(CFG::DAEMON_OUT_PIPE_FILE);
  std::filesystem::remove(CFG::DAEMON_IN_PIPE_FILE);
  Daemon::ensureDaemonIsNotRunningAlready();
  Daemon::probeDisks();
  Daemon::createInPipe();
  Daemon::createPipeResponder();
  signal(SIGINT, [](int signum) {
    // TODO: check if it is safe to call these functions in signal handler
    close(Daemon::inPipeFd);
    close(Daemon::outPipeFd);
    std::filesystem::remove(CFG::DAEMON_IN_PIPE_FILE);
    std::filesystem::remove(CFG::DAEMON_OUT_PIPE_FILE);
    std::cout << "\nBye" << std::endl;
    log("Exiting daemon");
    _exit(0);
  });

}


auto Daemon::Run() -> void {
  Daemon::running = true;
  log("Updating disks with interval=" + std::to_string(Daemon::interval.count()));
  while (Daemon::running) {
    for (auto &&d : Daemon::disks) d->probeStatus();
    std::this_thread::sleep_for(Daemon::interval);
  }
}


auto Daemon::probeDisks() -> void {
  constexpr auto const BLOCK_DEVS_DIR = "/sys/block";
  std::regex regexTxt{"sd*"};
  for (auto const &entry : std::filesystem::directory_iterator(BLOCK_DEVS_DIR)) {
    auto diskSystemName = entry.path().filename().string();

    // check if scsi drive
    if (diskSystemName.substr(0, 2) != "sd") continue;

    // check if rotational drive
    bool rotational;
    std::ifstream{"/sys/block/" + diskSystemName + "/queue/rotational"} >> rotational;
    if constexpr (CFG::EXCLUDE_NON_ROTATIONAL_DRIVES) {
      if (!rotational) continue;
    }

    // check disk vendor (ATA or other)
    auto s = std::stringstream{};
    s << std::ifstream{"/sys/block/" + diskSystemName + "/device/vendor"}.rdbuf();
    auto diskVendor = s.str();
    if (diskVendor.substr(0, 3) == "ATA") {
      disks.push_back(std::make_unique<SataDisk>());
    } else {
      disks.push_back(std::make_unique<ScsiDisk>());
    }
    disks.back()->setSysDiskName(diskSystemName);
    disks.back()->probeStatus();
  }
  std::sort(disks.begin(), disks.end(),
            [](auto &a, auto &b) { return a->getSysDiskName() < b->getSysDiskName(); });
}


auto Daemon::Exec(std::string const &cmd) -> std::string {
  std::array<char, 128> buffer{};
  std::string result;
  auto pipe = std::unique_ptr<FILE, decltype(&pclose)>(popen(cmd.c_str(), "r"), pclose);
  if (pipe == nullptr) {
    throw std::runtime_error("popen() failed");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}


auto Daemon::createInPipe() -> void {
  std::filesystem::create_directory(CFG::DAEMON_PIPES_DIR);
  std::filesystem::remove(CFG::DAEMON_IN_PIPE_FILE);
  auto result = mkfifo(CFG::DAEMON_IN_PIPE_FILE, 0777);
  if (result) {
    throw std::runtime_error("create daemon in pipe file failed. Errno: "s + strerror(errno));
  }
  log("Created daemon in pipe file: "s + CFG::DAEMON_IN_PIPE_FILE);
  Daemon::inPipeFd = open(CFG::DAEMON_IN_PIPE_FILE, O_RDWR | O_NONBLOCK);
  log("Opened daemon pipe files");
  if (Daemon::inPipeFd == -1)
    throw std::runtime_error("Opening daemon in pipe file failed. Errno: "s + strerror(errno));
}


auto Daemon::createOutPipe() -> void {
  std::filesystem::remove(CFG::DAEMON_OUT_PIPE_FILE);
  auto result = mkfifo(CFG::DAEMON_OUT_PIPE_FILE, 0777);
  if (result) {
    throw std::runtime_error("create daemon out pipe failed. Errno: "s + strerror(errno));
  }
  log("Created daemon out pipe file: "s + CFG::DAEMON_OUT_PIPE_FILE);
  log("Waiting for open...");
  Daemon::outPipeFd = open(CFG::DAEMON_OUT_PIPE_FILE, O_WRONLY);
  if (Daemon::outPipeFd == -1) {
    throw std::runtime_error("Opening out pipe failed. Errno: "s + strerror(errno));
  }
}


auto Daemon::createPipeResponder() -> void {
  std::thread([]() {
    log("Responder started");
    while (true) {
      auto pipeFdPoll = pollfd{Daemon::inPipeFd, POLLIN, 0};
      log("Waiting for POLLIN");
      poll(&pipeFdPoll, 1, -1);
      DataHeader dh;
      log("Reading header");
      read(Daemon::inPipeFd, &dh, sizeof(dh));
      switch (dh.ht) {
        case HeaderType::GET_DISKS:
          log("Received GetDisks cmd");
          Daemon::sendDisks();
          break;
        case HeaderType::RESET_COUNTERS:
          Daemon::resetCounters();
          break;
        default:
          std::cerr << "Received unknown command" << std::endl;
          break;
        case HeaderType::PING:
          log("Received ping");
          break;
        case HeaderType::DISKS_ARRAY:
          break;
      }
    }

  }).detach();
}


auto Daemon::sendDisks() -> void {
  auto sender = [](std::vector<std::byte> const &bytes) {
    log("Sending disks");
    try {
      log("Opening pipe");
      Daemon::createOutPipe();
    } catch (std::runtime_error &e) {
      std::cerr << e.what() << std::endl;
      return;
    }
    log("Opened out pipe");
    auto pipeFdPoll = pollfd{Daemon::outPipeFd, POLLOUT, 0};

    // send header
    log("Sending header");
    poll(&pipeFdPoll, 1, -1);
    auto dataHeader = DataHeader{.ht = HeaderType::DISKS_ARRAY, .bytes = bytes.size()};
    write(Daemon::outPipeFd, &dataHeader, sizeof(dataHeader));

    log("Sending bytes");
    // send bytes
    poll(&pipeFdPoll, 1, -1);
    auto written = write(Daemon::outPipeFd, bytes.data(), bytes.size());
    log("Sent "s + std::to_string(written) + " bytes" );
    Daemon::deleteOutPipe();
    close(Daemon::outPipeFd);
    Daemon::outPipeFd = 0;
    std::filesystem::remove(CFG::DAEMON_OUT_PIPE_FILE);
  };

  // serialize disks
  auto serializedDisks = std::vector<std::byte>{};
  for (auto const &d : Daemon::disks) {
    auto serializedD = d->serialize();
    serializedDisks.insert(serializedDisks.end(), serializedD.begin(), serializedD.end());
  }

  log("Serialized disks for sending");
  sender(serializedDisks);
}


auto Daemon::resetCounters() -> void {
}


auto Daemon::ensureDaemonIsNotRunningAlready() -> void {
  // TODO: change that
//  if (!Daemon::Exec("lsof "s + CFG::DAEMON_IN_PIPE_FILE + " 2>/dev/null").empty()) {
//    throw std::runtime_error("Daemon is running already");
//  }
}


auto Daemon::deleteOutPipe() -> void {
  if(Daemon::outPipeFd){
    close(Daemon::outPipeFd);
    Daemon::outPipeFd = 0;
    std::filesystem::remove(CFG::DAEMON_OUT_PIPE_FILE);
  }

}

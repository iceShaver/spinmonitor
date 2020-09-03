//
// Created by kamil on 8/11/20.
//

#include "client.hh"
#include "config.hh"
#include "tools.hh"
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <poll.h>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>

using namespace std::string_literals;

auto Client::getDisks() -> std::vector<std::unique_ptr<Disk>> {
  // Send GET_DISKS request
  log("SendingDisksRequest");
  auto pipeFdPoll = pollfd{.fd = open(CFG::DAEMON_IN_PIPE_FILE, O_WRONLY),
                           .events = POLLOUT,
                           .revents = 0};
  if (pipeFdPoll.fd == -1) {
    throw std::runtime_error("Opening pipe failed. "s + strerror(errno));
  }
  poll(&pipeFdPoll, 1, -1);
  auto dataHeader = DataHeader{.ht = HeaderType ::GET_DISKS};
  write(pipeFdPoll.fd, &dataHeader, sizeof(dataHeader));
  close(pipeFdPoll.fd);
  log("Sent GetDisks cmd");

  // Recv DISKS_ARRAY
  log("Opening "s + CFG::DAEMON_OUT_PIPE_FILE);

  do {
    pipeFdPoll.fd = open(CFG::DAEMON_OUT_PIPE_FILE, O_RDONLY);
  } while (pipeFdPoll.fd == -1 && errno == ENOENT && (std::this_thread::sleep_for(1ms), true));


  if (pipeFdPoll.fd == -1) {
    throw std::runtime_error("opening "s + CFG::DAEMON_OUT_PIPE_FILE + " failed: " + strerror(errno));
  }

  log("Opened daemon out pipe file");
  pipeFdPoll.events = POLLIN;
  pipeFdPoll.revents = 0;
  log("Waiting for POLLIN");
  poll(&pipeFdPoll, 1, -1);
  read(pipeFdPoll.fd, &dataHeader, sizeof(dataHeader));
  if (dataHeader.ht == HeaderType::DISKS_ARRAY)
    log("GotHeader DISKS_ARRAY");
  else if (dataHeader.ht == HeaderType::PING) {
    log("received ping");
    return {};
  } else {
    log("received bad header");
  }
  auto bytes = std::vector<std::byte>{dataHeader.bytes};
  log(std::to_string(bytes.size()));
  poll(&pipeFdPoll, 1, -1);
  auto received_bytes = read(pipeFdPoll.fd, bytes.data(), bytes.size());
  if (received_bytes == -1) {
    log("Read error: "s + strerror(errno));
    return {};
  }
  log("Received " + std::to_string(received_bytes) + " / " + std::to_string(dataHeader.bytes) + " bytes");
  return Disk::deserializeMuliple(bytes);
}


auto Client::Init() -> void {
  isRunning = true;
  log("__________________________________");
}



auto Client::Run() -> void {
  while (Client::isRunning) {
    auto disks = Client::getDisks();
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    system("clear");
    std::cout << std::ctime(&now) << std::endl;
    for (auto const &d : disks) {
      std::cout << *d << std::endl;
    }
    std::this_thread::sleep_for(Client::interval);
  }
}

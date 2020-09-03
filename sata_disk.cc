//
// Created by kamil on 8/5/20.
//

#include "sata_disk.hh"
#include "daemon.hh"
#include <iostream>
#include <regex>
#include <string>
#include "config.hh"

using namespace std::string_literals;

auto SataDisk::probeStatus() -> void {
  auto cmd = CFG::SATA_STATUS_CMD + this->getSysDiskName();
  auto cmdResult = Daemon::Exec(cmd);
  auto regexTxt = std::regex{CFG::SATA_STATUS_CMD_OUTPUT_ACTIVE_REGEX};
  if (std::regex_search(cmdResult, regexTxt))
    this->setDiskStatus(DiskStatus::SPINNING);
  else
    this->setDiskStatus(DiskStatus::SLEEPING);
}


auto SataDisk::serialize() -> std::vector<std::byte> {
  auto partial =  Disk::serialize();
  auto dt = DiskType::SATA;
  std::memcpy(partial.data(), (void*)&dt, sizeof(DiskType));
  return partial;
}

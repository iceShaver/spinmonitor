//
// Created by kamil on 8/5/20.
//

#include "scsi_disk.hh"
#include "config.hh"
#include "daemon.hh"
#include <regex>
auto ScsiDisk::probeStatus() -> void {
  auto cmd = CFG::SCSI_STATUS_CMD + this->getSysDiskName();
  auto cmdResult = Daemon::Exec(cmd);
  auto regexTxt = std::regex{CFG::SCSI_STATUS_CMD_OUTPT_NOT_ACTIVE_REGEX};
  if (std::regex_search(cmdResult, regexTxt))
    this->setDiskStatus(DiskStatus::SLEEPING);
  else
    this->setDiskStatus(DiskStatus::SPINNING);
}
auto ScsiDisk::serialize() -> std::vector<std::byte> {
  auto partial = Disk::serialize();
  auto dt = DiskType::SCSI;
  memcpy(partial.data(), (void *) &dt, sizeof(DiskType));
  return partial;
}

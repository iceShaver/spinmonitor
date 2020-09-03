//
// Created by kamil on 8/4/20.
//

#include "disk.hh"
#include "sata_disk.hh"
#include "scsi_disk.hh"
#include "tools.hh"
#include <cstring>
#include <utility>
#include <iostream>
#include <cstddef>

Disk::Disk(std::string sysDiskName, DiskType diskType, DiskStatus diskStatus)
    : sysDiskName(std::move(sysDiskName)), diskStatus(diskStatus),
      lastStatusChangeTime(std::chrono::system_clock::now()) {
}


Disk::Disk() : sysDiskName(), diskStatus(), lastStatusChangeTime(std::chrono::system_clock::now()) {
}


std::string const &Disk::getSysDiskName() const {
  return sysDiskName;
}


void Disk::setSysDiskName(std::string const &sysDiskName) {
  this->sysDiskName = sysDiskName;
}


DiskStatus Disk::getDiskStatus() const {
  return diskStatus;
}


void Disk::setDiskStatus(DiskStatus diskStatus) {
  if (this->diskStatus != diskStatus)
    this->lastStatusChangeTime = std::chrono::system_clock::now();
  this->diskStatus = diskStatus;
}

auto Disk::getSecondsSinceLastStatusChange() const {
  auto result = std::chrono::system_clock::now() - this->lastStatusChangeTime;
  return std::chrono::duration_cast<std::chrono::seconds>(result);
}


std::ostream &operator<<(std::ostream &os, Disk const &disk) {
  os << disk.sysDiskName << ": "
     << (disk.diskStatus == DiskStatus::SPINNING ? "Active" : "Suspended")
     << "\t Since: " << disk.getSecondsSinceLastStatusChange();
  return os;
}


auto Disk::serialize()  -> std::vector<std::byte> {
  auto result = std::vector<std::byte>{};
  auto size = sizeof(DiskType) + this->sysDiskName.length() + 1 + sizeof(DiskStatus) + sizeof(long);
  result.resize(size);
  auto vecWritePtr = result.data() + sizeof(DiskType);
  // diskType, diskName, diskStatus ,lastStatusChangeTime
  std::strcpy((char *) vecWritePtr, this->sysDiskName.c_str());
  vecWritePtr += this->sysDiskName.length() + 1;
  std::memcpy(vecWritePtr, &this->diskStatus, sizeof(this->diskStatus));
  vecWritePtr += sizeof(this->diskStatus);
  // time

  long time = this->lastStatusChangeTime.time_since_epoch().count();
  std::memcpy(vecWritePtr, &time, sizeof(time));

  return result;
}




auto Disk::deserializeMuliple(std::vector<std::byte> const &data) -> std::vector<std::unique_ptr<Disk>> {
  auto result = std::vector<std::unique_ptr<Disk>>{};
  auto vecReadPtr = data.data();
  std::clog << std::dec;
  while (vecReadPtr < data.data() + data.size()) {
    // disk type
    auto diskTypePtr = (DiskType*) vecReadPtr;
    vecReadPtr += sizeof(DiskType);
    if(*diskTypePtr == DiskType::SATA)
      result.push_back(std::make_unique<SataDisk>());
    else if(*diskTypePtr == DiskType::SCSI)
      result.push_back(std::make_unique<ScsiDisk>());
    // disk name
    auto name = std::string{(char const*)vecReadPtr};
    result.back()->sysDiskName = name;
    vecReadPtr += name.length() + 1;
    // disk status
    result.back()->diskStatus = *(DiskStatus*) vecReadPtr;
    vecReadPtr += sizeof(DiskStatus);
    // lastStatusChangeTime
    std::chrono::system_clock::duration const d(*(long*)vecReadPtr);
    std::chrono::system_clock::time_point const t1(d);
    result.back()->lastStatusChangeTime = t1;
    vecReadPtr += sizeof(long);
  }
  return result;
}

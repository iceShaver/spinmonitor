//
// Created by kamil on 8/4/20.
//

#ifndef SPINMONITOR_DISK_HH
#define SPINMONITOR_DISK_HH
#include <filesystem>
#include <ostream>
#include <vector>
#include <chrono>

using namespace std::chrono_literals;


enum class DiskType { SATA,
                      SCSI };
enum class DiskStatus { SPINNING,
                        SLEEPING,
                        UNDETERMINED };
class Disk {
public:
  Disk();
  Disk(std::string sysDiskName, DiskType diskType, DiskStatus diskStatus);

  virtual auto serialize() -> std::vector<std::byte>;
  static auto deserializeMuliple(std::vector<std::byte>const&data) -> std::vector<std::unique_ptr<Disk>>;


  auto getSysDiskName() const -> std::string const &;
  auto setSysDiskName(std::string const &sysDiskName) -> void;
  auto getDiskStatus() const -> DiskStatus;
  auto getSecondsSinceLastStatusChange() const;

  virtual auto probeStatus() -> void = 0;
  friend std::ostream &operator<<(std::ostream &os, Disk const &disk);

protected:
  auto setDiskStatus(DiskStatus diskStatus) -> void;

private:
  std::string sysDiskName;
  DiskStatus diskStatus;
  std::chrono::system_clock::time_point lastStatusChangeTime;
};


#endif//SPINMONITOR_DISK_HH

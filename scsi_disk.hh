//
// Created by kamil on 8/5/20.
//

#ifndef SPINMONITOR_SCSI_DISK_HH
#define SPINMONITOR_SCSI_DISK_HH


#include "disk.hh"
class ScsiDisk : public Disk {
  auto probeStatus() -> void override;
  auto serialize() -> std::vector<std::byte> override;

protected:
};


#endif//SPINMONITOR_SCSI_DISK_HH

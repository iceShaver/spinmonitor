//
// Created by kamil on 8/5/20.
//

#ifndef SPINMONITOR_SATA_DISK_HH
#define SPINMONITOR_SATA_DISK_HH


#include "disk.hh"
class SataDisk : public Disk {
public:
  auto probeStatus()  -> void override;
  auto serialize() -> std::vector<std::byte> override;


protected:
};


#endif//SPINMONITOR_SATA_DISK_HH

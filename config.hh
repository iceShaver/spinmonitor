//
// Created by kamil on 8/11/20.
//

#ifndef SPINMONITOR_CONFIG_HH
#define SPINMONITOR_CONFIG_HH


#define PARAM constexpr auto const

namespace CFG {
  PARAM DEBUG = 0u;
  PARAM APP_NAME = "SpinMonitor";
  PARAM VER = "0.1";
  PARAM SATA_STATUS_CMD = "hdparm -C /dev/";
  PARAM SATA_STATUS_CMD_OUTPUT_ACTIVE_REGEX = "active";
  PARAM SCSI_STATUS_CMD = "sdparm -Cready /dev/";
  PARAM SCSI_STATUS_CMD_OUTPT_NOT_ACTIVE_REGEX = "Not ready";
  PARAM DAEMON_IN_PIPE_FILE = "/run/SpinMonitor/daemon.in.pipe";
  PARAM DAEMON_OUT_PIPE_FILE = "/run/SpinMonitor/daemon.out.pipe";
  PARAM DAEMON_PIPES_DIR = "/run/SpinMonitor";
  PARAM LOG_LEVEL = 3u;
  PARAM EXCLUDE_NON_ROTATIONAL_DRIVES = false;
}// namespace CFG


enum class HeaderType {
  PING,
  GET_DISKS,
  RESET_COUNTERS,
  DISKS_ARRAY
};

struct DataHeader final {
  HeaderType ht = HeaderType::PING;
  size_t bytes = 0;
};

#endif//SPINMONITOR_CONFIG_HH

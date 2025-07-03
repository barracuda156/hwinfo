// Copyright (c) Leon Freist <freist@informatik.uni-freiburg.de>
// This software is part of HWBenchmark

#include "hwinfo/platform.h"

#ifdef HWINFO_APPLE

#include <hwinfo/ram.h>
#include <sys/sysctl.h>
#include <mach/mach_host.h>
#include <mach/mach.h>
#include <unistd.h>

#include <string>

namespace hwinfo {

// _____________________________________________________________________________________________________________________
int64_t getMemSize() {
  uint64_t memSize;
  size_t size = sizeof(memSize);
  int res;

  res = sysctlbyname("hw.memsize", &memSize, &size, nullptr, 0);
  if (res < 0) {
    return -1;
  }

  return memSize;
}

// _____________________________________________________________________________________________________________________
Memory::Memory() {
  // TODO: get information for actual memory modules (DIMM)
  Module module;
  module.vendor = "<unknown>";
  module.name = "<unknown>";
  module.serial_number = "<unknown>";
  module.model = "<unknown>";
  module.id = 0;
  module.total_Bytes = getMemSize();
  module.frequency_Hz = -1;
  _modules.push_back(module);
}

// _____________________________________________________________________________________________________________________
int64_t Memory::free_Bytes() const {
  mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
  vm_statistics_data_t vmstat;
  kern_return_t ret = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmstat, &count);
  if (ret != KERN_SUCCESS) return 0;
  uint64_t free = (uint64_t)vmstat.free_count * (uint64_t)sysconf(_SC_PAGESIZE);
  return free;
}

// _____________________________________________________________________________________________________________________
int64_t Memory::available_Bytes() const {
  mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
  vm_statistics_data_t vmstat;
  kern_return_t ret = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vmstat, &count);
  if (ret != KERN_SUCCESS) return 0;
  uint64_t free = (uint64_t)vmstat.free_count * (uint64_t)sysconf(_SC_PAGESIZE);
  uint64_t inactive = (uint64_t)vmstat.inactive_count * (uint64_t)sysconf(_SC_PAGESIZE);
  return free + inactive;
}

}  // namespace hwinfo

#endif  // HWINFO_APPLE

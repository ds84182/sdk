// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "vm/globals.h"
#if defined(HOST_OS_CTR)

#include "vm/virtual_memory.h"

#include <unistd.h>

#include "platform/assert.h"
#include "platform/utils.h"

#include "vm/isolate.h"

namespace dart {

uword VirtualMemory::page_size_ = 0x1000;

void VirtualMemory::Init() {
}

static void unmap(void* address, intptr_t size) {
  if (size == 0) {
    return;
  }

  u32 addr = reinterpret_cast<u32>(address);
  Result res = svcControlMemory(&addr, addr, 0, size, MEMOP_FREE, (MemPerm)0);
  if (res != 0) {
    FATAL2("svcControlMemory error: %d (%s)", res, osStrError(res));
  }
}

// TODO: Non-linear allocations

VirtualMemory* VirtualMemory::Allocate(intptr_t size,
                                       bool is_executable,
                                       const char* name) {
  ASSERT(Utils::IsAligned(size, page_size_));
  ASSERT(!is_executable);
  uint32_t address = 0;
  Result res = svcControlMemory(&address, 0, 0, size, MEMOP_ALLOC_LINEAR, (MemPerm)(MEMPERM_READ | MEMPERM_WRITE));
  if (res != 0) {
    return NULL;
  }
  MemoryRegion region(reinterpret_cast<void*>(address), size);
  return new VirtualMemory(region, region);
}

VirtualMemory* VirtualMemory::AllocateAligned(intptr_t size,
                                              intptr_t alignment,
                                              bool is_executable,
                                              const char* name) {
  ASSERT(Utils::IsAligned(size, page_size_));
  ASSERT(Utils::IsAligned(alignment, page_size_));
  ASSERT(!is_executable);
  intptr_t allocated_size = size + alignment;
  uint32_t address = 0;
  Result res = svcControlMemory(&address, 0, 0, allocated_size, MEMOP_ALLOC_LINEAR, (MemPerm)(MEMPERM_READ | MEMPERM_WRITE));
  if (res != 0) {
    return NULL;
  }

  uword base = address;
  uword aligned_base = Utils::RoundUp(base, alignment);
  ASSERT(base <= aligned_base);

  if (base != aligned_base) {
    uword extra_leading_size = aligned_base - base;
    unmap(reinterpret_cast<void*>(base), extra_leading_size);
    allocated_size -= extra_leading_size;
  }

  if (allocated_size != size) {
    uword extra_trailing_size = allocated_size - size;
    unmap(reinterpret_cast<void*>(aligned_base + size), extra_trailing_size);
  }

  MemoryRegion region(reinterpret_cast<void*>(aligned_base), size);
  return new VirtualMemory(region, region);
}

VirtualMemory::~VirtualMemory() {
  if (vm_owns_region()) {
    unmap(reserved_.pointer(), reserved_.size());
  }
}

bool VirtualMemory::FreeSubSegment(void* address,
                                   intptr_t size) {
  unmap(address, size);
  return true;
}

void VirtualMemory::Protect(void* address, intptr_t size, Protection mode) {
#if defined(DEBUG)
  Thread* thread = Thread::Current();
  ASSERT((thread == nullptr) || thread->IsMutatorThread() ||
         thread->isolate()->mutator_thread()->IsAtSafepoint());
#endif
  // uword start_address = reinterpret_cast<uword>(address);
  // uword end_address = start_address + size;
  // uword page_address = Utils::RoundDown(start_address, PageSize());
  // int prot = 0;
  // switch (mode) {
  //   case kNoAccess:
  //     prot = PROT_NONE;
  //     break;
  //   case kReadOnly:
  //     prot = PROT_READ;
  //     break;
  //   case kReadWrite:
  //     prot = PROT_READ | PROT_WRITE;
  //     break;
  //   case kReadExecute:
  //     prot = PROT_READ | PROT_EXEC;
  //     break;
  //   case kReadWriteExecute:
  //     prot = PROT_READ | PROT_WRITE | PROT_EXEC;
  //     break;
  // }
  // if (mprotect(reinterpret_cast<void*>(page_address),
  //              end_address - page_address, prot) != 0) {
  //   int error = errno;
  //   const int kBufferSize = 1024;
  //   char error_buf[kBufferSize];
  //   FATAL2("mprotect error: %d (%s)", error,
  //          Utils::StrError(error, error_buf, kBufferSize));
  // }
}

}  // namespace dart

#endif  // defined(HOST_OS_CTR)

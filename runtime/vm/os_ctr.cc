// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "vm/globals.h"
#if defined(HOST_OS_CTR)

#include "vm/os.h"

#include <errno.h>           // NOLINT
#include <limits.h>          // NOLINT
#include <sys/resource.h>    // NOLINT
#include <sys/time.h>        // NOLINT
#include <unistd.h>          // NOLINT

#include "platform/utils.h"
#include "vm/isolate.h"
#include "vm/zone.h"

namespace dart {

const char* OS::Name() {
  return "ctr";
}

intptr_t OS::ProcessId() {
  return static_cast<intptr_t>(0); // TODO
}


const char* OS::GetTimeZoneName(int64_t seconds_since_epoch) {
  return "";
}

int OS::GetTimeZoneOffsetInSeconds(int64_t seconds_since_epoch) {
  return 0;
}

int OS::GetLocalTimeZoneAdjustmentInSeconds() {
  return 0;
}

int64_t OS::GetCurrentTimeMillis() {
  return GetCurrentTimeMicros() / 1000;
}

int64_t OS::GetCurrentTimeMicros() {
  return osGetTime() * 1000;
}

int64_t OS::GetCurrentMonotonicTicks() {
  return svcGetSystemTick();
}

int64_t OS::GetCurrentMonotonicFrequency() {
  return SYSCLOCK_ARM11;
}

int64_t OS::GetCurrentMonotonicMicros() {
  return GetCurrentMonotonicTicks() / CPU_TICKS_PER_MSEC;
}

int64_t OS::GetCurrentThreadCPUMicros() {
  return -1;
}

intptr_t OS::ActivationFrameAlignment() {
  // Even if we generate code that maintains a stronger alignment, we cannot
  // assert the stronger stack alignment because C++ code will not maintain it.
  return 8;
}

intptr_t OS::PreferredCodeAlignment() {
  const int kMinimumAlignment = 16;
  intptr_t alignment = kMinimumAlignment;
  // TODO(5411554): Allow overriding default code alignment for
  // testing purposes.
  // Flags::DebugIsInt("codealign", &alignment);
  ASSERT(Utils::IsPowerOfTwo(alignment));
  ASSERT(alignment >= kMinimumAlignment);
  ASSERT(alignment <= OS::kMaxPreferredCodeAlignment);
  return alignment;
}

int OS::NumberOfAvailableProcessors() {
  return PTMSYSM_CheckNew3DS() ? 3 : 1; // TODO: 4 and 2
}

void OS::Sleep(int64_t millis) {
  int64_t micros = millis * kMicrosecondsPerMillisecond;
  SleepMicros(micros);
}

void OS::SleepMicros(int64_t micros) {
  svcSleepThread(micros * kNanosecondsPerMicrosecond);
}

void OS::DebugBreak() {
  __builtin_trap(); // TODO: svcBreak
}

DART_NOINLINE uintptr_t OS::GetProgramCounter() {
  return reinterpret_cast<uintptr_t>(
      __builtin_extract_return_addr(__builtin_return_address(0)));
}


void OS::Print(const char* format, ...) {
  va_list args;
  va_start(args, format);
  VFPrint(stdout, format, args);
  va_end(args);
}

void OS::VFPrint(FILE* stream, const char* format, va_list args) {
  vfprintf(stream, format, args);
  fflush(stream);
}

char* OS::SCreate(Zone* zone, const char* format, ...) {
  va_list args;
  va_start(args, format);
  char* buffer = VSCreate(zone, format, args);
  va_end(args);
  return buffer;
}

char* OS::VSCreate(Zone* zone, const char* format, va_list args) {
  // Measure.
  va_list measure_args;
  va_copy(measure_args, args);
  intptr_t len = Utils::VSNPrint(NULL, 0, format, measure_args);
  va_end(measure_args);

  char* buffer;
  if (zone) {
    buffer = zone->Alloc<char>(len + 1);
  } else {
    buffer = reinterpret_cast<char*>(malloc(len + 1));
  }
  ASSERT(buffer != NULL);

  // Print.
  va_list print_args;
  va_copy(print_args, args);
  Utils::VSNPrint(buffer, len + 1, format, print_args);
  va_end(print_args);
  return buffer;
}

bool OS::StringToInt64(const char* str, int64_t* value) {
  ASSERT(str != NULL && strlen(str) > 0 && value != NULL);
  int32_t base = 10;
  char* endptr;
  int i = 0;
  if (str[0] == '-') {
    i = 1;
  } else if (str[0] == '+') {
    i = 1;
  }
  if ((str[i] == '0') && (str[i + 1] == 'x' || str[i + 1] == 'X') &&
      (str[i + 2] != '\0')) {
    base = 16;
  }
  errno = 0;
  if (base == 16) {
    // Unsigned 64-bit hexadecimal integer literals are allowed but
    // immediately interpreted as signed 64-bit integers.
    *value = static_cast<int64_t>(strtoull(str, &endptr, base));
  } else {
    *value = strtoll(str, &endptr, base);
  }
  return ((errno == 0) && (endptr != str) && (*endptr == 0));
}

void OS::RegisterCodeObservers() {}

void OS::PrintErr(const char* format, ...) {
  va_list args;
  va_start(args, format);
  VFPrint(stderr, format, args);
  va_end(args);
}

void OS::Init() {}

void OS::Cleanup() {}

void OS::Abort() {
  abort();
}

void OS::Exit(int code) {
  exit(code);
}

}  // namespace dart

#endif  // defined(HOST_OS_CTR)

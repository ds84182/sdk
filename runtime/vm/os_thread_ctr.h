// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#ifndef RUNTIME_VM_OS_THREAD_CTR_H_
#define RUNTIME_VM_OS_THREAD_CTR_H_

#if !defined(RUNTIME_VM_OS_THREAD_H_)
#error Do not include os_thread_ctr.h directly; use os_thread.h instead.
#endif

#include <3ds.h>

#include "platform/assert.h"
#include "platform/globals.h"

namespace dart {

typedef uword ThreadLocalKey;
typedef ::Thread ThreadId;
typedef ::Thread ThreadJoinId;

static const ThreadLocalKey kUnsetThreadLocalKey =
    static_cast<uword>(-1);

extern thread_local uword tlsData[0x100];

class ThreadInlineImpl {
 private:
  ThreadInlineImpl() {}
  ~ThreadInlineImpl() {}

  static uword GetThreadLocal(ThreadLocalKey key) {
    ASSERT(key != kUnsetThreadLocalKey);
    return tlsData[key-1];
  }

  friend class OSThread;

  DISALLOW_ALLOCATION();
  DISALLOW_COPY_AND_ASSIGN(ThreadInlineImpl);
};

class MutexData {
 private:
  MutexData() {}
  ~MutexData() {}

  LightLock* mutex() { return &mutex_; }

  LightLock mutex_;

  friend class Mutex;

  DISALLOW_ALLOCATION();
  DISALLOW_COPY_AND_ASSIGN(MutexData);
};

class MonitorData {
 private:
  MonitorData() {}
  ~MonitorData() {}

  LightEvent* event() { return &event_; }
  LightLock* mutex() { return &mutex_; }

  LightEvent event_;
  LightLock mutex_;

  friend class Monitor;

  DISALLOW_ALLOCATION();
  DISALLOW_COPY_AND_ASSIGN(MonitorData);
};

}  // namespace dart

#endif  // RUNTIME_VM_OS_THREAD_CTR_H_

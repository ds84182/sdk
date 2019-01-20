// Copyright (c) 2012, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "platform/globals.h"  // NOLINT

#if defined(HOST_OS_CTR)

#include "vm/os_thread.h"

namespace dart {

const ThreadLocalKey lastTLSKey = 0x100;
static ThreadLocalKey nextTLSKey = 0;
static OSThread::ThreadDestructor tlsKeyDestructors[lastTLSKey + 1] = {NULL};
thread_local uword tlsData[lastTLSKey] = {0};

class ThreadStartData {
 public:
  ThreadStartData(const char* name,
                  OSThread::ThreadStartFunction function,
                  uword parameter)
      : name_(name), function_(function), parameter_(parameter) {}

  const char* name() const { return name_; }
  OSThread::ThreadStartFunction function() const { return function_; }
  uword parameter() const { return parameter_; }

 private:
  const char* name_;
  OSThread::ThreadStartFunction function_;
  uword parameter_;

  DISALLOW_COPY_AND_ASSIGN(ThreadStartData);
};

// Dispatch to the thread start function provided by the caller. This trampoline
// is used to ensure that the thread is properly destroyed if the thread just
// exits.
static void ThreadStart(void* data_ptr) {
  ThreadStartData* data = reinterpret_cast<ThreadStartData*>(data_ptr);

  const char* name = data->name();
  OSThread::ThreadStartFunction function = data->function();
  uword parameter = data->parameter();
  delete data;

  // Create new OSThread object and set as TLS for new thread.
  OSThread* thread = OSThread::CreateOSThread();
  if (thread != NULL) {
    OSThread::SetCurrent(thread);
    thread->set_name(name);
    // Call the supplied thread start function handing it its parameters.
    function(parameter);
    // for (int i=0; i<lastTLSKey; i++) {
    //   tlsKeyDestructors
    // }
  }
}

int OSThread::Start(const char* name,
                    ThreadStartFunction function,
                    uword parameter) {
  ThreadStartData* data = new ThreadStartData(name, function, parameter);

  threadCreate(ThreadStart, data, OSThread::GetMaxStackSize(), 0x30, -1, false);

  return 0;
}

const ThreadId OSThread::kInvalidThreadId = static_cast<ThreadId>(0);
const ThreadJoinId OSThread::kInvalidThreadJoinId =
    static_cast<ThreadJoinId>(0);

ThreadLocalKey OSThread::CreateThreadLocal(ThreadDestructor destructor) {
  ThreadLocalKey key = ++nextTLSKey;
  tlsKeyDestructors[key-1] = destructor;
  return key;
}

void OSThread::DeleteThreadLocal(ThreadLocalKey key) {
  ASSERT(key != kUnsetThreadLocalKey);
  tlsKeyDestructors[key-1] = NULL;
}

void OSThread::SetThreadLocal(ThreadLocalKey key, uword value) {
  ASSERT(key != kUnsetThreadLocalKey);
  tlsData[key-1] = value;
}

intptr_t OSThread::GetMaxStackSize() {
  const int kStackSize = (128 * kWordSize * KB);
  return kStackSize;
}

ThreadId OSThread::GetCurrentThreadId() {
  return threadGetCurrent();
}

#ifdef SUPPORT_TIMELINE
ThreadId OSThread::GetCurrentThreadTraceId() {
  return GetCurrentThreadId();
}
#endif  // PRODUCT

ThreadJoinId OSThread::GetCurrentThreadJoinId(OSThread* thread) {
  ASSERT(thread != NULL);
  // Make sure we're filling in the join id for the current thread.
  ASSERT(thread->id() == GetCurrentThreadId());
  // Make sure the join_id_ hasn't been set, yet.
  DEBUG_ASSERT(thread->join_id_ == kInvalidThreadJoinId);
  ThreadJoinId id = threadGetCurrent();
#if defined(DEBUG)
  thread->join_id_ = id;
#endif
  return id;
}

void OSThread::Join(ThreadJoinId id) {
  Result result = threadJoin(id, 0);
  ASSERT(result == 0);
}

intptr_t OSThread::ThreadIdToIntPtr(ThreadId id) {
  ASSERT(sizeof(id) == sizeof(intptr_t));
  return reinterpret_cast<intptr_t>(id);
}

ThreadId OSThread::ThreadIdFromIntPtr(intptr_t id) {
  return reinterpret_cast<ThreadId>(id);
}

bool OSThread::Compare(ThreadId a, ThreadId b) {
  return a == b;
}

bool OSThread::GetCurrentStackBounds(uword* lower, uword* upper) {
  return false; // TODO Implement
}

#if defined(USING_SAFE_STACK)
NO_SANITIZE_ADDRESS
NO_SANITIZE_SAFE_STACK
uword OSThread::GetCurrentSafestackPointer() {
#error "SAFE_STACK is unsupported on this platform"
  return 0;
}

NO_SANITIZE_ADDRESS
NO_SANITIZE_SAFE_STACK
void OSThread::SetCurrentSafestackPointer(uword ssp) {
#error "SAFE_STACK is unsupported on this platform"
}
#endif

Mutex::Mutex(NOT_IN_PRODUCT(const char* name))
#if !defined(PRODUCT)
    : name_(name)
#endif
{
  LightLock_Init(data_.mutex());

#if defined(DEBUG)
  // When running with assertions enabled we do track the owner.
  owner_ = OSThread::kInvalidThreadId;
#endif  // defined(DEBUG)
}

Mutex::~Mutex() {
#if defined(DEBUG)
  // When running with assertions enabled we do track the owner.
  ASSERT(owner_ == OSThread::kInvalidThreadId);
#endif  // defined(DEBUG)
}

void Mutex::Lock() {
  LightLock_Lock(data_.mutex());
#if defined(DEBUG)
  // When running with assertions enabled we do track the owner.
  owner_ = OSThread::GetCurrentThreadId();
#endif  // defined(DEBUG)
}

bool Mutex::TryLock() {
  // Return false if the lock is busy and locking failed.
  if (LightLock_TryLock(data_.mutex())) {
    return false;
  }
#if defined(DEBUG)
  // When running with assertions enabled we do track the owner.
  owner_ = OSThread::GetCurrentThreadId();
#endif  // defined(DEBUG)
  return true;
}

void Mutex::Unlock() {
#if defined(DEBUG)
  // When running with assertions enabled we do track the owner.
  ASSERT(IsOwnedByCurrentThread());
  owner_ = OSThread::kInvalidThreadId;
#endif  // defined(DEBUG)
  LightLock_Unlock(data_.mutex());
}

Monitor::Monitor() {
  LightEvent_Init(data_.event(), RESET_STICKY);
  LightLock_Init(data_.mutex());

#if defined(DEBUG)
  // When running with assertions enabled we track the owner.
  owner_ = OSThread::kInvalidThreadId;
#endif  // defined(DEBUG)
}

Monitor::~Monitor() {
#if defined(DEBUG)
  // When running with assertions enabled we track the owner.
  ASSERT(owner_ == OSThread::kInvalidThreadId);
#endif  // defined(DEBUG)
}

bool Monitor::TryEnter() {
  // Return false if the lock is busy and locking failed.
  if (LightLock_TryLock(data_.mutex())) {
    return false;
  }
#if defined(DEBUG)
  // When running with assertions enabled we do track the owner.
  owner_ = OSThread::GetCurrentThreadId();
#endif  // defined(DEBUG)
  return true;
}

void Monitor::Enter() {
  LightLock_Lock(data_.mutex());
#if defined(DEBUG)
  // When running with assertions enabled we do track the owner.
  owner_ = OSThread::GetCurrentThreadId();
#endif  // defined(DEBUG)
}

void Monitor::Exit() {
#if defined(DEBUG)
  // When running with assertions enabled we do track the owner.
  ASSERT(IsOwnedByCurrentThread());
  owner_ = OSThread::kInvalidThreadId;
#endif  // defined(DEBUG)
  LightLock_Unlock(data_.mutex());
}

Monitor::WaitResult Monitor::Wait(int64_t millis) {
  return WaitMicros(millis * kMicrosecondsPerMillisecond);
}

Monitor::WaitResult Monitor::WaitMicros(int64_t micros) {
#if defined(DEBUG)
  // When running with assertions enabled we track the owner.
  ASSERT(IsOwnedByCurrentThread());
  ThreadId saved_owner = owner_;
  owner_ = OSThread::kInvalidThreadId;
#endif  // defined(DEBUG)

  Monitor::WaitResult retval = kNotified;
  // if (micros == kNoTimeout) {
  //   // Wait forever.
  //   int result = pthread_cond_wait(data_.cond(), data_.mutex());
  //   VALIDATE_PTHREAD_RESULT(result);
  // } else {
  //   struct timespec ts;
  //   ComputeTimeSpecMicros(&ts, micros);
  //   int result = pthread_cond_timedwait(data_.cond(), data_.mutex(), &ts);
  //   ASSERT((result == 0) || (result == ETIMEDOUT));
  //   if (result == ETIMEDOUT) {
  //     retval = kTimedOut;
  //   }
  // }

#if defined(DEBUG)
  // When running with assertions enabled we track the owner.
  ASSERT(owner_ == OSThread::kInvalidThreadId);
  owner_ = OSThread::GetCurrentThreadId();
  ASSERT(owner_ == saved_owner);
#endif  // defined(DEBUG)
  return retval;
}

void Monitor::Notify() {
  // When running with assertions enabled we track the owner.
  ASSERT(IsOwnedByCurrentThread());
  svcArbitrateAddress(__sync_get_arbiter(), (u32)data_.event(), ARBITRATION_SIGNAL, 1, 0);
}

void Monitor::NotifyAll() {
  // When running with assertions enabled we track the owner.
  ASSERT(IsOwnedByCurrentThread());
  svcArbitrateAddress(__sync_get_arbiter(), (u32)data_.event(), ARBITRATION_SIGNAL, -1, 0);
}

}

#endif

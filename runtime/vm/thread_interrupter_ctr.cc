// Copyright (c) 2013, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "platform/globals.h"
#if defined(HOST_OS_CTR)

#include "vm/flags.h"
#include "vm/os.h"
#include "vm/profiler.h"
#include "vm/signal_handler.h"
#include "vm/thread_interrupter.h"

namespace dart {

#ifndef PRODUCT

DECLARE_FLAG(bool, trace_thread_interrupter);

// TODO: Implement using svcGetDebugThreadContext

// Returns true if the current process is being debugged (either
// running under the debugger or has a debugger attached post facto).
// Code from https://developer.apple.com/library/content/qa/qa1361/_index.html
bool ThreadInterrupter::IsDebuggerAttached() {
  return false;
}

class ThreadInterrupterCTR : public AllStatic {
 public:
};

void ThreadInterrupter::InterruptThread(OSThread* thread) {
  if (FLAG_trace_thread_interrupter) {
    OS::PrintErr("ThreadInterrupter interrupting %p\n", thread->id());
  }
}

void ThreadInterrupter::InstallSignalHandler() {}

void ThreadInterrupter::RemoveSignalHandler() {}

#endif  // !PRODUCT

}  // namespace dart

#endif  // defined(HOST_OS_MACOS)

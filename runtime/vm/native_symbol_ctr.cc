// Copyright (c) 2013, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "platform/globals.h"
#if defined(HOST_OS_CTR)

#include "vm/native_symbol.h"

namespace dart {

void NativeSymbolResolver::Init() {}

void NativeSymbolResolver::Cleanup() {}

char* NativeSymbolResolver::LookupSymbolName(uintptr_t pc, uintptr_t* start) {
  return NULL;
}

void NativeSymbolResolver::FreeSymbolName(char* name) {}

bool NativeSymbolResolver::LookupSharedObject(uword pc,
                                              uword* dso_base,
                                              char** dso_name) {
  return false;
}

}  // namespace dart

#endif  // defined(HOST_OS_CTR)

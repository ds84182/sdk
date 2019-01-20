// Copyright (c) 2016, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#include "vm/globals.h"
#if defined(HOST_OS_CTR)

#include "vm/cpuinfo.h"

#include "platform/assert.h"

namespace dart {

CpuInfoMethod CpuInfo::method_ = kCpuInfoDefault;
const char* CpuInfo::fields_[kCpuInfoMax] = {0};

void CpuInfo::Init() {}

void CpuInfo::Cleanup() {}

bool CpuInfo::FieldContains(CpuInfoIndices idx, const char* search_string) {
  return false;
}

const char* CpuInfo::ExtractField(CpuInfoIndices idx) {
  return strdup("");
}

bool CpuInfo::HasField(const char* field) {
  return false;
}

}  // namespace dart

#endif  // defined(HOST_OS_CTR)

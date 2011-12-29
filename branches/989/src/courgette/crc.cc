// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "courgette/crc.h"

#ifdef COURGETTE_USE_CRC_LIB
#  include "zlib.h"
#else
extern "C" {
#  include "third_party/lzma_sdk/7zCrc.h"
}
#endif

#include "base/basictypes.h"

namespace courgette {

uint32 CalculateCrc(const uint8* buffer, size_t size) {
  uint32 crc;

#ifdef COURGETTE_USE_CRC_LIB
  // Calculate Crc by calling CRC method in zlib
  crc = crc32(0, buffer, size);
#else
  // Calculate Crc by calling CRC method in LZMA SDK
  CrcGenerateTable();
  crc = CrcCalc(buffer, size);
#endif

  return ~crc;
}

}  // namespace

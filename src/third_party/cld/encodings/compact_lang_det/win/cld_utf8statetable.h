// Copyright (c) 2006-2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ENCODINGS_COMPACT_LANG_DET_WIN_CLD_UTF8STATETABLE_H_
#define ENCODINGS_COMPACT_LANG_DET_WIN_CLD_UTF8STATETABLE_H_

#if !defined(CLD_WINDOWS)

#include "util/utf8/utf8statetable.h"

#else

#include "encodings/compact_lang_det/win/cld_basictypes.h"

// These four-byte entries compactly encode how many bytes 0..255 to delete
// in making a string replacement, how many bytes to add 0..255, and the offset
// 0..64k-1 of the replacement string in remap_string.
struct RemapEntry {
  uint8 delete_bytes;
  uint8 add_bytes;
  uint16 bytes_offset;
};

// Exit type codes for state tables. All but the first get stuffed into
// signed one-byte entries. The first is only generated by executable code.
// To distinguish from next-state entries, these must be contiguous and
// all <= kExitNone
typedef enum {
  kExitDstSpaceFull = 239,
  kExitIllegalStructure,  // 240
  kExitOK,                // 241
  kExitReject,            // ...
  kExitReplace1,
  kExitReplace2,
  kExitReplace3,
  kExitReplace21,
  kExitReplace31,
  kExitReplace32,
  kExitReplaceOffset1,
  kExitReplaceOffset2,
  kExitReplace1S0,
  kExitSpecial,
  kExitDoAgain,
  kExitRejectAlt,
  kExitNone               // 255
} ExitReason;

typedef enum {
  kExitDstSpaceFull_2 = -32769,
  kExitIllegalStructure_2,  // -32768
  kExitOK_2,                // -32767
  kExitReject_2,            // ...
  kExitReplace1_2,
  kExitReplace2_2,
  kExitReplace3_2,
  kExitReplace21_2,
  kExitReplace31_2,
  kExitReplace32_2,
  kExitReplaceOffset1_2,
  kExitReplaceOffset2_2,
  kExitReplace1S0_2,
  kExitSpecial_2,
  kExitDoAgain_2,
  kExitRejectAlt_2,
  kExitNone_2               // -32753
} ExitReason_2;

// This struct represents one entire state table. The three initialized byte
// areas are state_table, remap_base, and remap_string. state0 and state0_size
// give the byte offset and length within state_table of the initial state --
// table lookups are expected to start and end in this state, but for
// truncated UTF-8 strings, may end in a different state. These allow a quick
// test for that condition. entry_shift is 8 for tables subscripted by a full
// byte value and 6 for space-optimized tables subscripted by only six
// significant bits in UTF-8 continuation bytes.
typedef struct {
  const uint32 state0;
  const uint32 state0_size;
  const uint32 total_size;
  const int max_expand;
  const int entry_shift;
  const int bytes_per_entry;
  const uint32 losub;
  const uint32 hiadd;
  const uint8* state_table;
  const RemapEntry* remap_base;
  const uint8* remap_string;
  const uint8* fast_state;
} UTF8StateMachineObj;

// Near-duplicate declaration for tables with two-byte entries
typedef struct {
  const uint32 state0;
  const uint32 state0_size;
  const uint32 total_size;
  const int max_expand;
  const int entry_shift;
  const int bytes_per_entry;
  const uint32 losub;
  const uint32 hiadd;
  const signed short* state_table;
  const RemapEntry* remap_base;
  const uint8* remap_string;
  const uint8* fast_state;
} UTF8StateMachineObj_2;


typedef UTF8StateMachineObj UTF8PropObj;
typedef UTF8StateMachineObj UTF8ScanObj;
typedef UTF8StateMachineObj_2 UTF8PropObj_2;


// Look up property of one UTF-8 character and advance over it
// Return 0 if input length is zero
// Return 0 and advance one byte if input is ill-formed
uint8 UTF8GenericProperty(const UTF8PropObj* st,
                          const uint8** src,
                          int* srclen);

// BigOneByte versions are needed for tables > 240 states, but most
// won't need the TwoByte versions.

// Look up property of one UTF-8 character and advance over it
// Return 0 if input length is zero
// Return 0 and advance one byte if input is ill-formed
uint8 UTF8GenericPropertyBigOneByte(const UTF8PropObj* st,
                          const uint8** src,
                          int* srclen);

// Scan a UTF-8 stringpiece based on a state table.
// Always scan complete UTF-8 characters
// Set number of bytes scanned. Return reason for exiting
int UTF8GenericScan(const UTF8ScanObj* st,
                    const uint8* str,
                    const int len,
                    int* bytes_consumed);

#endif

#endif  // ENCODINGS_COMPACT_LANG_DET_WIN_CLD_UTF8STATETABLE_H_

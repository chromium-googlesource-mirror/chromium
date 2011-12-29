// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// TODO(dcheng): For efficiency reasons, consider passing custom data around
// as a vector instead. It allows us to append a
// std::pair<string16, string16> and swap the deserialized values in.

#include "ui/base/clipboard/custom_data_helper.h"

#include <utility>

#include "base/pickle.h"

namespace ui {

namespace {

class SkippablePickle : public Pickle {
 public:
  SkippablePickle(const void* data, size_t data_len);
  bool SkipString16(void** iter);
};

SkippablePickle::SkippablePickle(const void* data, size_t data_len)
    : Pickle(reinterpret_cast<const char*>(data), data_len) {
}

bool SkippablePickle::SkipString16(void** iter) {
  DCHECK(iter);

  int len;
  if (!ReadLength(iter, &len))
    return false;
  if (!IteratorHasRoomFor(*iter, len * sizeof(char16)))
    return false;

  UpdateIter(iter, len * sizeof(char16));
  return true;
}

}  // namespace

void ReadCustomDataTypes(const void* data,
                         size_t data_length,
                         std::vector<string16>* types) {
  SkippablePickle pickle(data, data_length);
  void* iter = NULL;

  size_t size = 0;
  if (!pickle.ReadSize(&iter, &size))
    return;

  // Keep track of the original elements in the types vector. On failure, we
  // truncate the vector to the original size since we want to ignore corrupt
  // custom data pickles.
  size_t original_size = types->size();

  for (size_t i = 0; i < size; ++i) {
    types->push_back(string16());
    if (!pickle.ReadString16(&iter, &types->back()) ||
        !pickle.SkipString16(&iter)) {
      types->resize(original_size);
      return;
    }
  }
}

void ReadCustomDataForType(const void* data,
                           size_t data_length,
                           const string16& type,
                           string16* result) {
  SkippablePickle pickle(data, data_length);
  void* iter = NULL;

  size_t size = 0;
  if (!pickle.ReadSize(&iter, &size))
    return;

  for (size_t i = 0; i < size; ++i) {
    string16 deserialized_type;
    if (!pickle.ReadString16(&iter, &deserialized_type))
      return;
    if (deserialized_type == type) {
      pickle.ReadString16(&iter, result);
      return;
    }
    if (!pickle.SkipString16(&iter))
      return;
  }
}

void ReadCustomDataIntoMap(const void* data,
                           size_t data_length,
                           std::map<string16, string16>* result) {
  Pickle pickle(reinterpret_cast<const char*>(data), data_length);
  void* iter = NULL;

  size_t size = 0;
  if (!pickle.ReadSize(&iter, &size))
    return;

  for (size_t i = 0; i < size; ++i) {
    string16 type;
    if (!pickle.ReadString16(&iter, &type)) {
      // Data is corrupt, return an empty map.
      result->clear();
      return;
    }
    std::pair<std::map<string16, string16>::iterator, bool> insert_result =
        result->insert(std::make_pair(type, string16()));
    if (!pickle.ReadString16(&iter, &insert_result.first->second)) {
      // Data is corrupt, return an empty map.
      result->clear();
      return;
    }
  }
}

void WriteCustomDataToPickle(const std::map<string16, string16>& data,
                             Pickle* pickle) {
  pickle->WriteSize(data.size());
  for (std::map<string16, string16>::const_iterator it = data.begin();
       it != data.end();
       ++it) {
    pickle->WriteString16(it->first);
    pickle->WriteString16(it->second);
  }
}

}  // namespace ui

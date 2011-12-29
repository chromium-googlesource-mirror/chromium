// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEBKIT_MEDIA_ACTIVE_LOADER_H_
#define WEBKIT_MEDIA_ACTIVE_LOADER_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"

namespace WebKit {
class WebURLLoader;
}

namespace webkit_media {

// Wraps an active WebURLLoader with some additional state.
//
// Handles deferring and deletion of loaders.
class ActiveLoader {
 public:
  // Creates an ActiveLoader with the given loader. It is assumed that the
  // initial state of |loader| is loading and not deferred.
  //
  // ActiveLoader takes ownership of |loader|.
  explicit ActiveLoader(WebKit::WebURLLoader* loader);
  ~ActiveLoader();

  // Starts or stops deferring the resource load.
  void SetDeferred(bool deferred);
  bool deferred() { return deferred_; }

  // Cancels the URL loader associated with this object.
  void Cancel();

 private:
  friend class BufferedDataSourceTest;

  scoped_ptr<WebKit::WebURLLoader> loader_;
  bool deferred_;

  DISALLOW_IMPLICIT_CONSTRUCTORS(ActiveLoader);
};

}  // namespace webkit_media

#endif  // WEBKIT_MEDIA_ACTIVE_LOADER_H_

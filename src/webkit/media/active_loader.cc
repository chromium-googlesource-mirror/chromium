// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "webkit/media/active_loader.h"

#include "third_party/WebKit/Source/WebKit/chromium/public/platform/WebURLLoader.h"
#include "webkit/media/buffered_resource_loader.h"

namespace webkit_media {

ActiveLoader::ActiveLoader(WebKit::WebURLLoader* loader)
    : loader_(loader),
      deferred_(false) {
}

ActiveLoader::~ActiveLoader() {
  Cancel();
}

void ActiveLoader::SetDeferred(bool deferred) {
  deferred_ = deferred;
  loader_->setDefersLoading(deferred);
}

void ActiveLoader::Cancel() {
  loader_->cancel();
}

}  // namespace webkit_media

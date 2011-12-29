// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/process_proxy/process_output_watcher.h"

#include <algorithm>
#include <cstdio>

#include <sys/ioctl.h>
#include <sys/select.h>

#include "base/eintr_wrapper.h"
#include "base/logging.h"

namespace {

void InitReadFdSet(int out_fd, int err_fd, int stop_fd, fd_set* set) {
  FD_ZERO(set);
  FD_SET(out_fd, set);
  FD_SET(err_fd, set);
  FD_SET(stop_fd, set);
}

}  // namespace

ProcessOutputWatcher::ProcessOutputWatcher(int out_fd, int err_fd, int stop_fd,
    const ProcessOutputCallback& callback)
    : out_fd_(out_fd),
      err_fd_(err_fd),
      stop_fd_(stop_fd),
      on_read_callback_(callback)  {
  VerifyFileDescriptor(out_fd_);
  VerifyFileDescriptor(err_fd_);
  VerifyFileDescriptor(stop_fd_);
  max_fd_ = std::max(std::max(out_fd_, err_fd_), stop_fd_);
  // We want to be sure we will be able to add 0 at the end of the input, so -1.
  read_buffer_size_ = arraysize(read_buffer_) - 1;
}

void ProcessOutputWatcher::Start() {
  WatchProcessOutput();
  OnStop();
}

ProcessOutputWatcher::~ProcessOutputWatcher() {
  if (out_fd_ >= 0) {
    if (HANDLE_EINTR(close(out_fd_) != 0))
      DPLOG(WARNING) << "close out fd failed.";
  }
  if (err_fd_ >= 0) {
    if (HANDLE_EINTR(close(err_fd_)) != 0)
      DPLOG(WARNING) << "close err fd failed.";
  }
  if (stop_fd_ >= 0) {
    if (HANDLE_EINTR(close(stop_fd_)))
      DPLOG(WARNING) << "close stop fd failed.";
  }
}

void ProcessOutputWatcher::WatchProcessOutput() {
  while (true) {
    // This has to be reset with every watch cycle.
    fd_set rfds;
    InitReadFdSet(out_fd_, err_fd_, stop_fd_, &rfds);

    int select_result =
        HANDLE_EINTR(select(max_fd_ + 1, &rfds, NULL, NULL, NULL));
    if (select_result < 0) {
      DPLOG(WARNING) << "select failed";
      return;
    }

    // Check if we were stopped.
    if (FD_ISSET(stop_fd_, &rfds)) {
      return;
    }

    if (FD_ISSET(err_fd_, &rfds)) {
      ReadFromFd(PROCESS_OUTPUT_TYPE_ERR, err_fd_);
    }

    if (FD_ISSET(out_fd_, &rfds)) {
      ReadFromFd(PROCESS_OUTPUT_TYPE_OUT, out_fd_);
    }
  }
}

void ProcessOutputWatcher::VerifyFileDescriptor(int fd) {
  CHECK_LE(0, fd);
  CHECK_GT(FD_SETSIZE, fd);
}

void ProcessOutputWatcher::ReadFromFd(ProcessOutputType type, int fd) {
  // We don't want to necessary read pipe until it is empty so we don't starve
  // other streams in case data is written faster than we read it. If there is
  // more than read_buffer_size_ bytes in pipe, it will be read in the next
  // iteration.
  ssize_t bytes_read = HANDLE_EINTR(read(fd, read_buffer_, read_buffer_size_));
  if (bytes_read < 0) {
    DPLOG(WARNING) << "read from buffer failed";
     return;
  }

  // Ensure the string in buffer is NULL terminated. |read_buffer_size_| is
  // is set to be smaller than real buffer capacity, so |read_buffer_| won't
  // get overflown.
  read_buffer_[bytes_read] = 0;

  // TODO(tbarzic): We can probably avoid extra string creation.
  if (bytes_read)
    on_read_callback_.Run(type, std::string(read_buffer_));
}

void ProcessOutputWatcher::OnStop() {
  delete this;
}

// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/gpu_scheduler.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/compiler_specific.h"
#include "base/debug/trace_event.h"
#include "base/message_loop.h"
#include "base/time.h"
#include "ui/gfx/gl/gl_bindings.h"
#include "ui/gfx/gl/gl_fence.h"
#include "ui/gfx/gl/gl_switches.h"

using ::base::SharedMemory;

namespace gpu {

GpuScheduler::GpuScheduler(
    CommandBuffer* command_buffer,
    AsyncAPIInterface* handler,
    gles2::GLES2Decoder* decoder)
    : command_buffer_(command_buffer),
      handler_(handler),
      decoder_(decoder),
      parser_(NULL),
      unscheduled_count_(0) {
}

GpuScheduler::~GpuScheduler() {
}

void GpuScheduler::PutChanged() {
  TRACE_EVENT1("gpu", "GpuScheduler:PutChanged", "this", this);

  CommandBuffer::State state = command_buffer_->GetState();

  // If there is no parser, exit.
  if (!parser_.get()) {
    DCHECK_EQ(state.get_offset, state.put_offset);
    return;
  }

  parser_->set_put(state.put_offset);
  if (state.error != error::kNoError)
    return;

  // Check that the GPU has passed all fences.
  if (!PollUnscheduleFences())
    return;

  // One of the unschedule fence tasks might have unscheduled us.
  if (!IsScheduled())
    return;

  error::Error error = error::kNoError;
  while (!parser_->IsEmpty()) {
    DCHECK(IsScheduled());
    DCHECK(unschedule_fences_.empty());

    error = parser_->ProcessCommand();

    // TODO(piman): various classes duplicate various pieces of state, leading
    // to needlessly complex update logic. It should be possible to simply
    // share the state across all of them.
    command_buffer_->SetGetOffset(static_cast<int32>(parser_->get()));

    if (error::IsError(error)) {
      command_buffer_->SetContextLostReason(decoder_->GetContextLostReason());
      command_buffer_->SetParseError(error);
      return;
    }

    if (!command_processed_callback_.is_null())
      command_processed_callback_.Run();

    if (unscheduled_count_ > 0)
      return;
  }
}

void GpuScheduler::SetScheduled(bool scheduled) {
  TRACE_EVENT2("gpu", "GpuScheduler:SetScheduled", "this", this,
               "new unscheduled_count_",
               unscheduled_count_ + (scheduled? -1 : 1));
  if (scheduled) {
    --unscheduled_count_;
    DCHECK_GE(unscheduled_count_, 0);

    if (unscheduled_count_ == 0 && !scheduled_callback_.is_null())
      scheduled_callback_.Run();
  } else {
    ++unscheduled_count_;
  }
}

bool GpuScheduler::IsScheduled() {
  return unscheduled_count_ == 0;
}

bool GpuScheduler::HasMoreWork() {
  return !unschedule_fences_.empty();
}

void GpuScheduler::SetScheduledCallback(
    const base::Closure& scheduled_callback) {
  scheduled_callback_ = scheduled_callback;
}

Buffer GpuScheduler::GetSharedMemoryBuffer(int32 shm_id) {
  return command_buffer_->GetTransferBuffer(shm_id);
}

void GpuScheduler::set_token(int32 token) {
  command_buffer_->SetToken(token);
}

bool GpuScheduler::SetGetBuffer(int32 transfer_buffer_id) {
  Buffer ring_buffer = command_buffer_->GetTransferBuffer(transfer_buffer_id);
  if (!ring_buffer.ptr) {
    return false;
  }

  if (!parser_.get()) {
    parser_.reset(new CommandParser(handler_));
  }

  parser_->SetBuffer(
      ring_buffer.ptr,
      ring_buffer.size,
      0,
      ring_buffer.size);

  SetGetOffset(0);
  return true;
}

bool GpuScheduler::SetGetOffset(int32 offset) {
  if (parser_->set_get(offset)) {
    command_buffer_->SetGetOffset(static_cast<int32>(parser_->get()));
    return true;
  }
  return false;
}

int32 GpuScheduler::GetGetOffset() {
  return parser_->get();
}

void GpuScheduler::SetCommandProcessedCallback(
    const base::Closure& callback) {
  command_processed_callback_ = callback;
}

void GpuScheduler::DeferToFence(base::Closure task) {
  unschedule_fences_.push(make_linked_ptr(
       new UnscheduleFence(gfx::GLFence::Create(), task)));
}

bool GpuScheduler::PollUnscheduleFences() {
  if (unschedule_fences_.empty())
    return true;

  if (unschedule_fences_.front()->fence.get()) {
    while (!unschedule_fences_.empty()) {
      if (unschedule_fences_.front()->fence->HasCompleted()) {
        unschedule_fences_.front()->task.Run();
        unschedule_fences_.pop();
      } else {
        return false;
      }
    }
  } else {
    glFinish();

    while (!unschedule_fences_.empty()) {
      unschedule_fences_.front()->task.Run();
      unschedule_fences_.pop();
    }
  }

  return true;
}

GpuScheduler::UnscheduleFence::UnscheduleFence(
    gfx::GLFence* fence_, base::Closure task_): fence(fence_), task(task_) {
}

GpuScheduler::UnscheduleFence::~UnscheduleFence() {
}

}  // namespace gpu

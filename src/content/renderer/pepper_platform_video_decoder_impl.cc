// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/pepper_platform_video_decoder_impl.h"

#include <vector>

#include "base/bind.h"
#include "base/logging.h"
#include "content/common/child_process.h"
#include "content/renderer/gpu/gpu_channel_host.h"
#include "content/renderer/render_thread_impl.h"

using media::BitstreamBuffer;

PlatformVideoDecoderImpl::PlatformVideoDecoderImpl(
    VideoDecodeAccelerator::Client* client,
    int32 command_buffer_route_id)
    : client_(client),
      command_buffer_route_id_(command_buffer_route_id) {
  DCHECK(client);
}

PlatformVideoDecoderImpl::~PlatformVideoDecoderImpl() {}

bool PlatformVideoDecoderImpl::Initialize(Profile profile) {
  // TODO(vrk): Support multiple decoders.
  if (decoder_)
    return true;

  RenderThreadImpl* render_thread = RenderThreadImpl::current();

  // This is not synchronous, but subsequent IPC messages will be buffered, so
  // it is okay to immediately send IPC messages through the returned channel.
  GpuChannelHost* channel =
      render_thread->EstablishGpuChannelSync(
          content::CAUSE_FOR_GPU_LAUNCH_VIDEODECODEACCELERATOR_INITIALIZE);

  if (!channel)
    return false;

  DCHECK_EQ(channel->state(), GpuChannelHost::kConnected);

  // Send IPC message to initialize decoder in GPU process.
  decoder_ = channel->CreateVideoDecoder(
      command_buffer_route_id_, profile, this);
  return decoder_.get() != NULL;
}

void PlatformVideoDecoderImpl::Decode(const BitstreamBuffer& bitstream_buffer) {
  DCHECK(decoder_);
  decoder_->Decode(bitstream_buffer);
}

void PlatformVideoDecoderImpl::AssignPictureBuffers(
    const std::vector<media::PictureBuffer>& buffers) {
  DCHECK(decoder_);
  decoder_->AssignPictureBuffers(buffers);
}

void PlatformVideoDecoderImpl::ReusePictureBuffer(
    int32 picture_buffer_id) {
  DCHECK(decoder_);
  decoder_->ReusePictureBuffer(picture_buffer_id);
}

void PlatformVideoDecoderImpl::Flush() {
  DCHECK(decoder_);
  decoder_->Flush();
}

void PlatformVideoDecoderImpl::Reset() {
  DCHECK(decoder_);
  decoder_->Reset();
}

void PlatformVideoDecoderImpl::Destroy() {
  DCHECK(decoder_);
  decoder_->Destroy();
  client_ = NULL;
  decoder_ = NULL;
}

void PlatformVideoDecoderImpl::NotifyEndOfStream() {
  DCHECK(RenderThreadImpl::current());
  client_->NotifyEndOfStream();
}

void PlatformVideoDecoderImpl::NotifyError(
    VideoDecodeAccelerator::Error error) {
  DCHECK(RenderThreadImpl::current());
  client_->NotifyError(error);
}

void PlatformVideoDecoderImpl::ProvidePictureBuffers(
    uint32 requested_num_of_buffers,
    const gfx::Size& dimensions) {
  DCHECK(RenderThreadImpl::current());
  client_->ProvidePictureBuffers(requested_num_of_buffers, dimensions);
}

void PlatformVideoDecoderImpl::DismissPictureBuffer(int32 picture_buffer_id) {
  DCHECK(RenderThreadImpl::current());
  client_->DismissPictureBuffer(picture_buffer_id);
}

void PlatformVideoDecoderImpl::PictureReady(const media::Picture& picture) {
  DCHECK(RenderThreadImpl::current());
  client_->PictureReady(picture);
}

void PlatformVideoDecoderImpl::NotifyInitializeDone() {
  NOTREACHED() << "GpuVideoDecodeAcceleratorHost::Initialize is synchronous!";
}

void PlatformVideoDecoderImpl::NotifyEndOfBitstreamBuffer(
  int32 bitstream_buffer_id) {
  DCHECK(RenderThreadImpl::current());
  client_->NotifyEndOfBitstreamBuffer(bitstream_buffer_id);
}

void PlatformVideoDecoderImpl::NotifyFlushDone() {
  DCHECK(RenderThreadImpl::current());
  client_->NotifyFlushDone();
}

void PlatformVideoDecoderImpl::NotifyResetDone() {
  DCHECK(RenderThreadImpl::current());
  client_->NotifyResetDone();
}

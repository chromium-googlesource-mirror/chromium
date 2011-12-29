// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef REMOTING_BASE_DECODER_H_
#define REMOTING_BASE_DECODER_H_

#include <vector>

#include "base/memory/scoped_ptr.h"
#include "media/base/video_frame.h"
#include "remoting/proto/video.pb.h"
#include "third_party/skia/include/core/SkRect.h"

namespace remoting {

typedef std::vector<SkIRect> RectVector;

// Interface for a decoder that takes a stream of bytes from the network and
// outputs frames of data.
//
// TODO(ajwong): Beef up this documentation once the API stablizes.
class Decoder {
 public:
  // DecodeResult is returned from DecodePacket() and indicates current state
  // of the decoder. DECODE_DONE means that last packet for the frame was
  // processed, and the frame can be displayed now. DECODE_IN_PROGRESS
  // indicates that the decoder must receive more data before the frame can be
  // displayed. DECODE_ERROR is returned if there was an error in the stream.
  enum DecodeResult {
    DECODE_ERROR = -1,
    DECODE_IN_PROGRESS,
    DECODE_DONE,
  };

  Decoder() {}
  virtual ~Decoder() {}

  // Initializes the decoder to draw into the given |frame|.
  virtual void Initialize(scoped_refptr<media::VideoFrame> frame) = 0;

  // Feeds more data into the decoder.
  virtual DecodeResult DecodePacket(const VideoPacket* packet) = 0;

  // Returns rects that were updated in the last frame. Can be called only
  // after DecodePacket returned DECODE_DONE. Caller keeps ownership of
  // |rects|. |rects| is kept empty if whole screen needs to be updated.
  virtual void GetUpdatedRects(RectVector* rects) = 0;

  // Reset the decoder to an uninitialized state. Release all references to
  // the initialized |frame|.  Initialize() must be called before the decoder
  // is used again.
  virtual void Reset() = 0;

  // Returns true if decoder is ready to accept data via DecodePacket.
  virtual bool IsReadyForData() = 0;

  virtual VideoPacketFormat::Encoding Encoding() = 0;

  // Set the output dimensions for the decoder.  If the dimensions are empty
  // then the source is rendered without scaling.
  // Output dimensions are ignored if the decoder doesn't support scaling.
  virtual void SetOutputSize(const SkISize& size) {}

  // Set the clipping rectangle to the decoder. Decoder should respect this and
  // only output changes in this rectangle. The new clipping rectangle will be
  // effective on the next decoded video frame.
  //
  // When scaling is enabled clipping rectangles are ignored.
  virtual void SetClipRect(const SkIRect& clip_rect) {}

  // Force decoder to output a video frame with content in |rects| using the
  // last decoded video frame.  |rects| are expressed in video frame rather
  // than output coordinates.
  virtual void RefreshRects(const RectVector& rects) {}
};

}  // namespace remoting

#endif  // REMOTING_BASE_DECODER_H_

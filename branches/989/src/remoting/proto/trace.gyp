# Copyright (c) 2010 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'trace_proto_lib',
      'type': 'static_library',
      'sources': [
        'trace.proto',
      ],
      'variables': {
        'proto_out_dir': 'remoting/proto',
      },
      'includes': ['../../build/protoc.gypi'],
    },
  ],
}

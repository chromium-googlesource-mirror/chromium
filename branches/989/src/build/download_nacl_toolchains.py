#!/usr/bin/env python
# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Shim to run nacl toolchain download script only if there is a nacl dir."""

import os
import sys


def Main(args):
  # Exit early if disable_nacl=1.
  if 'disable_nacl=1' in os.environ.get('GYP_DEFINES', ''):
    return 0
  # Handle chromeos=1 specially (until its building its own toolchain).
  if 'chromeos=1' in os.environ.get('GYP_DEFINES', ''):
    args = [
      '--nacl-newlib-only',
      '--file-hash', 'linux_x86_newlib',
           '1deb316302fde89a2200dff6550cf510ae90b89b',
      '--base-url', ('https://commondatastorage.googleapis.com/'
                     'nativeclient-archive2/special_chromeos'),
      '--x86-version', '7258',
    ]
    print 'NOTE: Special handling for chromeos'
    print 'Running with these argument instead:'
    print args
  script_dir = os.path.dirname(os.path.abspath(__file__))
  src_dir = os.path.dirname(script_dir)
  nacl_dir = os.path.join(src_dir, 'native_client')
  nacl_build_dir = os.path.join(nacl_dir, 'build')
  download_script = os.path.join(nacl_build_dir, 'download_toolchains.py')
  if not os.path.exists(download_script):
    print "Can't find '%s'" % download_script
    print 'Presumably you are intentionally building without NativeClient.'
    print 'Skipping NativeClient toolchain download.'
    sys.exit(0)
  sys.path.insert(0, nacl_build_dir)
  import download_toolchains
  download_toolchains.Main(args)
  return 0


if __name__ == '__main__':
  sys.exit(Main(sys.argv[1:]))

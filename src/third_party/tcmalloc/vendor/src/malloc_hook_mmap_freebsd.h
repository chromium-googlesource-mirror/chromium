// Copyright (c) 2011, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Override mmap/munmap/mremap/sbrk to provide support for calling the
// related hooks (in addition, of course, to doing what these
// functions normally do).

#ifndef __FreeBSD__
# error Should only be including malloc_hook_mmap_freebsd.h on FreeBSD systems.
#endif

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <errno.h>

static inline void* do_mmap(void *start, size_t length,
                            int prot, int flags,
                            int fd, off_t offset) __THROW {
  return (void *)syscall(SYS_mmap, start, length, prot, flags, fd, offset);
}

// Make sure mmap doesn't get #define'd away by <sys/mman.h>
#undef mmap

extern "C" {
  void* mmap(void *start, size_t length,int prot, int flags,
             int fd, off_t offset) __THROW
    ATTRIBUTE_SECTION(malloc_hook);
  int munmap(void* start, size_t length) __THROW
    ATTRIBUTE_SECTION(malloc_hook);
  void* sbrk(intptr_t increment) __THROW
    ATTRIBUTE_SECTION(malloc_hook);
}

static inline void* do_sbrk(intptr_t increment) {
  void* curbrk;

  __asm__ __volatile__(
      "movl .curbrk, %%eax;"
      "movl %%eax, %0"
      : "=r" (curbrk)
      :: "%eax");

  char* prevbrk = static_cast<char*>(curbrk);
  void* newbrk = prevbrk + increment;

  if (brk(newbrk) == -1) {
    assert(0);
    return reinterpret_cast<void*>(static_cast<intptr_t>(-1));
  }

  return prevbrk;
}


extern "C" void* mmap(void *start, size_t length, int prot, int flags,
                      int fd, off_t offset) __THROW {
  MallocHook::InvokePreMmapHook(start, length, prot, flags, fd, offset);
  void *result = do_mmap(start, length, prot, flags, fd,
                         static_cast<size_t>(offset)); // avoid sign extension
  MallocHook::InvokeMmapHook(result, start, length, prot, flags, fd, offset);
  return result;
}

extern "C" int munmap(void* start, size_t length) __THROW {
  MallocHook::InvokeMunmapHook(start, length);
  return syscall(SYS_munmap, start, length);
}

extern "C" void* sbrk(intptr_t increment) __THROW {
  MallocHook::InvokePreSbrkHook(increment);
  void *result = do_sbrk(increment);
  MallocHook::InvokeSbrkHook(result, increment);
  return result;
}

/*static*/void* MallocHook::UnhookedMMap(void *start, size_t length, int prot,
                                         int flags, int fd, off_t offset) {
  return mmap(start, length, prot, flags, fd, offset);
}

/*static*/int MallocHook::UnhookedMUnmap(void *start, size_t length) {
  return munmap(start, length);
}

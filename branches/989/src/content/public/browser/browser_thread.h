// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_PUBLIC_BROWSER_BROWSER_THREAD_H_
#define CONTENT_PUBLIC_BROWSER_BROWSER_THREAD_H_
#pragma once

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/task.h"
#include "base/tracked_objects.h"
#include "content/common/content_export.h"
#include "content/public/browser/browser_thread_delegate.h"

#if defined(UNIT_TEST)
#include "base/logging.h"
#endif  // UNIT_TEST

class MessageLoop;

namespace base {
class MessageLoopProxy;
class Thread;
}

namespace content {

class BrowserThreadImpl;

///////////////////////////////////////////////////////////////////////////////
// BrowserThread
//
// Utility functions for threads that are known by a browser-wide
// name.  For example, there is one IO thread for the entire browser
// process, and various pieces of code find it useful to retrieve a
// pointer to the IO thread's message loop.
//
// Invoke a task by thread ID:
//
//   BrowserThread::PostTask(BrowserThread::IO, FROM_HERE, task);
//
// The return value is false if the task couldn't be posted because the target
// thread doesn't exist.  If this could lead to data loss, you need to check the
// result and restructure the code to ensure it doesn't occur.
//
// This class automatically handles the lifetime of different threads.
// It's always safe to call PostTask on any thread.  If it's not yet created,
// the task is deleted.  There are no race conditions.  If the thread that the
// task is posted to is guaranteed to outlive the current thread, then no locks
// are used.  You should never need to cache pointers to MessageLoops, since
// they're not thread safe.
class CONTENT_EXPORT BrowserThread {
 public:
  // An enumeration of the well-known threads.
  // NOTE: threads must be listed in the order of their life-time, with each
  // thread outliving every other thread below it.
  enum ID {
    // The main thread in the browser.
    UI,

    // This is the thread that interacts with the database.
    DB,

    // This is the "main" thread for WebKit within the browser process when
    // NOT in --single-process mode.
    // Deprecated: Do not design new code to use this thread; see
    // http://crbug.com/106839
    WEBKIT_DEPRECATED,

    // This is the thread that interacts with the file system.
    FILE,

    // Used for file system operations that block user interactions.
    // Responsiveness of this thread affect users.
    FILE_USER_BLOCKING,

    // Used to launch and terminate Chrome processes.
    PROCESS_LAUNCHER,

    // This is the thread to handle slow HTTP cache operations.
    CACHE,

    // This is the thread that processes IPC and network messages.
    IO,

#if defined(OS_CHROMEOS)
    // This thread runs websocket to TCP proxy.
    // TODO(dilmah): remove this thread, instead implement this functionality
    // as hooks into websocket layer.
    WEB_SOCKET_PROXY,
#endif

    // This identifier does not represent a thread.  Instead it counts the
    // number of well-known threads.  Insert new well-known threads before this
    // identifier.
    ID_COUNT
  };

  // These are the same methods in message_loop.h, but are guaranteed to either
  // get posted to the MessageLoop if it's still alive, or be deleted otherwise.
  // They return true iff the thread existed and the task was posted.  Note that
  // even if the task is posted, there's no guarantee that it will run, since
  // the target thread may already have a Quit message in its queue.
  static bool PostTask(ID identifier,
                       const tracked_objects::Location& from_here,
                       const base::Closure& task);
  static bool PostDelayedTask(ID identifier,
                              const tracked_objects::Location& from_here,
                              const base::Closure& task,
                              int64 delay_ms);
  static bool PostNonNestableTask(ID identifier,
                                  const tracked_objects::Location& from_here,
                                  const base::Closure& task);
  static bool PostNonNestableDelayedTask(
      ID identifier,
      const tracked_objects::Location& from_here,
      const base::Closure& task,
      int64 delay_ms);

  // TODO(brettw) remove these when Task->Closure conversion is done.
  static bool PostTask(ID identifier,
                       const tracked_objects::Location& from_here,
                       Task* task);
  static bool PostDelayedTask(ID identifier,
                              const tracked_objects::Location& from_here,
                              Task* task,
                              int64 delay_ms);
  static bool PostNonNestableTask(ID identifier,
                                  const tracked_objects::Location& from_here,
                                  Task* task);
  static bool PostNonNestableDelayedTask(
      ID identifier,
      const tracked_objects::Location& from_here,
      Task* task,
      int64 delay_ms);

  static bool PostTaskAndReply(
      ID identifier,
      const tracked_objects::Location& from_here,
      const base::Closure& task,
      const base::Closure& reply);

  template <class T>
  static bool DeleteSoon(ID identifier,
                         const tracked_objects::Location& from_here,
                         const T* object) {
    return PostNonNestableTask(
        identifier, from_here, new DeleteTask<T>(object));
  }

  template <class T>
  static bool ReleaseSoon(ID identifier,
                          const tracked_objects::Location& from_here,
                          const T* object) {
    return PostNonNestableTask(
        identifier, from_here, new ReleaseTask<T>(object));
  }

  // Callable on any thread.  Returns whether the given ID corresponds to a well
  // known thread.
  static bool IsWellKnownThread(ID identifier);

  // Callable on any thread.  Returns whether you're currently on a particular
  // thread.
  static bool CurrentlyOn(ID identifier);

  // Callable on any thread.  Returns whether the threads message loop is valid.
  // If this returns false it means the thread is in the process of shutting
  // down.
  static bool IsMessageLoopValid(ID identifier);

  // If the current message loop is one of the known threads, returns true and
  // sets identifier to its ID.  Otherwise returns false.
  static bool GetCurrentThreadIdentifier(ID* identifier);

  // Callers can hold on to a refcounted MessageLoopProxy beyond the lifetime
  // of the thread.
  static scoped_refptr<base::MessageLoopProxy> GetMessageLoopProxyForThread(
      ID identifier);

  // Returns a pointer to the thread's message loop, which will become
  // invalid during shutdown, so you probably shouldn't hold onto it.
  //
  // This must not be called before the thread is started, or after
  // the thread is stopped, or it will DCHECK.
  //
  // Ownership remains with the BrowserThread implementation, so you
  // must not delete the pointer.
  static MessageLoop* UnsafeGetMessageLoopForThread(ID identifier);

  // Sets the delegate for the specified BrowserThread.
  //
  // Only one delegate may be registered at a time.  Delegates may be
  // unregistered by providing a NULL pointer.
  //
  // If the caller unregisters a delegate before CleanUp has been
  // called, it must perform its own locking to ensure the delegate is
  // not deleted while unregistering.
  static void SetDelegate(ID identifier, BrowserThreadDelegate* delegate);

  // Use these templates in conjuction with RefCountedThreadSafe when you want
  // to ensure that an object is deleted on a specific thread.  This is needed
  // when an object can hop between threads (i.e. IO -> FILE -> IO), and thread
  // switching delays can mean that the final IO tasks executes before the FILE
  // task's stack unwinds.  This would lead to the object destructing on the
  // FILE thread, which often is not what you want (i.e. to unregister from
  // NotificationService, to notify other objects on the creating thread etc).
  template<ID thread>
  struct DeleteOnThread {
    template<typename T>
    static void Destruct(const T* x) {
      if (CurrentlyOn(thread)) {
        delete x;
      } else {
        if (!DeleteSoon(thread, FROM_HERE, x)) {
#if defined(UNIT_TEST)
          // Only logged under unit testing because leaks at shutdown
          // are acceptable under normal circumstances.
          LOG(ERROR) << "DeleteSoon failed on thread " << thread;
#endif  // UNIT_TEST
        }
      }
    }
  };

  // Sample usage:
  // class Foo
  //     : public base::RefCountedThreadSafe<
  //           Foo, BrowserThread::DeleteOnIOThread> {
  //
  // ...
  //  private:
  //   friend struct BrowserThread::DeleteOnThread<BrowserThread::IO>;
  //   friend class DeleteTask<Foo>;
  //
  //   ~Foo();
  struct DeleteOnUIThread : public DeleteOnThread<UI> { };
  struct DeleteOnIOThread : public DeleteOnThread<IO> { };
  struct DeleteOnFileThread : public DeleteOnThread<FILE> { };
  struct DeleteOnDBThread : public DeleteOnThread<DB> { };
  struct DeleteOnWebKitThread : public DeleteOnThread<WEBKIT_DEPRECATED> { };

 private:
  friend class BrowserThreadImpl;

  BrowserThread() {}
  DISALLOW_COPY_AND_ASSIGN(BrowserThread);
};

}  // namespace content

#endif  // CONTENT_PUBLIC_BROWSER_BROWSER_THREAD_H_

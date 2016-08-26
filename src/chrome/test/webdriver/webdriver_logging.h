// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_TEST_WEBDRIVER_WEBDRIVER_LOGGING_H_
#define CHROME_TEST_WEBDRIVER_WEBDRIVER_LOGGING_H_

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/memory/scoped_ptr.h"
#include "base/synchronization/lock.h"
#include "base/values.h"

namespace base {
class Time;
}

namespace webdriver {

// WebDriver logging levels.
enum LogLevel {
  kSevereLogLevel = 1000,
  kWarningLogLevel = 900,
  kInfoLogLevel = 800,
  kFineLogLevel = 500,
  kFinerLogLevel = 400,
  kAllLogLevel = -1000
};

// Represents a type/source of a WebDriver log.
class LogType {
 public:
  enum Type {
    kInvalid = -1,
    kDriver,
    kNum  // must be correct
  };

  static bool FromString(const std::string& name, LogType* log_type);

  LogType();
  explicit LogType(Type type);
  ~LogType();

  std::string ToString() const;
  Type type() const;

 private:
  Type type_;
};

class LogHandler {
 public:
  LogHandler();
  virtual ~LogHandler();

  virtual void Log(LogLevel level, const base::Time& time,
                   const std::string& message) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(LogHandler);
};

class FileLog : public LogHandler {
 public:
  static void InitGlobalLog(LogLevel level);
  static FileLog* Get();

  FileLog(const FilePath& path, LogLevel level);
  virtual ~FileLog();

  virtual void Log(LogLevel level, const base::Time& time,
                   const std::string& message) OVERRIDE;

  bool GetLogContents(std::string* contents);

  void set_min_log_level(LogLevel level);

 private:
  static FileLog* singleton_;

  FilePath path_;
  file_util::ScopedFILE file_;
  base::Lock lock_;

  LogLevel min_log_level_;

  DISALLOW_COPY_AND_ASSIGN(FileLog);
};

class InMemoryLog : public LogHandler {
 public:
  InMemoryLog();
  virtual ~InMemoryLog();

  virtual void Log(LogLevel level, const base::Time& time,
                   const std::string& message) OVERRIDE;

  const base::ListValue* entries_list() const;

 private:
  base::ListValue entries_list_;
  base::Lock entries_lock_;

  DISALLOW_COPY_AND_ASSIGN(InMemoryLog);
};

// Forwards logging messages to added logs.
class Logger {
 public:
  Logger();
  explicit Logger(LogLevel level);
  ~Logger();

  void Log(LogLevel level, const std::string& message) const;
  void AddHandler(LogHandler* log_handler);

  void set_min_log_level(LogLevel level);

 private:
  std::vector<LogHandler*> handlers_;
  LogLevel min_log_level_;
};

// Initializes logging for WebDriver. All logging below the given level
// will be discarded in the global file log.
void InitWebDriverLogging(LogLevel min_log_level);

}  // namespace webdriver

#endif  // CHROME_TEST_WEBDRIVER_WEBDRIVER_LOGGING_H_
// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/file_path.h"
#include "base/logging.h"
#include "base/message_loop.h"
#include "base/test/test_timeouts.h"
#include "base/utf_string_conversions.h"
#include "net/test/test_server.h"

static void PrintUsage() {
  printf("run_testserver --doc-root=relpath [--http|--https|--ftp|--sync]\n"
         "               [--https-cert=ok|mismatched-name|expired]\n");
  printf("(NOTE: relpath should be relative to the 'src' directory)\n");
}

int main(int argc, const char* argv[]) {
  base::AtExitManager at_exit_manager;
  MessageLoopForIO message_loop;

  // Process command line
  CommandLine::Init(argc, argv);
  CommandLine* command_line = CommandLine::ForCurrentProcess();

  if (!logging::InitLogging(
          FILE_PATH_LITERAL("testserver.log"),
          logging::LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG,
          logging::LOCK_LOG_FILE,
          logging::APPEND_TO_OLD_LOG_FILE,
          logging::DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS)) {
    printf("Error: could not initialize logging. Exiting.\n");
    return -1;
  }

  TestTimeouts::Initialize();

  if (command_line->GetSwitches().empty() || command_line->HasSwitch("help")) {
    PrintUsage();
    return -1;
  }

  net::TestServer::Type server_type(net::TestServer::TYPE_HTTP);
  if (command_line->HasSwitch("https")) {
    server_type = net::TestServer::TYPE_HTTPS;
  } else if (command_line->HasSwitch("ftp")) {
    server_type = net::TestServer::TYPE_FTP;
  } else if (command_line->HasSwitch("sync")) {
    server_type = net::TestServer::TYPE_SYNC;
  }

  net::TestServer::HTTPSOptions https_options;
  if (command_line->HasSwitch("https-cert")) {
    server_type = net::TestServer::TYPE_HTTPS;
    std::string cert_option = command_line->GetSwitchValueASCII("https-cert");
    if (cert_option == "ok") {
      https_options.server_certificate = net::TestServer::HTTPSOptions::CERT_OK;
    } else if (cert_option == "mismatched-name") {
      https_options.server_certificate =
          net::TestServer::HTTPSOptions::CERT_MISMATCHED_NAME;
    } else if (cert_option == "expired") {
      https_options.server_certificate =
          net::TestServer::HTTPSOptions::CERT_EXPIRED;
    } else {
      printf("Error: --https-cert has invalid value %s\n", cert_option.c_str());
      PrintUsage();
      return -1;
    }
  }

  FilePath doc_root = command_line->GetSwitchValuePath("doc-root");
  if ((server_type != net::TestServer::TYPE_SYNC) && doc_root.empty()) {
    printf("Error: --doc-root must be specified\n");
    PrintUsage();
    return -1;
  }

  scoped_ptr<net::TestServer> test_server;
  if (server_type == net::TestServer::TYPE_HTTPS)
    test_server.reset(new net::TestServer(https_options, doc_root));
  else
    test_server.reset(new net::TestServer(server_type, doc_root));

  if (!test_server->Start()) {
    printf("Error: failed to start test server. Exiting.\n");
    return -1;
  }

  if (!file_util::DirectoryExists(test_server->document_root())) {
    printf("Error: invalid doc root: \"%s\" does not exist!\n",
        UTF16ToUTF8(test_server->document_root().LossyDisplayName()).c_str());
    return -1;
  }

  printf("testserver running at %s (type ctrl+c to exit)\n",
         test_server->host_port_pair().ToString().c_str());

  message_loop.Run();
  return 0;
}

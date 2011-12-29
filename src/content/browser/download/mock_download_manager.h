// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_DOWNLOAD_MOCK_DOWNLOAD_MANAGER_H_
#define CONTENT_BROWSER_DOWNLOAD_MOCK_DOWNLOAD_MANAGER_H_
#pragma once

#include "content/browser/download/download_item_impl.h"
#include "content/browser/download/download_id.h"
#include "content/browser/download/download_id_factory.h"
#include "content/public/browser/download_manager.h"

class DownloadStatusUpdater;

class MockDownloadManager : public content::DownloadManager {
 public:
  explicit MockDownloadManager(content::DownloadManagerDelegate* delegate,
                               DownloadIdFactory* id_factory,
                               DownloadStatusUpdater* updater);
  virtual ~MockDownloadManager();

  // DownloadManager:
  virtual void Shutdown() OVERRIDE;
  virtual void GetTemporaryDownloads(const FilePath& dir_path,
                                     DownloadVector* result) OVERRIDE;
  virtual void GetAllDownloads(const FilePath& dir_path,
                               DownloadVector* result) OVERRIDE;
  virtual void SearchDownloads(const string16& query,
                               DownloadVector* result) OVERRIDE;
  virtual bool Init(content::BrowserContext* browser_context) OVERRIDE;
  virtual void StartDownload(int32 id) OVERRIDE;
  virtual void UpdateDownload(int32 download_id,
                              int64 bytes_so_far,
                              int64 bytes_per_sec,
                              std::string hash_state) OVERRIDE;
  virtual void OnResponseCompleted(int32 download_id, int64 size,
                                   const std::string& hash) OVERRIDE;
  virtual void CancelDownload(int32 download_id) OVERRIDE;
  virtual void OnDownloadInterrupted(int32 download_id,
                                     int64 size,
                                     std::string hash_state,
                                     InterruptReason reason) OVERRIDE;
  virtual void OnDownloadRenamedToFinalName(int download_id,
                                            const FilePath& full_path,
                                            int uniquifier) OVERRIDE;
  virtual int RemoveDownloadsBetween(const base::Time remove_begin,
                                     const base::Time remove_end) OVERRIDE;
  virtual int RemoveDownloads(const base::Time remove_begin) OVERRIDE;
  virtual int RemoveAllDownloads() OVERRIDE;
  virtual void DownloadUrl(const GURL& url,
                           const GURL& referrer,
                           const std::string& referrer_encoding,
                           TabContents* tab_contents) OVERRIDE;
  virtual void DownloadUrlToFile(const GURL& url,
                                 const GURL& referrer,
                                 const std::string& referrer_encoding,
                                 const DownloadSaveInfo& save_info,
                                 TabContents* tab_contents) OVERRIDE;
  virtual void AddObserver(Observer* observer) OVERRIDE;
  virtual void RemoveObserver(Observer* observer) OVERRIDE;
  virtual void OnPersistentStoreQueryComplete(
      std::vector<DownloadPersistentStoreInfo>* entries) OVERRIDE;
  virtual void OnItemAddedToPersistentStore(int32 download_id,
                                            int64 db_handle) OVERRIDE;
  virtual int InProgressCount() const OVERRIDE;
  virtual content::BrowserContext* GetBrowserContext() const OVERRIDE;
  virtual FilePath LastDownloadPath() OVERRIDE;
  virtual void CreateDownloadItem(
      DownloadCreateInfo* info,
      const DownloadRequestHandle& request_handle) OVERRIDE;
  virtual content::DownloadItem* CreateSavePackageDownloadItem(
      const FilePath& main_file_path,
      const GURL& page_url,
      bool is_otr,
      content::DownloadItem::Observer* observer) OVERRIDE;
  virtual void ClearLastDownloadPath() OVERRIDE;
  virtual void FileSelected(const FilePath& path, void* params) OVERRIDE;
  virtual void FileSelectionCanceled(void* params) OVERRIDE;
  virtual void RestartDownload(int32 download_id) OVERRIDE;
  virtual void CheckForHistoryFilesRemoval() OVERRIDE;
  virtual content::DownloadItem* GetDownloadItem(int id) OVERRIDE;
  virtual void SavePageDownloadFinished(
      content::DownloadItem* download) OVERRIDE;
  virtual content::DownloadItem* GetActiveDownloadItem(int id) OVERRIDE;
  virtual content::DownloadManagerDelegate* delegate() const OVERRIDE;
  virtual void SetDownloadManagerDelegate(
      content::DownloadManagerDelegate* delegate) OVERRIDE;
  virtual void ContinueDownloadWithPath(content::DownloadItem* download,
                                        const FilePath& chosen_file) OVERRIDE;
  virtual content::DownloadItem* GetActiveDownload(int32 download_id) OVERRIDE;
  virtual void SetFileManager(DownloadFileManager* file_manager) OVERRIDE;

 private:
  content::DownloadManagerDelegate* delegate_;
  DownloadIdFactory* id_factory_;
  DownloadStatusUpdater* updater_;
  DownloadFileManager* file_manager_;
  std::map<int32, content::DownloadItem*> item_map_;
  std::map<int32, content::DownloadItem*> inactive_item_map_;
};

#endif  // CONTENT_BROWSER_DOWNLOAD_MOCK_DOWNLOAD_MANAGER_H_

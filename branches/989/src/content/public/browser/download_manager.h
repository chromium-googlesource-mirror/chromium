// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// The DownloadManager object manages the process of downloading, including
// updates to the history system and providing the information for displaying
// the downloads view in the Destinations tab. There is one DownloadManager per
// active browser context in Chrome.
//
// Download observers:
// Objects that are interested in notifications about new downloads, or progress
// updates for a given download must implement one of the download observer
// interfaces:
//   DownloadManager::Observer:
//     - allows observers, primarily views, to be notified when changes to the
//       set of all downloads (such as new downloads, or deletes) occur
// Use AddObserver() / RemoveObserver() on the appropriate download object to
// receive state updates.
//
// Download state persistence:
// The DownloadManager uses the history service for storing persistent
// information about the state of all downloads. The history system maintains a
// separate table for this called 'downloads'. At the point that the
// DownloadManager is constructed, we query the history service for the state of
// all persisted downloads.

#ifndef CONTENT_PUBLIC_BROWSER_DOWNLOAD_MANAGER_H_
#define CONTENT_PUBLIC_BROWSER_DOWNLOAD_MANAGER_H_
#pragma once

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/file_path.h"
#include "base/gtest_prod_util.h"
#include "base/time.h"
#include "content/browser/download/download_id.h"
#include "content/browser/download/interrupt_reasons.h"
#include "content/public/browser/download_item.h"
#include "content/public/browser/browser_thread.h"
#include "net/base/net_errors.h"

class DownloadFileManager;
class DownloadManagerTest;
class DownloadRequestHandle;
class GURL;
class TabContents;
struct DownloadCreateInfo;
struct DownloadSaveInfo;

namespace content {
class BrowserContext;
class DownloadManagerDelegate;

// Browser's download manager: manages all downloads and destination view.
class CONTENT_EXPORT DownloadManager
    : public base::RefCountedThreadSafe<
          DownloadManager, content::BrowserThread::DeleteOnUIThread> {
 public:
  virtual ~DownloadManager() {}

  // Shutdown the download manager. Must be called before destruction.
  virtual void Shutdown() = 0;

  // Interface to implement for observers that wish to be informed of changes
  // to the DownloadManager's collection of downloads.
  class CONTENT_EXPORT Observer {
   public:
    // New or deleted download, observers should query us for the current set
    // of downloads.
    virtual void ModelChanged() = 0;

    // Called when the DownloadManager is being destroyed to prevent Observers
    // from calling back to a stale pointer.
    virtual void ManagerGoingDown() {}

    // Called immediately after the DownloadManager puts up a select file
    // dialog.
    // |id| indicates which download opened the dialog.
    virtual void SelectFileDialogDisplayed(int32 id) {}

   protected:
    virtual ~Observer() {}
  };

  typedef std::vector<DownloadItem*> DownloadVector;

  // Return all temporary downloads that reside in the specified directory.
  virtual void GetTemporaryDownloads(const FilePath& dir_path,
                                     DownloadVector* result) = 0;

  // Return all non-temporary downloads in the specified directory that are
  // are in progress or have completed.
  virtual void GetAllDownloads(const FilePath& dir_path,
                               DownloadVector* result) = 0;

  // Returns all non-temporary downloads matching |query|. Empty query matches
  // everything.
  virtual void SearchDownloads(const string16& query,
                               DownloadVector* result) = 0;

  // Returns true if initialized properly.
  virtual bool Init(content::BrowserContext* browser_context) = 0;

  // Notifications sent from the download thread to the UI thread
  virtual void StartDownload(int32 id) = 0;
  virtual void UpdateDownload(int32 download_id,
                              int64 bytes_so_far,
                              int64 bytes_per_sec,
                              std::string hash_state) = 0;

  // |download_id| is the ID of the download.
  // |size| is the number of bytes that have been downloaded.
  // |hash| is sha256 hash for the downloaded file. It is empty when the hash
  // is not available.
  virtual void OnResponseCompleted(int32 download_id, int64 size,
                           const std::string& hash) = 0;

  // Offthread target for cancelling a particular download.  Will be a no-op
  // if the download has already been cancelled.
  virtual void CancelDownload(int32 download_id) = 0;

  // Called when there is an error in the download.
  // |download_id| is the ID of the download.
  // |size| is the number of bytes that are currently downloaded.
  // |hash_state| is the current state of the hash of the data that has been
  // downloaded.
  // |reason| is a download interrupt reason code.
  virtual void OnDownloadInterrupted(int32 download_id,
                                     int64 size,
                                     std::string hash_state,
                                     InterruptReason reason) = 0;

  // Called when the download is renamed to its final name.
  // |uniquifier| is a number used to make unique names for the file.  It is
  // only valid for the DANGEROUS_BUT_VALIDATED state of the download item.
  virtual void OnDownloadRenamedToFinalName(int download_id,
                                    const FilePath& full_path,
                                    int uniquifier) = 0;

  // Remove downloads after remove_begin (inclusive) and before remove_end
  // (exclusive). You may pass in null Time values to do an unbounded delete
  // in either direction.
  virtual int RemoveDownloadsBetween(const base::Time remove_begin,
                             const base::Time remove_end) = 0;

  // Remove downloads will delete all downloads that have a timestamp that is
  // the same or more recent than |remove_begin|. The number of downloads
  // deleted is returned back to the caller.
  virtual int RemoveDownloads(const base::Time remove_begin) = 0;

  // Remove all downloads will delete all downloads. The number of downloads
  // deleted is returned back to the caller.
  virtual int RemoveAllDownloads() = 0;

  // Download the object at the URL. Used in cases such as "Save Link As..."
  virtual void DownloadUrl(const GURL& url,
                   const GURL& referrer,
                   const std::string& referrer_encoding,
                   TabContents* tab_contents) = 0;

  // Download the object at the URL and save it to the specified path. The
  // download is treated as the temporary download and thus will not appear
  // in the download history. Used in cases such as drag and drop.
  virtual void DownloadUrlToFile(const GURL& url,
                         const GURL& referrer,
                         const std::string& referrer_encoding,
                         const DownloadSaveInfo& save_info,
                         TabContents* tab_contents) = 0;

  // Allow objects to observe the download creation process.
  virtual void AddObserver(Observer* observer) = 0;

  // Remove a download observer from ourself.
  virtual void RemoveObserver(Observer* observer) = 0;

  // Called by the embedder, after creating the download manager, to let it know
  // about downloads from previous runs of the browser.
  virtual void OnPersistentStoreQueryComplete(
      std::vector<DownloadPersistentStoreInfo>* entries) = 0;

  // Called by the embedder, in response to
  // DownloadManagerDelegate::AddItemToPersistentStore.
  virtual void OnItemAddedToPersistentStore(int32 download_id,
                                            int64 db_handle) = 0;

  // The number of in progress (including paused) downloads.
  virtual int InProgressCount() const = 0;

  virtual content::BrowserContext* GetBrowserContext() const = 0;

  virtual FilePath LastDownloadPath() = 0;

  // Creates the download item.  Must be called on the UI thread.
  virtual void CreateDownloadItem(
      DownloadCreateInfo* info,
      const DownloadRequestHandle& request_handle) = 0;

  // Creates a download item for the SavePackage system.
  // Must be called on the UI thread.  Note that the DownloadManager
  // retains ownership.
  virtual DownloadItem* CreateSavePackageDownloadItem(
      const FilePath& main_file_path,
      const GURL& page_url,
      bool is_otr,
      DownloadItem::Observer* observer) = 0;

  // Clears the last download path, used to initialize "save as" dialogs.
  virtual void ClearLastDownloadPath() = 0;

  // Called by the delegate after the save as dialog is closed.
  virtual void FileSelected(const FilePath& path, void* params) = 0;
  virtual void FileSelectionCanceled(void* params) = 0;

  // Called by the delegate if it delayed the download in
  // DownloadManagerDelegate::ShouldStartDownload and now is ready.
  virtual void RestartDownload(int32 download_id) = 0;

  // Checks whether downloaded files still exist. Updates state of downloads
  // that refer to removed files. The check runs in the background and may
  // finish asynchronously after this method returns.
  virtual void CheckForHistoryFilesRemoval() = 0;

  // Get the download item from the history map.  Useful after the item has
  // been removed from the active map, or was retrieved from the history DB.
  virtual DownloadItem* GetDownloadItem(int id) = 0;

  // Called when Save Page download is done.
  virtual void SavePageDownloadFinished(DownloadItem* download) = 0;

  // Get the download item from the active map.  Useful when the item is not
  // yet in the history map.
  virtual DownloadItem* GetActiveDownloadItem(int id) = 0;

  virtual content::DownloadManagerDelegate* delegate() const = 0;

  // For testing only.  May be called from tests indirectly (through
  // other for testing only methods).
  virtual void SetDownloadManagerDelegate(
      content::DownloadManagerDelegate* delegate) = 0;

 protected:
  // These functions are here for unit tests.

  // Called back after a target path for the file to be downloaded to has been
  // determined, either automatically based on the suggested file name, or by
  // the user in a Save As dialog box.
  virtual void ContinueDownloadWithPath(DownloadItem* download,
                                        const FilePath& chosen_file) = 0;

  // Retrieves the download from the |download_id|.
  // Returns NULL if the download is not active.
  virtual DownloadItem* GetActiveDownload(int32 download_id) = 0;

  virtual void SetFileManager(DownloadFileManager* file_manager) = 0;

 private:
  // For testing.
  friend class ::DownloadManagerTest;

  friend class base::RefCountedThreadSafe<
      DownloadManager, content::BrowserThread::DeleteOnUIThread>;
  friend struct content::BrowserThread::DeleteOnThread<
      content::BrowserThread::UI>;
  friend class DeleteTask<DownloadManager>;
};

}  // namespace content

#endif  // CONTENT_PUBLIC_BROWSER_DOWNLOAD_MANAGER_H_

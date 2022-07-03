#pragma once

#ifndef COMMITLOG_PROTOTYPE_COMMITLOGMANAGER_HH
#define COMMITLOG_PROTOTYPE_COMMITLOGMANAGER_HH

#include <iostream>
#include <algorithm>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <dirent.h>
#include <cstring>
#include <chrono>

#include "commitlog.hh"
#include "log_record.hh"
// TODO: INTEGRATION: include DibibaseInstance ------------
// #include "DibibaseInstance.hh"


/**
 * A set of global variable to act as config
 * TODO: change this into a proper config.yml file
 */
std::string DEFAULT_COMMITLOG_DIRECTORY = "logs";
long int LOG_BUFFER_SIZE = 2000;
std::chrono::duration<long long int, std::milli> LOG_TIMEOUT = std::chrono::milliseconds(1000);


enum class SyncMode {
  SYNC = 0, // every log entry gets written to disk before committing
  BATCHED   // entries are kept in memory until the buffer is full or after a timeout
};

class CommitlogManager {
public:
  explicit CommitlogManager(SyncMode mode);

  ~CommitlogManager();

  // append a log into log buffer
  int32_t append_log_record(LogRecord &record);

  // getters and setters
  inline int32_t get_persistent_lsn() { return persistent_lsn; }

  inline void set_persistent_lsn(int32_t input_lsn) { persistent_lsn = input_lsn; }

  inline std::unique_ptr<char[]> get_log_buffer() { return std::move(log_buffer); }

  // start a separate thread that is responsible for flushing the buffer to disk.
  // The thread is triggered every LOG_TIMEOUT millisecond, when the flush buffer is full,
  // or when force_flush is called.
  void force_flush();

  // create a thread to flush the buffer
  void run_flush_thread();

  // generate a name for the next log file and create it
  std::unique_ptr<Commitlog> create_new_log_file();

  // a utility function to be used in apply_all_logs and apply_logs_from_lsn,
  // takes a queue of log files and recovers their data
  void apply_logs_queue(std::queue<std::unique_ptr<Commitlog>> &log_queue);

  // apply unarchived logs
  void apply_all_logs();

  // apply logs starting at a specific lsn
  void apply_logs_from_lsn(int32_t recovery_lsn);

  // a queue of log files that were marked inactive but not yet archived
  // once they are written to the DibibaseInstance they are marked archived and would be moved to another queue to be deleted
  std::queue<std::unique_ptr<Commitlog>> logs_to_flush;
  // logs that has been marked as both inactive and archived should be deleted
  std::queue<std::unique_ptr<Commitlog>> logs_to_delete;

  // a wrapper around the commitlog update_archived status method
  static void mark_archived(std::unique_ptr<Commitlog> log) { log->update_archived_status(true); };

  // TODO: INTEGRATION: Change into a DB instance.
  // register the DibibaseInstance as an observer
  void register_db(DibibaseInstance *database) { db = database; };
private:
  std::unique_ptr<Commitlog> commitlog; // The current log file to which we're appending records
  SyncMode sync_mode;   // Either synchronous or batched record flushes
  int32_t log_buffer_offset = 0;
  int32_t flush_buffer_size = 0; // in case of batched flushes
  std::atomic<bool> need_flush; // in case of batched flushes
  int32_t last_lsn = 0;
  std::condition_variable append_cv; // for notifying the append_thread
  std::atomic<int32_t> lsn;     // atomic counter for recording the lsn

  // Log records before and including persistent_lsn have already been written to disk
  std::atomic<int32_t> persistent_lsn;

  // used to buffer records until flushing
  std::unique_ptr<char[]> log_buffer = std::make_unique<char[]>(LOG_BUFFER_SIZE);

  // the contents of log buffer is put here to flush, so flushing doesn't block appending
  // new records to log_buffer
  std::unique_ptr<char[]> flush_buffer;

  std::mutex mutex_latch;
  std::thread *flush_thread;
  std::condition_variable flush_cv; // for notifying the flushing thread

  std::string default_directory;

  // TODO: INTEGRATION: Change this. ---------------------
  // instance of the database
  DibibaseInstance *db;
};


#endif //COMMITLOG_PROTOTYPE_COMMITLOGMANAGER_HH

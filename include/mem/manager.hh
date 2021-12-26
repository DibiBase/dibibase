#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "common.hh"
#include "io/manager.hh"
#include "mem/frame.hh"
#include "mem/page-replacement-strategy.hh"
#include "mem/page.hh"

namespace dibibase::mem {

class DIBIBASE_PUBLIC Manager {

public:
  Manager(io::Manager disk_space_manager, size_t buffer_size,
          std::unique_ptr<PageReplacementStrategy> page_replacement_strategy)
      : m_replacement_strategy(std::move(page_replacement_strategy)),
        m_disk_space_manager(disk_space_manager), m_first_free_index(0) {
    // TODO: init frames
  }

  ~Manager() {
    // TODO: all frames should be unpinned
    // TODO: skip invalid frames
    // TODO: cleanup and invalidate frames
  }

  Frame frame(int64_t page_number) {
    // TODO: check if page is even allocated
    // TODO: return and pin frame if it's already loaded
    // TODO: if there is a free frame use it else evict a frame and clean it up
    // then use it
    // TODO: create a new frame with the old buffer
    // TODO: flush evicted frame
    // TODO: load new page into the new frame and pin it
    //
  }

  Page page(int64_t page_number) { return page(frame(page_number)); }

  Frame get_new_frame(uint64_t page_num) {
    // TODO: allocate page from disk space manager
    // TODO: get its frame
  }

  Page get_new_page(uint32_t partition_number) {
    return page(get_new_frame(partition_number));
  }

  void delete_page(Page page) {
    // TODO: flush, remove, cleanup, and free underlying frame
    // TODO: make similar call to disk space manager
  }

  void delete_partition(int partition_number) {
    // TODO: remove, cleanup, flush, and free all frames with the same partition
    // numbner
    // TODO: make similar call to disk space manager
  }

  void evict(int64_t page_number) {
    // TODO: cleanup frame only if its loaded, is valid, and not pinned
    // TODO: replace frame with a new one and invalidate it
    // TODO: set m_first_free_index
  }

  void evictAll() {
    // TODO: evict all frames
  }

private:
  Page page(Frame frame) {
    // TODO: create new page from frame
  }

private:
  std::vector<Frame> m_frames;
  std::unordered_map<int64_t, uint32_t> m_pages_map;
  std::unique_ptr<PageReplacementStrategy> m_replacement_strategy;
  io::Manager m_disk_space_manager;
  uint32_t m_first_free_index;
};

} // namespace dibibase::mem

#include "db/table_manager.hh"
#include <memory>
#include <sys/types.h>
#include <utility>

using namespace dibibase::db;
using namespace dibibase::io;
using namespace dibibase::catalog;

TableManager::TableManager(std::string directory_path, std::string table_name,
                           catalog::Schema schema,
                           std::vector<std::unique_ptr<mem::Summary>> summary)
    : m_directory_path(directory_path), m_table_name(table_name),
      m_schema(schema), m_current_sstable_id(summary.size() - 1),
      m_summary(std::move(summary)) {}

void TableManager::write_record(catalog::Record record) {
  // Check record aganist schema.
  if (!m_schema.verify(record)) {
    DEFINE_ERROR(dismatch_record_with_schema_error);
    return;
  }

  size_t sort_index = m_schema.get_sort_index();
  m_memtable[record[sort_index]] = record;

  // Assuming that (record key size + offset in data file) = 48 bytes.
  // and IndexPage max size is 4096, then we can store 100 record key per single
  // page. then memtable can store up to 10000 keys, which will be stored in 100
  // pages when flushing into disk.
  // TODO: supporting dynamic size for the record key.
  if (m_memtable.size() >= 10000) {
    // TODO: It is supposed that flushing memtable into disk is asynchronous,
    // it is running in the background. Memtable must be ready to store new
    // records while flushing.
    flush();
    m_memtable.clear();
  }
}

Record TableManager::read_record(std::unique_ptr<catalog::Data> sort_key) {
  // Searching in memtable.
  auto find_key = m_memtable.find(sort_key);
  if (find_key != m_memtable.end()) {
    return find_key->second;
  }

  // Finding page number in summary which contains the record key.
  off_t page_number;
  ssize_t sstable_id = m_current_sstable_id;
  while (sstable_id >= 0) {
    page_number = m_summary[sstable_id]->find_index_page(sort_key.get());
    if (page_number != -1)
      break;
    sstable_id--;
  }

  // Loading index page from disk.
  DiskManager *disk_manager = DiskManager::get_instance();
  std::unique_ptr<db::IndexPage> index_page = disk_manager->load_index_page(
      m_directory_path, m_table_name, sstable_id, page_number);

  // Finding record key offset within the data file.
  off_t key_offset = index_page->find_key_offset(sort_key.get());

  return disk_manager->get_record_from_data(m_directory_path, m_table_name,
                                            sstable_id, m_schema, key_offset);
}

void TableManager::flush() {
  TableBuilder table_builder(m_directory_path, m_table_name, m_schema,
                             m_current_sstable_id, m_memtable);
  m_current_sstable_id++;
  m_summary.push_back(table_builder.get_new_summary());
}

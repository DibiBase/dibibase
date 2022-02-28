#include "db/table_manager.hh"

#include <memory>
#include <sys/types.h>
#include <utility>

using namespace dibibase;
using namespace dibibase::db;

TableManager::TableManager(std::string &base_path, std::string table_name,
                           std::unique_ptr<catalog::Schema> schema,
                           std::vector<std::unique_ptr<mem::Summary>> summary)
    : m_base_path(base_path), m_table_name(table_name),
      m_schema(std::move(schema)), m_current_sstable_id(summary.size()),
      m_summaries(std::move(summary)) {}

catalog::Record
TableManager::read_record(std::unique_ptr<catalog::Data> sort_key) {
  // Searching in memtable.
  std::shared_ptr<catalog::Data> shared_sort_key = std::move(sort_key);

  auto find_key = m_memtable.find(shared_sort_key);
  if (find_key != m_memtable.end())
    return find_key->second;

  // Finding page number in summary which contains the record key.
  uint8_t page_number;
  off_t key_offset;
  ssize_t sstable_id = m_current_sstable_id - 1;
  while (sstable_id >= 0) {
    page_number =
        m_summaries[sstable_id]->find_page_number(shared_sort_key.get());

    io::DiskManager &disk_manager = io::DiskManager::get_instance();

    // Loading index page from disk.
    std::unique_ptr<db::IndexPage> index_page = disk_manager.load_index_page(
        m_base_path, m_table_name, sstable_id, page_number);

    // Finding record key offset within the data file.
    key_offset = index_page->find_offset(shared_sort_key.get());

    if (key_offset != -1)
      return disk_manager.get_record_from_data(
          m_base_path, m_table_name, sstable_id, *m_schema, key_offset);

    sstable_id--;
  }

  throw non_existent_record_error("");
}

void TableManager::write_record(catalog::Record record) {
  // Check record aganist schema.
  if (!m_schema->verify(record))
    throw schema_mismatch_error("");

  auto sort_index = m_schema->sort_key_index();
  m_memtable.insert({record[sort_index], record});

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
  }
}

void TableManager::flush() {
  io::TableBuilder table_builder(m_base_path, m_table_name, *m_schema,
                                 m_current_sstable_id, m_memtable);
  m_summaries.push_back(table_builder.get_new_summary());
  m_memtable.clear();
}

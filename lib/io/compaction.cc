#include "io/compaction.hh"

#include <cstddef>
#include <fcntl.h>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

#include "catalog/schema.hh"
#include "util/logger.hh"

using namespace dibibase::io;

Compaction::Compaction(std::string &base_path, std::string &table_name,
                       catalog::Schema &schema, size_t sstable_id)
    : m_base_path(base_path), m_table_name(table_name), m_schema(schema),
      m_latest_sstable_id(sstable_id), m_total_records(0),
      m_disk_manager(io::DiskManager::get_instance()) {

  size_t record_size = m_schema.record_size();

  for (size_t id = 0; id < m_latest_sstable_id; id++) {
    off_t data_file_size =
        m_disk_manager.get_sstable_data_size(m_base_path, m_table_name, id);

    m_total_records += ((size_t)data_file_size / record_size);
  }
}

void Compaction::compact_sstables() {
  // Storing all records fetched from all data files.
  std::vector<catalog::Record> records;

  // Storing the start offset to be read from the data file.
  std::vector<off_t> offsets(m_latest_sstable_id + 1, 0);

  // Get sort key index from schema.
  auto sort_index = m_schema.sort_key_index();

  for (size_t id = 0; id < m_total_records; id++) {
    catalog::Record max_record = m_disk_manager.get_record_from_data(
        m_base_path, m_table_name, m_latest_sstable_id, m_schema,
        offsets[m_latest_sstable_id]);

    size_t max_sstable_id = m_latest_sstable_id;

    for (size_t id = m_latest_sstable_id - 1; id >= 0; id--) {
      catalog::Record fetched_record = m_disk_manager.get_record_from_data(
          m_base_path, m_table_name, id, m_schema, offsets[id]);

      catalog::Data *first_key = max_record[sort_index].get();
      catalog::Data *second_key = fetched_record[sort_index].get();

      if (!first_key->compare(second_key)) {
        // In case the 2 keys are equal, skip the fetched record from the least
        // sstable id.
        if (first_key->is_equal(second_key)) {
          offsets[id] += m_schema.record_size();
        } else {
          max_record = fetched_record;
          max_sstable_id = id;
        }
      }
    }

    records.push_back(max_record);
    offsets[max_sstable_id] += m_schema.record_size();
  }
}

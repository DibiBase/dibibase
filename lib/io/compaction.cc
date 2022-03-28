#include "io/compaction.hh"

#include <cstddef>
#include <fcntl.h>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

#include "catalog/schema.hh"
#include "io/table_builder.hh"
#include "util/logger.hh"

using namespace dibibase::io;

Compaction::Compaction(std::string &base_path, std::string &table_name,
                       catalog::Schema &schema, size_t sstable_id)
    : m_base_path(base_path), m_table_name(table_name), m_schema(schema),
      m_latest_sstable_id(sstable_id), m_total_records(0),
      m_disk_manager(io::DiskManager::get_instance()) {

  size_t record_size = m_schema.record_size();

  for (size_t id = 0; id <= m_latest_sstable_id; id++) {
    off_t data_file_size =
        m_disk_manager.get_data_file_size(m_base_path, m_table_name, id);

    m_total_records += ((size_t)data_file_size / record_size);
  }

  compact_sstables();
}

void Compaction::compact_sstables() {
  // Storing all records fetched from all data files.
  std::vector<catalog::Record> records;

  // Storing the start offset to be read from the data file.
  std::vector<off_t> offsets(m_latest_sstable_id + 1, 0);

  // Get sort key index from schema.
  auto sort_index = m_schema.sort_key_index();

  for (int i = 0; i < (int)m_total_records; i++) {
    catalog::Record max_record(std::vector<std::shared_ptr<catalog::Data>>{});

    size_t max_sstable_id = 0;

    for (int id = m_latest_sstable_id; id >= 0; id--) {
      if (offsets[id] >=
          m_disk_manager.get_data_file_size(m_base_path, m_table_name, id)) {
        continue;
      }

      catalog::Record fetched_record = m_disk_manager.get_record_from_data(
          m_base_path, m_table_name, id, m_schema, offsets[id]);

      // In case the maximum record is empty.
      if (max_record.values().empty()) {
        max_record = fetched_record;
        max_sstable_id = id;
        continue;
      }

      catalog::Data *max_key = max_record[sort_index].get();
      catalog::Data *fetched_key = fetched_record[sort_index].get();

      if (!max_key->compare(fetched_key)) {

        // In case the 2 keys are equal, skip the fetched record from the least
        // sstable id and increment the outer loop count.
        if (max_key->is_equal(fetched_key)) {
          offsets[id] += m_schema.record_size();
          i++;
        } else {
          max_record = fetched_record;
          max_sstable_id = id;
        }
      }
    }

    records.push_back(max_record);
    offsets[max_sstable_id] += m_schema.record_size();
  }

  for (int i = 0; i <= (int)m_latest_sstable_id; i++) {
    m_disk_manager.remove_sstable(m_base_path, m_table_name, i);
  }

  CompactionBuilder new_sstable(m_base_path, m_table_name, m_schema, 0,
                                records);

  m_summary = new_sstable.get_new_summary();
}

/*if (m_summaries.size() == 3) {
    io::Compaction compact(m_base_path, m_table_name, *m_schema,
                           m_summaries.size() - 1);

    m_summaries.clear();
    m_summaries.push_back(compact.get_new_summary());
  }*/

/*TableManager::~TableManager() {
/*if (!m_memtable.empty()) {
  flush();
}
}*/

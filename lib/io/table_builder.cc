#include "io/table_builder.hh"

#include <fcntl.h>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

#include "util/logger.hh"

using namespace dibibase::io;
using namespace dibibase::util;
using namespace dibibase::catalog;
using namespace dibibase::db;
using namespace dibibase::mem;

TableBuilder::TableBuilder(
    std::string &base_path, std::string &table_name, catalog::Schema &schema,
    size_t sstable_id,
    std::map<std::shared_ptr<catalog::Data>, catalog::Record, catalog::DataCmp>
        records)
    : m_base_path(base_path), m_table_name(table_name), m_schema(schema),
      m_new_sstable_id(sstable_id + 1), m_records(records) {

  // Creating summary, index and data file in disk.
  m_summary_fd = open((m_base_path + "/" + table_name + "/summary_" +
                       std::to_string(m_new_sstable_id) + ".db")
                          .c_str(),
                      O_WRONLY | O_CREAT);

  m_index_fd = open((m_base_path + "/" + table_name + "/index_" +
                     std::to_string(m_new_sstable_id) + ".db")
                        .c_str(),
                    O_WRONLY | O_CREAT);

  m_data_fd = open((m_base_path + "/" + table_name + "/data_" +
                    std::to_string(m_new_sstable_id) + ".db")
                       .c_str(),
                   O_WRONLY | O_CREAT);

  construct_sstable_files();
}

void TableBuilder::construct_sstable_files() {
  auto record_iterator = m_records.begin();

  // Getting record key type.
  catalog::Data::Type key_type = record_iterator->first->type();

  m_summary = std::make_unique<Summary>(key_type);
  m_index_page = std::make_unique<IndexPage>(key_type);

  // Key offset in data file.
  off_t key_offset = 0;

  // Storing the first record key in summary.
  m_summary->push_back(record_iterator->first.get());

  while (record_iterator != m_records.end()) {
    // Storing record bytes in a buffer.
    util::Buffer *record_buffer =
        new util::MemoryBuffer(m_schema.record_size());
    record_iterator->second.bytes(record_buffer);

    // Storing record bytes in data file.
    std::unique_ptr<unsigned char[]> r_buf = record_buffer->bytes();
    int data_wc = write(m_data_fd, r_buf.get(), m_schema.record_size());
    delete record_buffer;

    if (data_wc < 0) {
      util::Logger::make().err("Error writing in Data file: %d", errno);
      return;
    }

    if (!m_index_page->push_back(record_iterator->first.get(), key_offset)) {
      // Storing the first key allocated in a new page.
      m_summary->push_back(record_iterator->first.get());

      if (!write_index_file())
        return;

      m_index_page->clear();
      m_index_page->push_back(record_iterator->first.get(), key_offset);
    }

    key_offset += m_schema.record_size();
    record_iterator++;
  }

  if (!write_index_file())
    return;
  write_summary_file();
}

bool TableBuilder::write_index_file() {
  // Storing the index page in buffer.
  util::Buffer *index_page_buffer = new util::MemoryBuffer(4096);
  m_index_page->bytes(index_page_buffer);

  // Storing index page bytes in index file.
  std::unique_ptr<unsigned char[]> i_buf = index_page_buffer->bytes();
  int wc = write(m_index_fd, i_buf.get(), 4096);
  delete index_page_buffer;

  if (wc < 0) {
    util::Logger::make().err("Error writing in index file: %d", errno);
    return false;
  }
  return true;
}

void TableBuilder::write_summary_file() {
  // Storing summary in buffer.
  util::Buffer *summary_buffer = new util::MemoryBuffer(4096);
  m_summary->bytes(summary_buffer);

  // Storing summary bytes in summary file.
  std::unique_ptr<unsigned char[]> s_buf = summary_buffer->bytes();
  int wc = write(m_summary_fd, s_buf.get(), 4096);
  delete summary_buffer;

  if (wc < 0) {
    util::Logger::make().err("Error writing in summary file: %d", errno);
  }
}

TableBuilder::~TableBuilder() {
  close(m_summary_fd);
  close(m_index_fd);
  close(m_data_fd);
}

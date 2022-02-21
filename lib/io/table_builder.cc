#include "io/table_builder.hh"
#include <fcntl.h>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <utility>

using namespace dibibase::io;
using namespace dibibase::util;
using namespace dibibase::catalog;
using namespace dibibase::db;
using namespace dibibase::mem;

TableBuilder::TableBuilder(
    std::string directory_path, std::string table_name, catalog::Schema schema,
    size_t sstable_id,
    std::map<std::unique_ptr<catalog::Data>, catalog::Record> records)
    : m_directory_path(directory_path), m_table_name(table_name),
      m_schema(schema), m_new_sstable_id(sstable_id + 1), m_records(records),
      m_summary(std::make_unique<mem::Summary>()),
      m_index_page(std::make_unique<IndexPage>()) {

  std::string root_dir = directory_path + "/" + table_name;

  std::string data_path =
      root_dir + "/data" + std::to_string(m_new_sstable_id) + ".db";

  std::string index_path =
      root_dir + "/index" + std::to_string(m_new_sstable_id) + ".db";

  std::string summary_path =
      root_dir + "/summary" + std::to_string(m_new_sstable_id) + ".db";

  // Creating data file.
  m_file_descriptors[0] = open(data_path.c_str(), O_RDWR | O_CREAT,
                               S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  // Creating index file.
  m_file_descriptors[1] = open(index_path.c_str(), O_RDWR | O_CREAT,
                               S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  // Creating summary file.
  m_file_descriptors[2] = open(summary_path.c_str(), O_RDWR | O_CREAT,
                               S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

  size_t total_size = m_schema.record_size() * m_records.size();

  m_data_buffer = std::make_unique<MemoryBuffer>(total_size);

  m_index_buffer = std::make_unique<MemoryBuffer>(409600);

  m_summary_buffer = std::make_unique<MemoryBuffer>(4800);

  // Filling SSTable buffers.
  fill_data_buffer();

  // constructing SSTable data file.
  std::unique_ptr<unsigned char[]> data = m_data_buffer->bytes();
  write(m_file_descriptors[0], data.get(), m_data_buffer->size());

  // constructing SSTable index file.
  std::unique_ptr<unsigned char[]> index = m_index_buffer->bytes();
  write(m_file_descriptors[1], index.get(), m_index_buffer->size());

  // Filling summary buffer.
  m_summary->bytes(m_summary_buffer.get());

  // constructing SSTable summary file.
  std::unique_ptr<unsigned char[]> summary = m_summary_buffer->bytes();
  write(m_file_descriptors[2], summary.get(), m_summary_buffer->size());
}

void TableBuilder::fill_data_buffer() {
  auto record_iterator = m_records.begin();

  // Storing the first key in summary.
  m_summary->push_back(record_iterator->first.get());

  // Storing key offset within the data file.
  off_t key_offset = 0;

  while (record_iterator != m_records.end()) {
    // Filling buffer with the record data bytes.
    record_iterator->second.bytes(m_data_buffer.get());
    fill_index_buffer(record_iterator->first.get(), key_offset);
    key_offset += m_data_buffer->current_offset();
    record_iterator++;
  }
}

void TableBuilder::fill_index_buffer(Data *record_key, off_t offset) {
  if (m_index_page->add_sort_key(record_key, offset))
    return;

  // Storing the initial record key in a summary.
  m_summary->push_back(record_key);

  // Filling index buffer.
  m_index_page->bytes(m_index_buffer.get());

  m_index_page->clear();

  m_index_page->add_sort_key(record_key, offset);
}

TableBuilder::~TableBuilder() {
  for (int i = 0; i < 3; i++) {
    close(i);
  }
}
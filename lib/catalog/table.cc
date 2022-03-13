#include "catalog/table.hh"

using namespace dibibase::catalog;

std::unique_ptr<Table> Table::from(util::Buffer *buf) {
  auto name_length = buf->get_uint8();
  auto name = buf->get_string(name_length);
  auto read_schema = Schema::from(buf);
  auto m_last_sstable_id = buf->get_uint64();

  return std::make_unique<Table>(name,
                                 Schema(read_schema->sort_key_index(),
                                        read_schema->partition_key_index(),
                                        read_schema->fields()),
                                 m_last_sstable_id);
}

size_t Table::size() const {
  return sizeof(uint8_t) + m_name.length() + m_schema.size() + sizeof(uint64_t);
}

void Table::bytes(util::Buffer *buf) {
  buf->put_uint8(m_name.length());
  buf->put_string(m_name);
  m_schema.bytes(buf);
  buf->put_uint64(m_last_sstable_id);
}

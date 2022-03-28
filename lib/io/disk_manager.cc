#include "io/disk_manager.hh"

#include <cerrno>
#include <fcntl.h>
#include <memory>
#include <string>
#include <unistd.h>

#include "catalog/record.hh"
#include "catalog/schema.hh"
#include "db/index_page.hh"
#include "mem/summary.hh"
#include "util/buffer.hh"
#include "util/logger.hh"

using namespace dibibase;

std::unique_ptr<mem::Summary>
io::DiskManager::load_summary(std::string &database_path,
                              std::string &table_name, size_t sstable_id) {
  int fd = open((database_path + "/" + table_name + "/summary_" +
                 std::to_string(sstable_id) + ".db")
                    .c_str(),
                O_RDONLY);

  std::unique_ptr<unsigned char[]> buf =
      std::unique_ptr<unsigned char[]>(new unsigned char[4096]);
  int rc = read(fd, buf.get(), 4096);

  if (rc < 0)
    util::Logger::make().err("Error reading Summary file: %d", errno);
  else if (rc != 4096)
    util::Logger::make().err("Malformed Summary file: File length isn't 4096");

  util::Buffer *summary_buffer = new util::MemoryBuffer(std::move(buf), 4096);

  auto summary = mem::Summary::from(summary_buffer);

  delete summary_buffer;
  close(fd);

  return summary;
}

std::unique_ptr<db::IndexPage>
io::DiskManager::load_index_page(std::string &database_path,
                                 std::string &table_name, size_t sstable_id,
                                 uint8_t page_num) {
  int fd = open((database_path + "/" + table_name + "/index_" +
                 std::to_string(sstable_id) + ".db")
                    .c_str(),
                O_RDONLY);
  lseek(fd, page_num * 4096, SEEK_SET);

  std::unique_ptr<unsigned char[]> buf =
      std::unique_ptr<unsigned char[]>(new unsigned char[4096]);
  int rc = read(fd, buf.get(), 4096);

  if (rc < 0)
    util::Logger::make().err("Error reading Index file: %d", errno);
  else if (rc != 4096)
    util::Logger::make().err(
        "Malformed Index file: File length isn't a mulitple of 4096");

  util::Buffer *index_buffer = new util::MemoryBuffer(std::move(buf), 4096);

  auto index_page = db::IndexPage::from(index_buffer);

  delete index_buffer;
  close(fd);

  return index_page;
}

catalog::Record io::DiskManager::get_record_from_data(
    std::string &database_path, std::string &table_name, size_t sstable_id,
    catalog::Schema &schema, off_t offset) {
  int fd = open((database_path + "/" + table_name + "/data_" +
                 std::to_string(sstable_id) + ".db")
                    .c_str(),
                O_RDONLY);
  lseek(fd, offset, SEEK_SET);

  std::unique_ptr<unsigned char[]> buf =
      std::unique_ptr<unsigned char[]>(new unsigned char[schema.record_size()]);
  int rc = read(fd, buf.get(), schema.record_size());

  if (rc < 0)
    util::Logger::make().err("Error reading Data file: %d", errno);
  else if (rc != static_cast<int>(schema.record_size()))
    util::Logger::make().err("Malformed Data file: File is truncated");

  util::Buffer *record_buffer =
      new util::MemoryBuffer(std::move(buf), schema.record_size());

  auto read_record = catalog::Record::from(record_buffer, schema);
  auto record = catalog::Record(read_record->values());

  delete record_buffer;
  close(fd);

  return record;
}

off_t io::DiskManager::get_data_file_size(std::string &database_path,
                                          std::string &table_name,
                                          size_t sstable_id) {

  int fd = open((database_path + "/" + table_name + "/data_" +
                 std::to_string(sstable_id) + ".db")
                    .c_str(),
                O_RDONLY);

  if (fd < 0)
    std::perror("open");

  off_t size = lseek(fd, 0, SEEK_END);

  close(fd);

  return size;
}

void io::DiskManager::remove_sstable(std::string &database_path,
                                     std::string &table_name,
                                     size_t sstable_id) {

  int rc = remove((database_path + "/" + table_name + "/data_" +
                   std::to_string(sstable_id) + ".db")
                      .c_str());
  if (rc < 0)
    util::Logger::make().err("Error removing data file: %d", errno);

  rc = remove((database_path + "/" + table_name + "/index_" +
               std::to_string(sstable_id) + ".db")
                  .c_str());

  if (rc < 0)
    util::Logger::make().err("Error removing index file: %d", errno);

  rc = remove((database_path + "/" + table_name + "/summary_" +
               std::to_string(sstable_id) + ".db")
                  .c_str());

  if (rc < 0)
    util::Logger::make().err("Error removing summary file: %d", errno);
}

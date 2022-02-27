#include "db/database.hh"

#include <memory>

#include "catalog/data.hh"
#include "catalog/schema.hh"
#include "catalog/table.hh"
#include "db/table_manager.hh"
#include "io/disk_manager.hh"
#include "mem/summary.hh"

using namespace dibibase::db;
using namespace dibibase::catalog;
using namespace dibibase::util;
using namespace dibibase::mem;

void Database::create_table(std::string table_name, catalog::Schema schema) {
  size_t sstable_id = 0;
  Table table(table_name, schema, sstable_id);
  size_t table_size = table.get_table_size(table, schema);
  std::unique_ptr<Buffer> buf = std::make_unique<MemoryBuffer>(table_size);
  table.bytes(buf.get());
  write_metadata(buf.get());

  // making table_mgr map
  std::vector<std::unique_ptr<mem::Summary>> summary_vec;
  TableManager tablemanager(m_directory_path, table.get_table_name(),
                            table.get_table_schema(), std::move(summary_vec));
  m_table_managers.insert({table.get_table_name(), std::move(tablemanager)});
}

void Database::write_record(std::string table_name, catalog::Record record) {

  TableManager tablemanager =
      std::move(m_table_managers.find(table_name)->second);
  tablemanager.write_record(record);
}
Record Database::read_record(std::string table_name,
                             std::unique_ptr<catalog::Data> data) {
  TableManager tablemanager =
      std::move(m_table_managers.find(table_name)->second);
  Record record = tablemanager.read_record(std::move(data));
  return record;
}

void Database::write_metadata(util::Buffer *buffer) {
  std::unique_ptr<unsigned char[]> bytes = buffer->bytes();
  size_t size = buffer->size();
  // opening metadata file
  string filename = m_directory_path + "/" + "metadata.md";
  int fd = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    perror("cann't create/open metadata.md");
    close(fd);
    exit(1);
  }
  int writing = write(fd, bytes.get(), size);
  if (writing < 0) {
    perror("writing error");
    close(fd);
    exit(1);
  }
}

void Database::flush_metadata() {}

Database::Database(std::string directory_path)
    : m_directory_path(directory_path) {

  std::unique_ptr<Buffer> buffer = std::make_unique<MemoryBuffer>(9000);

  string filename = directory_path + "/" + "metadata.md";
  // case existing directory
  DIR *dr;
  dr = opendir(directory_path.c_str());
  if (dr) {
    int fd = open(filename.c_str(), O_RDONLY, 0);

    if (fd < 0)
      printf("metadata.md is empty");
    else {

      struct stat stat_buf;
      size_t rc = stat(filename.c_str(), &stat_buf);
      if (rc == 0)
        rc = stat_buf.st_size;
      else
        rc = -1;

      int rd = read(fd, buffer.get(), rc);
      if (rd < 0) {
        perror("reading error");
        close(fd);
        exit(1);
      }

      util::Buffer *buff = nullptr;
      std::unique_ptr<Table> tt;
      tt = Table::from(buff);
      std::unique_ptr<Table> table = Table::from(buffer.get());

      // iterate over buffer
      while (buffer->current_offset() < rc) {

        std::unique_ptr<Table> table = Table::from(buffer.get());
        //-->offset += table.size();
        int no_of_ssts = table->get_last_sstable_id();
        std::vector<std::unique_ptr<mem::Summary>> summary_vec;
        io::DiskManager diskmgr;

        for (int i = 0; i < no_of_ssts; i++) {
          std::unique_ptr<mem::Summary> summary =
              diskmgr.load_summary(directory_path, table->get_table_name(), i);
          summary_vec.push_back(std::move(summary));
        }

        // inst of table mgr
        TableManager tablemanager(m_directory_path, table->get_table_name(),
                                  table->get_table_schema(),
                                  std::move(summary_vec));
        // save it in map
        m_table_managers.insert(
            {table->get_table_name(), std::move(tablemanager)});
      }
    }
    closedir(dr);
  } else {
    // create a new directory
    int check = mkdir(directory_path.c_str(), 0777);
    if (check)
      printf("Couldn't create the directory an Error ocurred\n");
    // create new metadata.md file
    FILE *o_file = fopen(filename.c_str(), "w+");
    if (!o_file)
      printf("ERR couldn't create new metadata file");
  }
}

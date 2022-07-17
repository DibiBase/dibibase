
#include <iostream>

#include "lang/statements/create_table_statement.hh"
#include "lang/statements/insert_statement.hh"
#include "lang/statements/select_statement.hh"
#include "lang/statements/statement.hh"
#include "catalog/schema.hh"
#include "catalog/data.hh"
#include "catalog/record.hh"
#include "util/buffer.hh"

// #include <commitlog/commitlog.hh>
#include <commitlog/commitlog_manager.hh>

using namespace dibibase;
using namespace commitlog;

int main() {
    db::Database *my_db = new db::Database("../../dibibase-instance");
    // std::unique_ptr<CommitlogManager> clm(new CommitlogManager(SyncMode::BATCHED));
    // clm->register_db(&my_db);
    

    catalog::Field id_field("id", catalog::Data::Type(catalog::Data::Type::Id::INT, 4));
    catalog::Field employee_name_field("employee_name", catalog::Data::Type(catalog::Data::Type::ASCII,8));
    catalog::Schema my_schema(0,0,{id_field,employee_name_field}); 
    my_db->create_table("my_table",my_schema);

    // testing get schema
    catalog::Schema *test_schema ( my_db->get_table_schema("my_table").get());

    auto sch = test_schema->fields();
    for (auto &field : sch) {
        std::cout << "\nschema:----------------" <<field.name() << std::endl;
    }

    catalog::IntData id(1);
    catalog::ASCIIData name1 ("mohamed1");
    catalog::ASCIIData name2 ("record22");
    catalog::ASCIIData name3 ("record33");
    
    std::vector<std::shared_ptr<catalog::Data>> values1;
    values1.push_back(std::make_shared<catalog::IntData>(id));
    values1.push_back(std::make_shared<catalog::ASCIIData>(name1));


    std::vector<std::shared_ptr<catalog::Data>> values2;
    values2.push_back(std::make_shared<catalog::IntData>(id));
    values2.push_back(std::make_shared<catalog::ASCIIData>(name2));

    std::vector<std::shared_ptr<catalog::Data>> values3;
    values3.push_back(std::make_shared<catalog::IntData>(id));
    values3.push_back(std::make_shared<catalog::ASCIIData>(name3));

    catalog::Record first_record(values1);
    catalog::Record second_record(values2);
    catalog::Record third_record(values3);
    // int log_file_descriptor = open("../../dibibase-instance/test.log", O_RDWR, 0755);
    // create a buffer and serialize the record to it
    util::MemoryBuffer buffer(2000);
    // first_record.bytes(&buffer);
    // std::cout <<buffer.offset();

    // ==================================================
    // clm->append_log_record("my_table", &buffer, buffer.offset());
    // clm->apply_logs_from_lsn(1);
    // std::cout << "finished"  << std::endl;

    // write(log_file_descriptor, buffer.bytes().get(),buffer.offset());

    // std::unique_ptr<unsigned char []> test_buffer(new unsigned char[2000]);

    // int offset = read(log_file_descriptor,test_buffer.get(), 100);
    // std::cout << "log_file_descriptor:" << log_file_descriptor << std::endl;
    // std::cout << "offset:" << offset << std::endl;

    // util::MemoryBuffer read_buffer(move(test_buffer), offset);


    // my_db->write_record("my_table" , first_record);
    my_db->recover(1);


    // my_db->write_record("my_table" , second_record);
    // my_db->write_record("my_table" , third_record);

    // auto last_written_record_values = my_db->read_record("my_table" , std::make_unique<catalog::IntData>(id)).values();

    // for (auto &value : last_written_record_values) {
    //     std::cout << value.get()->print() << std::endl;
    // }

    // printf("%s" , last_written_record_values[1].get()->print().c_str());
  return EXIT_SUCCESS;
}
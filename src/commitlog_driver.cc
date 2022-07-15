
#include <iostream>

#include "lang/statements/create_table_statement.hh"
#include "lang/statements/insert_statement.hh"
#include "lang/statements/select_statement.hh"
#include "lang/statements/statement.hh"
#include "catalog/schema.hh"
#include "catalog/data.hh"
#include "catalog/record.hh"

// #include <commitlog/commitlog.hh>
#include <commitlog/commitlog_manager.hh>

using namespace dibibase;

int main() {
    db::Database my_db("../../dibibase-instance");
    Test test = Test();
    // Commitlog cl = Commitlog("./commitlog.log");
    std::cout << DEFAULT_COMMITLOG_DIRECTORY << std::endl;
    // instantiate a commitlog manager
    // CommitlogManager *clm (new CommitlogManager(SyncMode::BATCHED));
    // std::unique_ptr<CommitlogManager> clm(new CommitlogManager(SyncMode::BATCHED));
    // std::unique_ptr<CommitlogManager> clm = std::make_unique<CommitlogManager>(new CommitlogManager(SyncMode::BATCHED));
    
    
    // catalog::Field id_field("id", catalog::Data::Type(catalog::Data::Type::Id::INT, 4));
    // catalog::Field employee_name_field("employee_name", catalog::Data::Type(catalog::Data::Type::ASCII,8));
    // catalog::Schema my_schema(0,0,{id_field,employee_name_field}); 
    // my_db.create_table("my_table",my_schema); 


    // catalog::IntData id(1);
    // catalog::ASCIIData name1 ("mohamed1");
    // catalog::ASCIIData name2 ("record22");
    // catalog::ASCIIData name3 ("record33");
    
    // std::vector<std::shared_ptr<catalog::Data>> values1;
    // values1.push_back(std::make_shared<catalog::IntData>(id));
    // values1.push_back(std::make_shared<catalog::ASCIIData>(name1));


    // std::vector<std::shared_ptr<catalog::Data>> values2;
    // values2.push_back(std::make_shared<catalog::IntData>(id));
    // values2.push_back(std::make_shared<catalog::ASCIIData>(name2));

    // std::vector<std::shared_ptr<catalog::Data>> values3;
    // values3.push_back(std::make_shared<catalog::IntData>(id));
    // values3.push_back(std::make_shared<catalog::ASCIIData>(name3));

    // catalog::Record first_record(values1);
    // catalog::Record second_record(values2);
    // catalog::Record third_record(values3);

    // my_db.write_record("my_table" , first_record);
    // my_db.write_record("my_table" , second_record);
    // my_db.write_record("my_table" , third_record);

    // auto last_written_record_values = my_db.read_record("my_table" , std::make_unique<catalog::IntData>(id)).values();

    // for (auto &value : last_written_record_values) {
    //     std::cout << value.get()->print() << std::endl;
    // }

    // printf("%s" , last_written_record_values[1].get()->print().c_str());
  return EXIT_SUCCESS;
}
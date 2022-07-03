#include <cstdlib>
#include <iostream>

#include "antlr4-runtime.h"

#include "CqlLexer.h"
#include "CqlParser.h"

#include "lang/cql3_visitor.hh"
#include "lang/statements/create_table_statement.hh"
#include "lang/statements/insert_statement.hh"
#include "lang/statements/select_statement.hh"
#include "lang/statements/statement.hh"
#include "catalog/schema.hh"
#include "catalog/data.hh"
#include "catalog/record.hh"


using namespace antlr4;
using namespace dibibase;
using namespace dibibase::lang::cql3;

int main() {
    db::Database my_db("database");
    
    catalog::Field id_field("id", catalog::Data::Type(catalog::Data::Type::Id::INT, 4));
    catalog::Field employee_name_field("employee_name", catalog::Data::Type(catalog::Data::Type::ASCII,5));
    catalog::Schema my_schema(0,0,{id_field,employee_name_field}); 
    my_db.create_table("my_table",my_schema); 


    catalog::IntData id(1);
    catalog::ASCIIData name1 ("ahmed");
    catalog::ASCIIData name2 ("essam");
    
    std::vector<std::shared_ptr<catalog::Data>> values1;
    values1.push_back(std::make_shared<catalog::IntData>(id));
    values1.push_back(std::make_shared<catalog::ASCIIData>(name1));


    std::vector<std::shared_ptr<catalog::Data>> values2;
    values2.push_back(std::make_shared<catalog::IntData>(id));
    values2.push_back(std::make_shared<catalog::ASCIIData>(name2));


    
    catalog::Record first_record(values1);
    catalog::Record second_record(values2);

    my_db.write_record("my_table" , first_record);
    my_db.write_record("my_table" , second_record);

    auto last_written_record_values = my_db.read_record("my_table" , std::make_unique<catalog::IntData>(id)).values();


    printf("%s" , last_written_record_values[1].get()->print());
  return EXIT_SUCCESS;
}

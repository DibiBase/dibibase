#include "gtest/gtest.h"

#include "catalog/table.hh"
#include "mem/summary.hh"
#include "util/buffer.hh"
#include <cstddef>
#include <cstdint>
#include <string>
#include <iostream>

#include <limits>
using namespace dibibase::util;
using namespace dibibase::catalog;
using namespace dibibase::mem;

#define EXPECT_THROW_MSG(statement, expected_exception, expected_what)                    \
  try                                                                                     \
  {                                                                                       \
    statement;                                                                            \
    FAIL() << "Expected: " #statement " throws an exception of type " #expected_exception \
              ".\n"                                                                       \
              "  Actual: it throws nothing.";                                             \
  }                                                                                       \
  catch (const expected_exception& e)                                                     \
  {                                                                                       \
    EXPECT_EQ(expected_what, std::string{e.what()});                                      \
  }                                                                                       \
  catch (...)                                                                             \
  {                                                                                       \
    FAIL() << "Expected: " #statement " throws an exception of type " #expected_exception \
              ".\n"                                                                       \
              "  Actual: it throws a different type.";                                    \
  }

TEST(Summary, intialize) {
    std::vector < std::unique_ptr < Data >> test_sort_keys;
    Data::Type test_type(Data::Type::BLOB, 2);
    Summary test_summary(std::move(test_sort_keys), test_type);
    EXPECT_EQ(test_summary.sort_keys(), test_sort_keys);
}

TEST(Summary, empty_summary_throws_error) {
    EXPECT_THROW_MSG({
        std::vector < std::unique_ptr < Data >> test_sort_keys;
        Data::Type test_type(Data::Type::ASCII, 2);
        std::unique_ptr < Data > data1 = std::make_unique < ASCIIData > ("aa");
        Summary test_summary(std::move(test_sort_keys), test_type);
        test_summary.find_page_number(data1.get());
    }, std::invalid_argument, "Empty summary")
}

TEST(Summary, add_sort_keys) {
    std::vector < std::unique_ptr < Data >> test_sort_keys;
    Data::Type test_type(Data::Type::ASCII, 2);
    Summary test_summary(std::move(test_sort_keys), test_type);

    std::unique_ptr < Data > data1 = std::make_unique < ASCIIData > ("aa");

    test_summary.push_back(data1.get());
    uint8_t expected_page_number = 0;
    EXPECT_EQ(test_summary.find_page_number(data1.get()), expected_page_number);

    test_summary.push_back(data1.get());
    expected_page_number++;
    EXPECT_EQ(test_summary.find_page_number(data1.get()), expected_page_number);

    test_summary.push_back(data1.get());
    expected_page_number++;
    EXPECT_EQ(test_summary.find_page_number(data1.get()), expected_page_number);

    test_summary.push_back(data1.get());
    expected_page_number++;
    EXPECT_EQ(test_summary.find_page_number(data1.get()), expected_page_number);
}

TEST(Summary, size_works_properly) {
    std::vector < std::unique_ptr < Data >> test_sort_keys;
    Data::Type test_type(Data::Type::ASCII, 3);
    std::unique_ptr < Data > data1 = std::make_unique < ASCIIData > ("aa");

    Summary test_summary(std::move(test_sort_keys), test_type);
    EXPECT_EQ(test_summary.size(), sizeof(uint8_t) + test_type.size());

    test_summary.push_back(data1.get());
    EXPECT_EQ(test_summary.size(), sizeof(uint8_t) + test_type.size() + test_type.length());
}

TEST(Summary, from_and_bytes_works_properly) {
    std::vector < std::unique_ptr < Data >> test_sort_keys;
    std::vector < std::string > values {
        "Hassan",
        "Khaled",
        "Antwan",
        "Mariam",
        "Golang"
    }; // Any string of 6 letters
    for (auto val: values) {
        test_sort_keys.push_back(std::move(std::make_unique < ASCIIData > (val)));
    }
    Summary test_summary(std::move(test_sort_keys), Data::Type(Data::Type::ASCII, 6));
    Buffer * buff = new MemoryBuffer(36);

    test_summary.bytes(buff);

    std::unique_ptr < unsigned char[] > buff_bytes = buff -> bytes();

    Buffer * buff1 = new MemoryBuffer(std::move(buff_bytes), 36);

    std::unique_ptr < Summary > new_test_summary = Summary::from(buff1);
    const std::vector < std::unique_ptr < Data >> & extracted_keys = new_test_summary -> sort_keys();

    for (size_t i = 0; i < extracted_keys.size(); i++) {

        Data::Type type = extracted_keys[i] -> type();

        auto child_data = (dynamic_cast <ASCIIData *> (extracted_keys[i].get()));
        if (type.id() == Data::Type::ASCII) {
            std::string val = child_data -> data();
            EXPECT_EQ(val, values[i]);
        }
    }
}

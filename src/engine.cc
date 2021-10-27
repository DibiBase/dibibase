#include <iostream>

#include "db/db.hh"

using namespace dibibase::db;

int main(int argc, char **argv) {
  DBImpl engine;
  engine.write("name", "ahmed");
  engine.write("key", "value");
  engine.write("mariam", "name");
  engine.write("distribut", "goodd");
  engine.write("testttt", "gg11");
  engine.write("mariam", "name");

  std::cout << engine.read("name") << std::endl;
  std::cout << engine.read("key") << std::endl;
  std::cout << engine.read("mariam") << std::endl;
  std::cout << engine.read("distribut") << std::endl;

  std::cout << engine.read("testttt") << std::endl;

  engine.write("essam", "essam");
  engine.write("ayaaya", "ayaaaaa");
  engine.write("abcde", "232");

  std::cout << engine.read("essam") << std::endl;
  std::cout << engine.read("abcde") << std::endl;
}
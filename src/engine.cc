#include <iostream>

#include "db/db.hh"
#include "util/logger.hh"

using namespace dibibase::db;
using namespace dibibase::util;

int main(int argc, char **argv) {
  Logger logger = Logger::make();
  DBImpl engine;
  engine.write("name", "ahmed");
  engine.write("key", "value");
  engine.write("mariam", "name");
  engine.write("distributedsystems", "goodd");
  engine.write("testttt", "gg11");
  engine.write("mariam", "name");

  if (engine.read("name") == "ahmed") {
    logger.info("value is correctly extracted");
  } else {
    logger.err("wrong value");
  }

  if (engine.read("key") == "value") {
    logger.info("value is correctly extracted");
  } else {
    logger.err("wrong value");
  }

  if (engine.read("mariam") == "name") {
    logger.info("value is correctly extracted");
  } else {
    logger.err("wrong value");
  }

  if (engine.read("distributedsystems") == "goodd") {
    logger.info("value is correctly extracted");
  } else {
    logger.err("wrong value");
  }

  if (engine.read("testttt") == "gg11") {
    logger.info("value is correctly extracted");
  } else {
    logger.err("wrong value");
  }

  engine.write("essam", "essam123456789");
  engine.write("ayaaya", "ayaaaaa");
  engine.write("abcde", "232");

  if (engine.read("essam") == "essam123456789") {
    logger.info("value is correctly extracted");
  } else {
    logger.err("wrong value");
  }
}

#include <iostream>

#include "db/db.hh"
#include "util/logger.hh"

using namespace dibibase::db;
using namespace dibibase::util;

int main(int argc, char **argv) {
  Logger logger = Logger::make();
  DBImpl engine;
  /*engine.write("name12", "ahmed");
  engine.write("key", "value");
  engine.write("mariam", "name");
  engine.write("distributedsystems", "goodd");
  engine.write("testttttt", "gg11");
  engine.write("mariam", "name");*/

  if (engine.read("name12") == "ahmed") {
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

  if (engine.read("testttttt") == "gg11") {
    logger.info("value is correctly extracted");
  } else {
    logger.err("wrong value");
  }

  /*engine.write("essam", "essam123456789");
  engine.write("ayaaya", "ayaaaaa");
  engine.write("abcde", "232");*/

  if (engine.read("essam") == "essam123456789") {
    logger.info("value is correctly extracted");
  } else {
    logger.err("wrong value");
  }

  if(engine.read("ayaaya") == "ayaaaaa") {
    logger.info("value is correctly extracted");
  }
  else {
    logger.err("wrong value");
  }

  engine.write("new1", "ahmed");
  engine.write("new2", "value");
  engine.write("new3", "name");
  engine.write("new4", "goodd");

  if (engine.read("new1") == "ahmed") {
    logger.info("value is correctly extracted");
  } else {
    logger.err("wrong value");
  }

  if(engine.read("new2") == "value") {
    logger.info("value is correctly extracted");
  }
  else {
    logger.err("wrong value");
  }
}

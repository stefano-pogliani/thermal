// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "thermal/outputs/file.h"

#include <unistd.h>

#include <ctime>
#include <sstream>
#include <string>
#include <json.hpp>

#include "thermal/device-repo.h"
#include "thermal/log.h"

using nlohmann::json;

using thermal::DeviceRepo;
using thermal::FileOutput;
using thermal::Log;
using thermal::Output;
using thermal::OutputRef;


OutputRef FileOutput::Make(json config) {
  return OutputRef(new FileOutput(config));
}


FileOutput::FileOutput(json config) : Output(config) {
  // NOOP.
}


std::string FileOutput::now() {
  std::time_t t  = std::time(nullptr);
  std::tm*    s = std::gmtime(&t);
  std::stringstream start;
  start << std::put_time(s, "%FT%T");
  return start.str();
}


void FileOutput::loop(DeviceRepo& repo) {
  Log::info() << "Fetching temperatures ..." << std::endl;
  std::string start = this->now();

  repo.for_each([&](DeviceRepo::Device device) {
      std::string name = device.name;
      double temp = device.sensor->celsius();
      std::cout << start << " " << name << ": " << temp << std::endl;
  });
}

void FileOutput::run(DeviceRepo& repo) {
  json loop = this->config["loop"];
  if (loop.is_null()) {
    loop = json::object();
  }
  if (!loop.is_object()) {
    throw std::runtime_error("File output loop config is not an object");
  }

  int iterations = -1;
  int wait = 10;
  if (loop["count"].is_number()) {
    iterations = loop["count"];
  }
  if (loop["wait"].is_number()) {
    wait = loop["wait"];
  }

  if (iterations == -1) {
    while (true) {
      this->loop(repo);
      sleep(wait);
    }

  } else {
    for (int it = 0; it < iterations; it++) {
      this->loop(repo);
      if (it != iterations - 1) {
        sleep(wait);
      }
    }
  }
}

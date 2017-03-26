// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "thermal/config.h"

#include <exception>
#include <fstream>
#include <map>
#include <string>

#include <json.hpp>

#include "thermal/log.h"
#include "thermal/output.h"
#include "thermal/sensor.h"

// Include models to access factories.
#include "thermal/models/pcsensor/temper1f.h"

// Include outputs to access factories.
#include "thermal/outputs/file.h"
#include "thermal/outputs/prometheus.h"


using nlohmann::json;

using thermal::Configuration;
using thermal::DeviceRepo;
using thermal::Log;

using thermal::OutputRef;
using thermal::OutputFactory;
using thermal::SensorFactory;


// Map device models to factories.
std::map<std::string, SensorFactory> DEVICE_FACTORIES = {
  {"pcsensor/temper1f", thermal::pcsensor::TEMPer1F::Make}
};

// Map output runners to factories.
std::map<std::string, OutputFactory> OUTPUT_FACTORIES = {
  {"file", thermal::FileOutput::Make},
  {"prometheus", thermal::PrometheusOutput::Make}
};


// Map strings to log levels to load config.
std::map<std::string, Log::LEVEL> LOG_LEVEL_TRANSLATE = {
  {"error", Log::LEVEL::LL_ERROR},
  {"warn",  Log::LEVEL::LL_WARNING},
  {"info",  Log::LEVEL::LL_INFO},
  {"debug", Log::LEVEL::LL_DEBUG}
};


void Configuration::validate_device(json device) {
  if (!device["model"].is_string()) {
    throw std::runtime_error("All devices need a model");
  }
  if (!device["name"].is_string()) {
    throw std::runtime_error("All devices need a name");
  }

  std::string model = device["model"];
  if (DEVICE_FACTORIES.find(model) == DEVICE_FACTORIES.end()) {
    throw std::runtime_error("Cannot find factory for " + model);
  }
}

void Configuration::validate_devices() {
  json devices = this->config["devices"];
  if (!devices.is_array()) {
    throw std::runtime_error("Devices configuration is not a list");
  }
  for (json::iterator it = devices.begin(); it != devices.end(); ++it) {
    this->validate_device(*it);
  }
}

void Configuration::validate_log() {
  // Logging conf is optional;
  if (this->config["log"].is_null()) {
    return;
  }

  // Check log type.
  json log = this->config["log"];
  if (!log.is_object()) {
    throw std::runtime_error("Log configuration is not an object");
  }

  // Checl log level.
  if (!(log["level"].is_null() || log["level"].is_string())) {
    throw std::runtime_error("Log level is not a sting");
  }
  std::string level = log["level"];
  if (LOG_LEVEL_TRANSLATE.find(level) == LOG_LEVEL_TRANSLATE.end()) {
    throw std::runtime_error("Invalid log level '" + level + "'");
  }
}

void Configuration::validate_output() {
  // Check that an output object exists and has a regocgnised type.
  json output = this->config["output"];
  if (!output.is_object()) {
    throw std::runtime_error("Output configuration is not an object");
  }
  if (output["type"].is_null() || !output["type"].is_string()) {
    throw std::runtime_error("Output type is not a sting");
  }

  std::string type = output["type"];
  if (OUTPUT_FACTORIES.find(type) == OUTPUT_FACTORIES.end()) {
    throw std::runtime_error("Cannot find factory for " + type);
  }
}


Configuration::Configuration(std::string path) {
  this->path = path;
}

void Configuration::devices(DeviceRepo& repo) {
  Log::info() << "Configuring devices ..." << std::endl;
  json devs = this->config["devices"];

  for (json::iterator it = devs.begin(); it != devs.end(); ++it) {
    json device = *it;
    std::string model = device["model"];
    std::string name  = device["name"];

    Log::debug() << "Creating sensor '" << name << "' [model ";
    Log::debug() << model << "] ..." << std::endl;

    auto factory = DEVICE_FACTORIES[model];
    SensorRef sensor = factory(device);
    sensor->verify();
    sensor->initialise();
    repo.add_sensor(sensor, device);
  }

  Log::debug() << "Devices ready" << std::endl;
}

void Configuration::log() {
  Log::info() << "Configuring logs ..." << std::endl;
  Log::LEVEL level = Log::LEVEL::LL_INFO;
  if (!this->config["log"].is_null()) {
    std::string lvl = this->config["log"]["level"];
    level = LOG_LEVEL_TRANSLATE[lvl];
  }

  Log::init(level);
  Log::debug() << "Logs configured" << std::endl;
}

OutputRef Configuration::output() {
  Log::info() << "Configuring output ..." << std::endl;
  json output = this->config["output"];
  std::string type = output["type"];
  auto factory = OUTPUT_FACTORIES[type];
  return factory(output);
}

void Configuration::validate() {
  // Load the Json file.
  Log::info() << "Loading configuration from '" << this->path;
  Log::info() << "' ..." << std::endl;
  std::ifstream from(this->path);
  if (from.fail()) {
    throw std::runtime_error("Unable to open config file: " + this->path);
  }
  from >> this->config;

  // Make sure required stuff is there.
  Log::info() << "Validating configuration ..." << std::endl;
  this->validate_log();
  this->validate_devices();
  this->validate_output();
  Log::info() << "Configuration valid" << std::endl;
}

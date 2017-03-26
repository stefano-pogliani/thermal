// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "thermal/device-repo.h"

#include <json.hpp>

#include "thermal/sensor.h"

using nlohmann::json;
using thermal::DeviceRepo;
using thermal::SensorRef;


void DeviceRepo::add_sensor(SensorRef sensor, json config) {
  this->devices.push_back({config, config["name"], sensor});
}

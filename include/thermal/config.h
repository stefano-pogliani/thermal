// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef THERMAL_CONFIG_H_
#define THERMAL_CONFIG_H_

#include <string>
#include <json.hpp>

#include "thermal/device-repo.h"
#include "thermal/output.h"


namespace thermal {

  //! Load, validate, and apply configurations.
  class Configuration {
   protected:
    std::string path;
    nlohmann::json config;

    void validate_device(nlohmann::json device);
    void validate_devices();
    void validate_log();
    void validate_output();

   public:
    explicit Configuration(std::string path);

    void devices(DeviceRepo& repo);
    void log();
    OutputRef output();
    void validate();
  };

}  // namespace thermal

#endif  // THERMAL_CONFIG_H_

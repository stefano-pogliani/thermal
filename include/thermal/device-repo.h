// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef THERMAL_DEVICE_REPO_H_
#define THERMAL_DEVICE_REPO_H_

#include <string>
#include <json.hpp>

#include "thermal/sensor.h"

namespace thermal {

  //! Store and access configured devices.
  class DeviceRepo {
   public:
    //! Store sensor information for collections.
    class Device {
     public:
      nlohmann::json config;
      std::string name;
      SensorRef sensor;
    };

   protected:
    std::vector<DeviceRepo::Device> devices;

   public:
    void add_sensor(SensorRef sensor, nlohmann::json config);

    //! Iterates over all devices and passes them to the callback.
    void for_each(auto callback) {
      std::vector<DeviceRepo::Device>::iterator it;
      for (it = this->devices.begin(); it != this->devices.end(); it++) {
        callback(*it);
      }
    }
  };

}  // namespace thermal

#endif  // THERMAL_DEVICE_REPO_H_

// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef THERMAL_OUTPUT_H_
#define THERMAL_OUTPUT_H_

#include <memory>
#include <json.hpp>

#include "thermal/device-repo.h"

namespace thermal {

  //! Abstract output interface.
  class Output {
   protected:
    nlohmann::json config;

   public:
    Output(nlohmann::json config);

    //! Runs the output to collect and export temperatures.
    virtual void run(DeviceRepo& repo) = 0;
  };

  //! Reference counted Output.
  typedef std::shared_ptr<Output> OutputRef;

  //! Factory methods used in config.
  typedef OutputRef (*OutputFactory)(nlohmann::json config);

}  // namespace thermal

#endif  // THERMAL_OUTPUT_H_

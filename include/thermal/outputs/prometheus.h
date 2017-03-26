// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef THERMAL_OUTPUTS_PROMETHEUS_H_
#define THERMAL_OUTPUTS_PROMETHEUS_H_

#include <json.hpp>

#include "thermal/output.h"

namespace thermal {

  //! Run an HTTP server to expose devices in prometheus text format.
  class PrometheusOutput : public Output {
   public:
    static OutputRef Make(nlohmann::json config);

   public:
    PrometheusOutput(nlohmann::json config);
    void run(DeviceRepo& repo);

   protected:
    //! Returns the host to bind the server to.
    std::string host();

    //! Returns the port to listen on.
    std::string port();
  };

}  // namespace thermal

#endif  // THERMAL_OUTPUTS_PROMETHEUS_H_

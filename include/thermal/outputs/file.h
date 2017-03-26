// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef THERMAL_OUTPUTS_FILE_H_
#define THERMAL_OUTPUTS_FILE_H_

#include <ostream>
#include <string>
#include <json.hpp>

#include "thermal/output.h"


namespace thermal {

  //! Write output to file.
  class FileOutput : public Output {
   public:
    static OutputRef Make(nlohmann::json config);

   protected:
    void loop(DeviceRepo& repo);
    std::string now();

   public:
    FileOutput(nlohmann::json config);
    void run(DeviceRepo& repo);
  };

}  // namespace thermal

#endif  // THERMAL_OUTPUTS_FILE_H_

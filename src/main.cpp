#include <string.h>
#include <iostream>

#include "thermal/config.h"
#include "thermal/device-repo.h"
#include "thermal/log.h"
#include "thermal/output.h"
#include "thermal/sensor.h"

#define EXIT_ERROR -2
#define EXIT_USAGE -1

using thermal::Configuration;
using thermal::DeviceRepo;
using thermal::Log;
using thermal::OutputRef;
using thermal::SensorRef;


int main(int argc, char** argv) {
  // Initialise loggig to info until config is loaded.
  Log::init(Log::LEVEL::LL_INFO);

  // Load configuration.
  if (argc != 2) {
    std::cerr << "Usage: thermal CONFIG_PATH" << std::endl;
    std::cerr << "Thermal version: 0.1.1" << std::endl;
    return EXIT_USAGE;
  }

  // Instantiate main objects.
  std::string config_path(argv[1]);
  Configuration config(config_path);
  DeviceRepo repo;

  try {
    // Load config and initialise devices.
    config.validate();
    config.log();
    config.devices(repo);

    // Configure output and start collecting metrics.
    OutputRef out = config.output();
    out->run(repo);

    Log::info() << "Thermal exiting cleanly" << std::endl;
    return 0;

  } catch(std::exception& ex) {
    std::cerr << "Thermal encountered an error: " << ex.what() << std::endl;
    return EXIT_ERROR;
  }
}

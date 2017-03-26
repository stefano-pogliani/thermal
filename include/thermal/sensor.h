// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef THERMAL_SENSOR_H_
#define THERMAL_SENSOR_H_

#include <memory>
#include <json.hpp>


namespace thermal {

  //! Abstract definition of a sensor.
  /*!
   * All sensors can be used with the following interface:
   *
   *  SensorRef sensor = ...;  // some inti code
   *  sensor->verify();
   *  sensor->initialise();
   *  double temp = sensor->celsius();
   *  sensore->close();
   */
  class Sensor {
   public:
    virtual ~Sensor();

    //! Reads a temperature in Celsius degrees.
    virtual double celsius() = 0;

    //! Close the connection to the device and resets it.
    virtual void close() = 0;

    //! Initialise the device and prepare it for reads.
    virtual void initialise() = 0;

    //! Verify the existance and access of a device.
    virtual void verify() = 0;
  };

  //! Referece counted sensors.
  typedef std::shared_ptr<Sensor> SensorRef;

  //! Factory methods used in config.
  typedef SensorRef (*SensorFactory)(nlohmann::json config);

}  // namespace thermal

#endif  // THERMAL_SENSOR_H_

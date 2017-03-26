// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef THERMAL_MODELS_PCSENSOR_TEMPER1F_
#define THERMAL_MODELS_PCSENSOR_TEMPER1F_

#include "thermal/usb-sensor.h"


namespace thermal {
namespace pcsensor {

  //! PCSensor TEMPer1F interface.
  class TEMPer1F : public UsbSensor {
   public:
    static SensorRef Make(nlohmann::json config);

   protected:
    bool filter_bus_port;
    uint8_t bus;
    uint8_t port;

    void detach_kernel() const;
    bool filter_device(libusb_device* device) const;

    uint16_t product_id() const;
    uint16_t vendor_id() const;

    void claim_interfaces();
    void init_config();
    void init_device();

   public:
    TEMPer1F();
    TEMPer1F(uint8_t bus, uint8_t port);
    ~TEMPer1F();

    double celsius();
    void close();
    void initialise();
  };

}  // namespace pcsensor
}  // namespace thermal

#endif  // THERMAL_MODELS_PCSENSOR_TEMPER1F_

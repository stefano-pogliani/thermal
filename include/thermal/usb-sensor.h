// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef THERMAL_USB_SENSOR_H_
#define THERMAL_USB_SENSOR_H_

#include <libusb-1.0/libusb.h>
#include <exception>
#include <string>

#include "thermal/sensor.h"


namespace thermal {

  //! An error occured while dealing with libusb.
  class UsbError : public std::runtime_error {
   protected:
    int code;

   public:
    explicit UsbError(int code);
  };


  //! Thrown when a device is not found.
  class UsbDeviceNotFound : public std::runtime_error {
   public:
    UsbDeviceNotFound();
  };


  //! USB attached sensors.
  class UsbSensor : public Sensor {
   protected:
    struct libusb_context* usb_context;
    struct libusb_device* usb_device;
    struct libusb_device_handle* usb_handle;
    int timeout;

    //! Detach any attached kernel driver.
    virtual void detach_kernel() const = 0;

    //! Returns true if the instance should handle the given device.
    virtual bool filter_device(libusb_device* device) const;

    //! Iterates over USB devices to the sensor.
    /*!
     * The sensor is found when a USB device that matches
     *
     *   * VENDOR_ID
     *   * PRODUCT_ID
     *   * Serial Number
     *
     * is found.
     * This allows multiple devices of the same type to be
     * connected and correctly identified.
     */
    void find_device();

    // USB device identification.
    virtual uint16_t product_id() const = 0;
    virtual uint16_t vendor_id() const = 0;

    // USB device interaction.
    void control_transfer(
        uint8_t request_type, uint8_t request,
        uint16_t value, uint16_t index,
        unsigned char* data, uint16_t length
    );
    int interrupt_transfer(
        unsigned char endpoint, unsigned char* data, int length
    );

   public:
    UsbSensor();
    virtual ~UsbSensor();

    //! Reset and free the device.
    void close();

    //! Ensures that a device with the give ID exists.
    void verify();
  };

}  // namespace thermal

#endif  // THERMAL_USB_SENSOR_H_

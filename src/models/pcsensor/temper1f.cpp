// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "thermal/models/pcsensor/temper1f.h"

#include <string>
#include <json.hpp>

#include "thermal/log.h"
#include "thermal/usb-sensor.h"

// Device constants.
#define CONFIG_ID 0x01
#define INTERFACE1 0x00
#define INTERFACE2 0x01
#define RESPONSE_LEN 8
#define TEMP_ENDPOINT 0x82
#define TEMP_OFFSET 4
#define PRODUCT_ID 0x7401
#define VENDOR_ID 0x0C45

// Control questions.
#define QUESTION_INIT_DEVICE { 0x01, 0x01 }
#define QUESTION_INIT_INTERFACE1 { 0x01, 0x82, 0x77, 0x01, 0x00, 0x00, 0x00, 0x00 }
#define QUESTION_INIT_INTERFACE2 { 0x01, 0x86, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00 }
#define QUESTION_TEMPERATURE { 0x01, 0x80, 0x33, 0x01, 0x00, 0x00, 0x00, 0x00 }


using nlohmann::json;
using thermal::Log;
using thermal::SensorRef;
using thermal::UsbSensor;
using thermal::pcsensor::TEMPer1F;


SensorRef TEMPer1F::Make(json config) {
  json address = config["address"];
  if (address.is_object()) {
    uint8_t bus = address["bus"];
    uint8_t port = address["port"];
    return SensorRef(new TEMPer1F(bus, port));
  }
  return SensorRef(new TEMPer1F());
}


void TEMPer1F::detach_kernel() const {
  libusb_detach_kernel_driver(this->usb_handle, INTERFACE1);
  libusb_detach_kernel_driver(this->usb_handle, INTERFACE2);
}

bool TEMPer1F::filter_device(libusb_device* device) const {
  // Check bus and port numvers.
  uint8_t dev_bus = libusb_get_bus_number(device);
  uint8_t dev_port = libusb_get_port_number(device);
  bool port_match = !this->filter_bus_port || (
      dev_bus == this->bus && dev_port == this->port
  );

  // Make sure the model is supported.
  // TODO(stefano): make TEMPer1F abstract and create model based versions.
  bool model_match = true;

  // Done
  return port_match && model_match;
}

uint16_t TEMPer1F::product_id() const {
  return PRODUCT_ID;
}

uint16_t TEMPer1F::vendor_id() const {
  return VENDOR_ID;
}


void TEMPer1F::claim_interfaces() {
  int code = libusb_claim_interface(this->usb_handle, INTERFACE1);
  if (code) {
    throw UsbError(code);
  }
  code = libusb_claim_interface(this->usb_handle, INTERFACE2);
  if (code) {
    throw UsbError(code);
  }
}

void TEMPer1F::init_config() {
  int code = libusb_set_configuration(this->usb_handle, CONFIG_ID);
  if (code) {
    throw UsbError(code);
  }
}

void TEMPer1F::init_device() {
  /*
   * Send control commands to initialise the device.
   * This is based on the protocol as implemented here:
   * https://github.com/petervojtek/usb-thermometer/blob/master/pcsensor.c
   */
  // Initialise the device.
  unsigned char init_device[] = QUESTION_INIT_DEVICE;
  uint16_t len = sizeof init_device;
  this->control_transfer(0x21, 0x09, 0x0201, 0x00, init_device, len);

  // Ask for temperature.
  unsigned char answer[RESPONSE_LEN] = {0};
  unsigned char temp[] = QUESTION_TEMPERATURE;
  len = sizeof temp;
  this->control_transfer(0x21, 0x09, 0x0200, 0x01, temp, len);
  this->interrupt_transfer(TEMP_ENDPOINT, answer, RESPONSE_LEN);

  // Initialise interface one.
  unsigned char interface1[] = QUESTION_INIT_INTERFACE1;
  len = sizeof interface1;
  this->control_transfer(0x21, 0x09, 0x0200, 0x01, interface1, len);
  this->interrupt_transfer(TEMP_ENDPOINT, answer, RESPONSE_LEN);

  // Initialise interface two.
  unsigned char interface2[] = QUESTION_INIT_INTERFACE2;
  len = sizeof interface2;
  this->control_transfer(0x21, 0x09, 0x0200, 0x01, interface2, len);
  this->interrupt_transfer(TEMP_ENDPOINT, answer, RESPONSE_LEN);
  this->interrupt_transfer(TEMP_ENDPOINT, answer, RESPONSE_LEN);
}


TEMPer1F::TEMPer1F() : TEMPer1F(0, 0) {
  this->filter_bus_port = false;
}

TEMPer1F::TEMPer1F(uint8_t bus, uint8_t port) : UsbSensor() {
  this->bus = bus;
  this->port = port;
  this->filter_bus_port = true;
}

TEMPer1F::~TEMPer1F() {
  this->close();
}

double TEMPer1F::celsius() {
  // Ask for temperature.
  unsigned char answer[RESPONSE_LEN] = {0};
  unsigned char temp[] = QUESTION_TEMPERATURE;
  uint16_t len = sizeof temp;
  this->control_transfer(0x21, 0x09, 0x0200, 0x01, temp, len);
  this->interrupt_transfer(TEMP_ENDPOINT, answer, RESPONSE_LEN);

  // Decode temperature.
  float temperature = static_cast<float>(
      static_cast<signed char>(answer[TEMP_OFFSET]) +
      answer[TEMP_OFFSET+1] / 256.0f
  );
  return static_cast<double>(temperature);
}

void TEMPer1F::close() {
  // Release device interfaces.
  if (this->usb_handle) {
    libusb_release_interface(this->usb_handle, INTERFACE1);
    libusb_release_interface(this->usb_handle, INTERFACE2);
  }

  // Perform the rest of USB close.
  UsbSensor::close();
}

void TEMPer1F::initialise() {
  this->init_config();
  this->claim_interfaces();
  this->init_device();
}

// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "thermal/usb-sensor.h"

#include <libusb-1.0/libusb.h>
#include <exception>
#include <iostream>
#include <string>

#include "thermal/log.h"

using thermal::Log;
using thermal::UsbDeviceNotFound;
using thermal::UsbError;
using thermal::UsbSensor;


UsbDeviceNotFound::UsbDeviceNotFound() : std::runtime_error(
    "USB Device not found"
) {
  // Noop.
}

UsbError::UsbError(int code) : std::runtime_error("UsbError") {
  this->code = code;
}


UsbSensor::UsbSensor() {
  libusb_init(&this->usb_context);
  this->timeout = 5000;
  this->usb_device = nullptr;
  this->usb_handle = nullptr;
  if (Log::get_level() >= Log::LEVEL::LL_DEBUG) {
    libusb_set_debug(this->usb_context, LIBUSB_LOG_LEVEL_DEBUG);
  }
}

UsbSensor::~UsbSensor() {
  this->close();
}


bool UsbSensor::filter_device(libusb_device* device) const {
  return true;
}

void UsbSensor::find_device() {
  // Initialise the USB list.
  libusb_device** list;
  ssize_t count = libusb_get_device_list(this->usb_context, &list);
  if (count < 0) {
    throw UsbError(count);
  }

  // Iterate over devices.
  for (ssize_t idx = 0; idx < count; idx++) {
    libusb_device* device = list[idx];
    Log::debug() << "Inspecting device #" << idx << std::endl;

    // Check vendor and product id.
    libusb_device_descriptor descriptor;
    int ret = libusb_get_device_descriptor(device, &descriptor);
    if (ret != 0) {
      Log::warn() << "Failed to get device descriptor" << std::endl;
      continue;
    }
    if (descriptor.idVendor != this->vendor_id()) {
      Log::debug() << "Vendor ID does not match" << std::endl;
      continue;
    }
    if (descriptor.idProduct != this->product_id()) {
      Log::debug() << "Prodoct ID does not match" << std::endl;
      continue;
    }

    Log::debug() << "Product ID: 0x";
    Log::debug() << std::hex << descriptor.idProduct << std::endl;
    Log::debug() << "Vendor ID: 0x";
    Log::debug() << std::hex << descriptor.idVendor << std::endl;

    // Make sure this is the device we want.
    if (this->filter_device(device)) {
      this->usb_device = libusb_ref_device(device);
      int code = libusb_open(device, &this->usb_handle);
      if (code) {
        throw UsbError(code);
      }
      break;
    }
    Log::debug() << "Device #" << idx << " filtered out" << std::endl;
  }

  // Free allocated resources.
  libusb_free_device_list(list, 1);
  if (this->usb_device == nullptr) {
    throw UsbDeviceNotFound();
  }
}


void UsbSensor::control_transfer(
    uint8_t request_type, uint8_t request, uint16_t value,
    uint16_t index, unsigned char* data, uint16_t length
) {
  int code = libusb_control_transfer(
      this->usb_handle, request_type & 0xFF, request & 0xFF,
      value & 0xFFFF, index & 0xFFFF, data, length & 0xFFFF,
      this->timeout
  );
  if (code < 0) {
    throw UsbError(code);
  }
}

int UsbSensor::interrupt_transfer(
    unsigned char endpoint, unsigned char* data, int length
) {
  int response = 0;
  int code = libusb_interrupt_transfer(
      this->usb_handle, endpoint & 0xFF, data,
      length, &response, this->timeout
  );
  if (code) {
    throw UsbError(code);
  }
  return response;
}


void UsbSensor::close() {
  if (this->usb_handle) {
    libusb_reset_device(this->usb_handle);
    libusb_close(this->usb_handle);
    this->usb_handle = nullptr;
  }
  if (this->usb_device) {
    libusb_unref_device(this->usb_device);
    this->usb_device = nullptr;
  }
  if (this->usb_context) {
    libusb_exit(this->usb_context);
    this->usb_context = nullptr;
  }
}

void UsbSensor::verify() {
  this->find_device();
  this->detach_kernel();
}

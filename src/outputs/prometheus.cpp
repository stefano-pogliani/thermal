// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "thermal/outputs/prometheus.h"

#include <json.hpp>

#include <map>
#include <mutex>
#include <set>
#include <string>

#include <promclient/promclient.h>
#include <promclient/collector_registry.h>
#include <promclient/features/http.h>


using promclient::Collector;
using promclient::CollectorRegistry;
using promclient::CollectorRef;

using promclient::Descriptor;
using promclient::DescriptorRef;
using promclient::DescriptorsList;
using promclient::Metric;
using promclient::MetricsList;
using promclient::Sample;

using promclient::features::HttpExporter;

using thermal::DeviceRepo;
using thermal::Output;
using thermal::OutputRef;
using thermal::PrometheusOutput;
using thermal::SensorRef;


static std::string METRIC_NAME = "room_temp";
static std::string METRIC_HELP = "Room temperature in Celsius degrees";


class SensorGauge : public Collector {
 public:
  SensorGauge(
      SensorRef sensor,
      std::map<std::string, std::string> labels
  ) {
    this->labels_ = labels;
    this->sensor_ = sensor;

    std::set<std::string> label_names;
    for (auto pair : labels) {
      label_names.insert(pair.first);
    }

    this->descriptor_ = DescriptorRef(new Descriptor(
        METRIC_NAME, "gauge", METRIC_HELP, label_names
    ));
  }

  MetricsList collect() {
    // Grab the temperature (in a therad safe way).
    double value = 0;
    {
      std::lock_guard<std::mutex> lock(this->mutex_);
      value = this->sensor_->celsius();
    }

    // Build and return a list with the sample.
    MetricsList metrics;
    Sample sample("", value, this->labels_);
    metrics.push_back(Metric(this->descriptor_, {sample}));
    return metrics;
  }

  DescriptorsList describe() {
    return DescriptorsList({this->descriptor_});
  }

 protected:
  DescriptorRef descriptor_;
  std::map<std::string, std::string> labels_;

  std::mutex mutex_;
  SensorRef sensor_;
};


OutputRef PrometheusOutput::Make(nlohmann::json config) {
  return OutputRef(new PrometheusOutput(config));
};


PrometheusOutput::PrometheusOutput(nlohmann::json config) : Output(config) {
  // Noop.
}

void PrometheusOutput::run(DeviceRepo& repo) {
  // Create gauges for all devices.
  repo.for_each([this](auto device) {
      CollectorRef gauge = CollectorRef(new SensorGauge(
          device.sensor, {{"room", device.name}}
      ));
      CollectorRegistry::Default()->registr(gauge);
  });

  // Start the HTTP exporter to serve the temeratures.
  std::string host = this->host();
  std::string port = this->port();
  HttpExporter server(nullptr, host, port);
  server.mount();

  std::cout << "Metrics available at " << server.endpoint();
  std::cout << "/metrics" << std::endl;
  server.listen();
}


std::string PrometheusOutput::host() {
  std::string address = "127.0.0.1";
  nlohmann::json bind = this->config["bind"];
  if (!bind.is_null()) {
    if (!bind.is_string()) {
      throw std::runtime_error("Prometheus host is not a string");
    }
    address = bind;
  }
  return address;
}

std::string PrometheusOutput::port() {
  std::string port = "9123";
  nlohmann::json prt = this->config["port"];
  if (!prt.is_null()) {
    if (!prt.is_string() && !prt.is_number()) {
      throw std::runtime_error("Prometheus port not string or number");
    }
    if (prt.is_string()) {
      port = prt;
    } else {
      port = std::to_string(static_cast<int>(prt));
    }
  }
  return port;
}

// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "thermal/output.h"

using nlohmann::json;
using thermal::Output;


Output::Output(json config) {
  this->config = config;
}

// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#include "thermal/log.h"

#include <iostream>

using thermal::Log;


// NoOp std::ostream buffer.
class NullBuffer : public std::streambuf {
 public:
  int overflow(int c) {
    return c;
  }
};


NullBuffer null_buffer;
std::ostream Log::null(&null_buffer);
Log::LEVEL Log::level;


void Log::init(Log::LEVEL level) {
  Log::level = level;
}

Log::LEVEL Log::get_level() {
  return Log::level;
}


std::ostream& Log::debug() {
  if (Log::level >= Log::LEVEL::LL_DEBUG) {
    return std::cerr;
  }
  return Log::null;
}

std::ostream& Log::info() {
  if (Log::level >= Log::LEVEL::LL_INFO) {
    return std::cout;
  }
  return Log::null;
}

std::ostream& Log::warn() {
  if (Log::level >= Log::LEVEL::LL_WARNING) {
    return std::cerr;
  }
  return Log::null;
}

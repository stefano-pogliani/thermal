// Copyright 2017 Stefano Pogliani <stefano@spogliani.net>
#ifndef THERMAL_LOG_H_
#define THERMAL_LOG_H_

#include <iostream>


namespace thermal {

  class Log {
   public:
    enum LEVEL {
      LL_ERROR = 0,
      LL_WARNING,
      LL_INFO,
      LL_DEBUG
    };

   protected:
    static Log::LEVEL level;
    static std::ostream null;

   public:
    //! Initialise logging.
    static void init(Log::LEVEL level);
    static Log::LEVEL get_level();

    //! Returns a stream for debug messages.
    static std::ostream& debug();
    static std::ostream& info();
    static std::ostream& warn();
  };

}  // namespace thermal

#endif  // THERMAL_LOG_H_

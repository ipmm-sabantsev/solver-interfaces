#ifndef LOGGING_H
#define LOGGING_H

#include <string>
#include <ILog.h>

#define S1(x) #x
#define S2(x) S1(x)
#define LOCATION \
  std::string(__func__) + " " + __FILE__ + ":" S2(__LINE__) + " "

#define LOG(message) \
  ILog::report((std::string(LOCATION) + std::string(message)).c_str())

#endif // LOGGING_H

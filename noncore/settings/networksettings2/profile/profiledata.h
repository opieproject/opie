#ifndef PROFILE_DATA_H
#define PROFILE_DATA_H

#include <qstring.h>
typedef struct ProfileData {
      QString Description;
      // start up automatically
      bool Automatic;
      // if started up automatically, ask user for confirmation
      bool Confirm;
      // Do not bring this connection up
      bool Disabled;
} ProfileData_t; 

#endif

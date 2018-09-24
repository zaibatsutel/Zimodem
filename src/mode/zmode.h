/* zmode. */

#ifndef __zmode_h__
#define __zmode_h__

#include <Arduino.h>

class ZMode
{
  public:
    virtual void serialIncoming();
    virtual void loop();
};


extern ZMode *currMode;

# endif //__zmode_h__

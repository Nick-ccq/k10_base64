#ifndef _K10_BASE64_H_
#define _K10_BASE64_H_
#include "Arduino.h"
#include "unihiker_k10.h"

//#define ENABLE_DBG ///< Enable this macro to view the detailed execution process of the program.
#ifdef ENABLE_DBG
#define DBG(...) {Serial.print("[");Serial.print(__FUNCTION__); Serial.print("(): "); Serial.print(__LINE__); Serial.print(" ] "); Serial.println(__VA_ARGS__);}
#else
#define DBG(...)
#endif

class K10_base64
{
public:

    K10_base64(void);

    /**
      @fn K10tobase64
      @brief Capture current video frame from K10 camera and convert to base64 encoding
      @return Base64 encoded string
      */
    String K10tobase64(void);

    /**
     @fn imgtobase64
     @brief Convert image file (PNG/JPG) to base64 encoding
     @return Base64 encoded string
     */
    String imgtobase64(String img);


};

#endif

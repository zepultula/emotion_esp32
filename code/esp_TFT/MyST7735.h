#ifndef _MyST7735_
#define _MyST7735_

#include "Adafruit_ST77xx.h"
#include "Adafruit_ST7735.h"

#define ST7735_MADCTL_BGR 0x08
#define ST77XX_MADCTL_RGB 0x00

class MyST7735 : public Adafruit_ST7735 {

  public:
  MyST7735(int8_t cs, int8_t dc, int8_t rst);
  void init(void);
  void setRotation(uint8_t m);
  
};

#endif // _MyST7735_
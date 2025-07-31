#include "MyST7735.h"
#include "Adafruit_ST7735.h"
#include "Adafruit_ST77xx.h"

MyST7735::MyST7735(int8_t cs, int8_t dc, int8_t rst)
    : Adafruit_ST7735(cs, dc, rst) {}

/*************************************************************************
    init() to fix offset
    by calling protected function setColRowStart().
*************************************************************************/

void MyST7735::init(void) {
  initR(INITR_MINI160x80);
  setColRowStart(26, 1);
}

/*************************************************************************
    override setRotation() to fix color order using ST7735_MADCTL_BGR.
    To make it simple, option INITR_MINI160x80 is assumed and handled only.
*************************************************************************/
void MyST7735::setRotation(uint8_t m) {
  uint8_t madctl = 0;

  rotation = m & 3; // can't be higher than 3

  switch (rotation) {
  case 0:

    madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST7735_MADCTL_BGR;
    _height = ST7735_TFTHEIGHT_160;
    _width = ST7735_TFTWIDTH_80;
    _xstart = _colstart;
    _ystart = _rowstart;
    break;
  case 1:

    madctl = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST7735_MADCTL_BGR;
    _width = ST7735_TFTHEIGHT_160;
    _height = ST7735_TFTWIDTH_80;
    _ystart = _colstart;
    _xstart = _rowstart;
    break;
  case 2:
    madctl = ST7735_MADCTL_BGR;
    _height = ST7735_TFTHEIGHT_160;
    _width = ST7735_TFTWIDTH_80;
    _xstart = _colstart;
    _ystart = _rowstart;
    break;
  case 3:

    madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST7735_MADCTL_BGR;
    _width = ST7735_TFTHEIGHT_160;
    _height = ST7735_TFTWIDTH_80;
    _ystart = _colstart;
    _xstart = _rowstart;
    break;
  }

  sendCommand(ST77XX_MADCTL, &madctl, 1);
}
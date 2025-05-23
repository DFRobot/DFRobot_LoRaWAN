#ifndef __DFROBOT_ST7789_H
#define __DFROBOT_ST7789_H

#define ST7789_IC_WIDTH  135
#define ST7789_IC_HEIGHT  240
#define ST7789_COLSET  0x2A
#define ST7789_RAWSET  0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_MADCTL  0x36
#define ST7789_MADCTL_RGB 0x00
#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21



static const uint8_t PROGMEM DFRobot_ST7789_initCmd[]= {
0x01, 0x01, 0x80, 0, 150,
0x01, 0x11, 0x80, 0, 120,
0x01, 0x3A, 1, 0x55,
0x01, 0x36, 1, 0x00,
0x01, 0x21, 0,
0x01, 0x13, 0,
0x01, 0x29, 0,
0x00
};

static const uint8_t PROGMEM DFRobot_ST7789_R240x204_initCmd[]={
0x01, 0x01, 0x80, 0, 120,
0x01, 0x11, 0x80, 0, 120,
0x01, 0x36, 1, 0x00,
0x01, 0x3A, 1, 0x05,
0x01, 0xB2, 5, 0x0C, 0x0C, 0x00, 0x33, 0x33,
0x01, 0xB7, 1, 0x35,
0x01, 0xBB, 1, 0x1A,
0x01, 0xC0, 1, 0x2C,
0x01, 0xC2, 1, 0x01,
0x01, 0xC3, 1, 0x0B,
0x01, 0xC4, 1, 0x20,
0x01, 0xC6, 1, 0x0F,
0x01, 0xD0, 2, 0xA4, 0xA1,
0x01, 0x21, 0,
0x01, 0xE0, 14, 0x00, 0x19, 0x1E, 0x0A, 0x09, 0x15, 0x3D, 0x44, 0x51, 0x12, 0x03, 0x00, 0x3F, 0x3F,
0x01, 0xE1, 14, 0x00, 0x18, 0x1E, 0x0A, 0x09, 0x25, 0x3F, 0x43, 0x52, 0x33, 0x03, 0x00, 0x3F, 0x3F,
0x01, 0x35, 0x80, 0, 10,
0x01, 0x29, 0,
0x00
};
#endif

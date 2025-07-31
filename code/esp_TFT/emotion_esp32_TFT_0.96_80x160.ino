/*
 * ===============================================================
 * ==      DasaiOled - The Final Color GIF Player Version      ==
 * ===============================================================
 * Software: ผสานพลังระหว่างไลบรารี Adafruit และคลาส MyST7735
 * Hardware: ใช้การเดินสายไฟและคาถากำกับสำหรับ ESP32-C3-Zero
 * Mission: แสดงผล GIF สีสันสดใส!
 */

#include <Adafruit_GFX.h>    // Core graphics library
#include "MyST7735.h"        // ไลบรารีแก้ไขของนายท่าน
#include <SPI.h>
#include <AnimatedGIF.h>
#include "animation.h"       // <-- อย่าลืมใส่ไฟล์ animation.h ของท่าน

// --- การตั้งค่า Pin ตามที่นายท่านระบุ ---
#define TFT_SCK    4
#define TFT_MOSI   6
#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8

// --- สร้าง Object สำหรับจอและ GIF ---
MyST7735 tft = MyST7735(TFT_CS, TFT_DC, TFT_RST);
AnimatedGIF gif;

// --- ผู้ช่วยส่วนตัว (Global Variables) สำหรับจัดตำแหน่ง GIF ---
int gif_x_offset = 0;
int gif_y_offset = 0;

//
// ฟังก์ชัน Callback สำหรับวาด GIF (เวอร์ชัน Adafruit)
//
void GIFDraw(GIFDRAW *pDraw) {
  uint8_t *s;
  uint16_t usColor;
  int x, y;

  y = pDraw->iY + pDraw->y; // คำนวณตำแหน่งบรรทัดที่จะวาด

  s = pDraw->pPixels;
  // วาดทีละพิกเซล (วิธีนี้จัดการความโปร่งใสได้ถูกต้อง)
  for (x = 0; x < pDraw->iWidth; x++) {
    // ถ้าพิกเซลไม่ใช่สีโปร่งใส
    if (s[x] != pDraw->ucTransparent) {
      usColor = pDraw->pPalette[s[x]]; // แปลง index เป็นสีจริง
      // --- เพิ่มตำแหน่งจากผู้ช่วยส่วนตัวเข้าไป! ---
      tft.drawPixel(pDraw->iX + x + gif_x_offset, y + gif_y_offset, usColor);
    }
  }
}

// Wrapper สำหรับเล่นไฟล์ GIF
void playWrapper(uint8_t* gifinput, int size) {
  if (gif.open(gifinput, size, GIFDraw)) {
    Serial.printf("Playing GIF, size: %d bytes\n", size);
    
    // --- สั่งงานผู้ช่วยส่วนตัวให้คำนวณตำแหน่งกึ่งกลาง ---
    gif_x_offset = (tft.width() - gif.getCanvasWidth()) / 2;
    gif_y_offset = (tft.height() - gif.getCanvasHeight()) / 2;

    while (gif.playFrame(true, NULL)) {
      // ปล่อยให้มหัศจรรย์เกิดขึ้น!
    }
    gif.close();
  } else {
    Serial.println("Error opening GIF!");
  }
}

// --- ส่วนของ Loop และการสุ่ม Animation (เหมือนเดิม) ---
#define NUMBEROFANIMATION 32
int n = NUMBEROFANIMATION;
int r;
uint8_t last_animation = 0;
int debugRandom = 0;
int counter = 0;

void setup(void) {
  Serial.begin(115200);
  Serial.println("Final GIF Player is ALIVE!");

  // --- คาถากำกับ! (ขั้นตอนสำคัญ) ---
  SPI.begin(TFT_SCK, -1, TFT_MOSI, -1);

  // เริ่มการทำงานของจอด้วยคลาสแก้ไขของเรา
  tft.init();
  Serial.println("TFT Initialized with custom offset fix!");

  tft.setRotation(1); // ตั้งค่าการหมุนจอให้ถูกต้อง
  tft.fillScreen(ST77XX_BLACK); // เคลียร์หน้าจอ

  // เริ่มการทำงานของ AnimatedGIF
  gif.begin(LITTLE_ENDIAN_PIXELS);
  
  // เล่น GIF ต้อนรับ
  playWrapper((uint8_t*)_31, sizeof(_31));
}

void loop() {
  delay(random(3000, 7000)); // หน่วงเวลาแบบสุ่ม

  // --- Logic การสุ่มเลือก GIF ---
  if (debugRandom == 0) {
    do {
      r = random(1, n + 1);
    } while (r == last_animation);
    last_animation = r;
  } else {
    counter = (counter % n) + 1;
    r = counter;
  }
  
  Serial.print("Playing animation number: ");
  Serial.println(r);

  // --- เปลี่ยนจาก switch มาใช้ if-else if เพื่อความแน่นอน! ---
  if (r == 1) { playWrapper((uint8_t*)_1, sizeof(_1)); }
  else if (r == 2) { playWrapper((uint8_t*)_2, sizeof(_2)); }
  else if (r == 3) { playWrapper((uint8_t*)_3, sizeof(_3)); }
  else if (r == 4) { playWrapper((uint8_t*)_4, sizeof(_4)); }
  else if (r == 5) { playWrapper((uint8_t*)_5, sizeof(_5)); }
  else if (r == 6) { playWrapper((uint8_t*)_6, sizeof(_6)); }
  else if (r == 7) { playWrapper((uint8_t*)_40, sizeof(_40)); }
  else if (r == 8) { playWrapper((uint8_t*)_8, sizeof(_8)); }
  else if (r == 9) { playWrapper((uint8_t*)_9, sizeof(_9)); }
  else if (r == 10) { playWrapper((uint8_t*)_10, sizeof(_10)); }
  else if (r == 11) { playWrapper((uint8_t*)_36, sizeof(_36)); }
  else if (r == 12) { playWrapper((uint8_t*)_41, sizeof(_41)); }
  else if (r == 13) { playWrapper((uint8_t*)_13, sizeof(_13)); }
  else if (r == 14) { playWrapper((uint8_t*)_14, sizeof(_14)); }
  else if (r == 15) { playWrapper((uint8_t*)_34, sizeof(_34)); }
  else if (r == 16) { playWrapper((uint8_t*)_16, sizeof(_16)); }
  else if (r == 17) { playWrapper((uint8_t*)_35, sizeof(_35)); }
  else if (r == 18) { playWrapper((uint8_t*)_18, sizeof(_18)); }
  else if (r == 19) { playWrapper((uint8_t*)_19, sizeof(_19)); }
  else if (r == 20) { playWrapper((uint8_t*)_33, sizeof(_33)); }
  else if (r == 21) { playWrapper((uint8_t*)_21, sizeof(_21)); }
  else if (r == 22) { playWrapper((uint8_t*)_22, sizeof(_22)); }
  else if (r == 23) { playWrapper((uint8_t*)_23, sizeof(_23)); }
  else if (r == 24) { playWrapper((uint8_t*)_24, sizeof(_24)); }
  else if (r == 25) { playWrapper((uint8_t*)_25, sizeof(_25)); }
  else if (r == 26) { playWrapper((uint8_t*)_32, sizeof(_32)); }
  else if (r == 27) { playWrapper((uint8_t*)_37, sizeof(_37)); }
  else if (r == 28) { playWrapper((uint8_t*)_28, sizeof(_28)); }
  else if (r == 29) { playWrapper((uint8_t*)_29, sizeof(_29)); }
  else if (r == 30) { playWrapper((uint8_t*)_30, sizeof(_30)); }
  else if (r == 31) { playWrapper((uint8_t*)_42, sizeof(_42)); }
  else if (r == 32) { playWrapper((uint8_t*)_39, sizeof(_39)); }
  else if (r == 33) { playWrapper((uint8_t*)colorful_anime_intro, sizeof(colorful_anime_intro)); }
  else {
    Serial.println("Invalid animation number!");
  }
}

#include <SPI.h>
#include <TFT_eSPI.h> // <--- เปลี่ยนมาใช้ไลบรารีจอสี TFT_eSPI
#include <AnimatedGIF.h>
#include "animation.h" // <-- อย่าลืมใส่ไฟล์ animation.h ของท่าน

// สร้าง Object สำหรับจอ TFT และ GIF
TFT_eSPI tft = TFT_eSPI();
AnimatedGIF gif;

// ฟังก์ชัน Callback สำหรับวาด GIF (เวอร์ชันอัปเกรดสำหรับจอสี!)
// ฟังก์ชันนี้จะถูกเรียกโดยไลบรารี AnimatedGIF ทุกครั้งที่ถอดรหัสข้อมูลภาพได้ 1 บรรทัด
void GIFDraw(GIFDRAW *pDraw) {
    uint8_t *s;
    uint16_t *p, usColor;
    int x, y, iWidth;

    // กำหนดตำแหน่งที่จะวาดบนหน้าจอ
    y = pDraw->iY + pDraw->y; 
    iWidth = pDraw->iWidth;

    // สร้าง buffer ชั่วคราวสำหรับเก็บข้อมูลสีของภาพ 1 บรรทัด
    // เพื่อการวาดที่รวดเร็วขึ้นด้วย tft.pushImage()
    uint16_t lineBuffer[iWidth];
    p = lineBuffer;

    s = pDraw->pPixels;
    // แปลงข้อมูลพิกเซล (ที่เป็น index ของสี) ให้กลายเป็นสีจริงๆ (RGB565)
    // โดยดึงสีจากตารางสี (Palette) ที่มากับไฟล์ GIF
    for (x = 0; x < iWidth; x++) {
        // ถ้าพิกเซลนี้ไม่ใช่สีโปร่งใส
        if (s[x] != pDraw->ucTransparent) {
            // ดึงสีจาก Palette
            usColor = pDraw->pPalette[s[x]];
            // ใส่สีลงใน buffer ของเรา
            *p++ = usColor;
        } else {
            // ถ้าเป็นสีโปร่งใส ก็ข้ามไป (ไม่วาดทับของเก่า)
            // หรือจะเติมสีพื้นหลังก็ได้
            // *p++ = TFT_BLACK; // ตัวอย่างการเติมสีดำ
            
            // เพื่อให้โปร่งใสจริงๆ เราจะวาดพิกเซลทีละจุดแทนการ push ทั้งบรรทัด
            // ถ้าในบรรทัดนั้นมีพิกเซลที่โปร่งใสอยู่
            if (x > 0) {
                tft.pushImage(pDraw->iX, y, x, 1, lineBuffer);
            }
            // ย้ายตำแหน่งวาดไปต่อจากพิกเซลที่โปร่งใส
            tft.setCursor(pDraw->iX + x + 1, y);
            // รีเซ็ต buffer
            p = lineBuffer;
        }
    }
    // วาดส่วนที่เหลือของบรรทัด (ถ้ามี)
    if (p > lineBuffer) {
        tft.pushImage(tft.getCursorX(), y, p - lineBuffer, 1, lineBuffer);
    }
}


// Wrapper สำหรับเล่นไฟล์ GIF
void playWrapper(uint8_t* gifinput, int size) {
  // เปิดไฟล์ GIF จากหน่วยความจำ
  if (gif.open(gifinput, size, GIFDraw)) {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    // เล่น Animation ไปเรื่อยๆ จนจบ
    while (gif.playFrame(true, NULL)) {
    }
    gif.close();
  } else {
    Serial.println("Error opening GIF!");
  }
}

// --- ส่วนของ Loop และการสุ่ม Animation ยังคงเหมือนเดิม ---

#define NUMBEROFANIMATION 32
int n = NUMBEROFANIMATION;
int r;
uint8_t last_animation = 0;
int debugRandom = 0;
int counter = 0;


void setup() {
  Serial.begin(115200);
  Serial.println("TFT GIF Player Starting...");

  // เริ่มการทำงานของจอ TFT
  tft.init();
  tft.setRotation(1); // ตั้งค่าการหมุนจอ (ลองปรับเป็น 0, 1, 2, 3 เพื่อให้ได้ทิศทางที่ถูกต้อง)
  tft.fillScreen(TFT_BLACK); // เคลียร์หน้าจอเป็นสีดำ

  // เริ่มการทำงานของ AnimatedGIF
  // ESP32 ใช้ LITTLE_ENDIAN_PIXELS ถูกต้องแล้วค่ะ
  gif.begin(LITTLE_ENDIAN_PIXELS);

  // เล่น GIF ต้อนรับเมื่อเปิดเครื่อง
  playWrapper((uint8_t*)_31, sizeof(_31));
}

void loop() {
  // หน่วงเวลาแบบสุ่มก่อนเล่น animation ถัดไป
  r = random(1, 3) * 5000; // ลดเวลาหน่วงลงเล็กน้อย
  Serial.print("Waiting for ");
  Serial.print(r);
  Serial.println(" ms...");
  delay(r);

  // --- Logic การสุ่มเลือก GIF ---
  if (debugRandom == 0) {
    do {
      r = random(1, n + 1);
    } while (r == last_animation); // สุ่มใหม่จนกว่าจะได้เลขไม่ซ้ำกับรอบที่แล้ว
    last_animation = r;
  } else {
    counter++;
    if (counter > n) {
      counter = 1;
    }
    r = counter;
  }
  
  Serial.print("Playing animation number: ");
  Serial.println(r);

  // เลือกเล่น GIF ตามเลขที่สุ่มได้
  switch (r) {
    case 1: playWrapper((uint8_t*)_1, sizeof(_1)); break;
    case 2: playWrapper((uint8_t*)_2, sizeof(_2)); break;
    case 3: playWrapper((uint8_t*)_3, sizeof(_3)); break;
    case 4: playWrapper((uint8_t*)_4, sizeof(_4)); break;
    case 5: playWrapper((uint8_t*)_5, sizeof(_5)); break;
    case 6: playWrapper((uint8_t*)_6, sizeof(_6)); break;
    case 7: playWrapper((uint8_t*)_40, sizeof(_40)); break;
    case 8: playWrapper((uint8_t*)_8, sizeof(_8)); break;
    case 9: playWrapper((uint8_t*)_9, sizeof(_9)); break;
    case 10: playWrapper((uint8_t*)_10, sizeof(_10)); break;
    case 11: playWrapper((uint8_t*)_36, sizeof(_36)); break;
    case 12: playWrapper((uint8_t*)_41, sizeof(_41)); break;
    case 13: playWrapper((uint8_t*)_13, sizeof(_13)); break;
    case 14: playWrapper((uint8_t*)_14, sizeof(_14)); break;
    case 15: playWrapper((uint8_t*)_34, sizeof(_34)); break;
    case 16: playWrapper((uint8_t*)_16, sizeof(_16)); break;
    case 17: playWrapper((uint8_t*)_35, sizeof(_35)); break;
    case 18: playWrapper((uint8_t*)_18, sizeof(_18)); break;
    case 19: playWrapper((uint8_t*)_19, sizeof(_19)); break;
    case 20: playWrapper((uint8_t*)_33, sizeof(_33)); break;
    case 21: playWrapper((uint8_t*)_21, sizeof(_21)); break;
    case 22: playWrapper((uint8_t*)_22, sizeof(_22)); break;
    case 23: playWrapper((uint8_t*)_23, sizeof(_23)); break;
    case 24: playWrapper((uint8_t*)_24, sizeof(_24)); break;
    case 25: playWrapper((uint8_t*)_25, sizeof(_25)); break;
    case 26: playWrapper((uint8_t*)_32, sizeof(_32)); break;
    case 27: playWrapper((uint8_t*)_37, sizeof(_37)); break;
    case 28: playWrapper((uint8_t*)_28, sizeof(_28)); break;
    case 29: playWrapper((uint8_t*)_29, sizeof(_29)); break;
    case 30: playWrapper((uint8_t*)_30, sizeof(_30)); break;
    case 31: playWrapper((uint8_t*)_42, sizeof(_42)); break;
    case 32: playWrapper((uint8_t*)_39, sizeof(_39)); break;
    default: Serial.println("Invalid animation number!"); break;
  }
}

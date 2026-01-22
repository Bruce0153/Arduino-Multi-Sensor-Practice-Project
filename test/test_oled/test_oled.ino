#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// 尝试不同的SSD1315兼容型号（根据库版本选择）
// 若仍报错，可在U8g2库的examples中查看支持的SSD1315型号
// U8G2_SSD1315_128X64_VISIONOX_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// 如果上面的型号仍报错，尝试用SSD1306的兼容模式（部分SSD1315可兼容）
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup(void) {
  // 初始化I2C通信
  Wire.begin();
  // 初始化显示屏
  u8g2.begin();
  // 开启UTF8支持以显示汉字
  u8g2.enableUTF8Print();
}

void loop(void) {
  u8g2.clearBuffer();
  
  // 选择支持汉字的字体（确保字体存在）
  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
  
  // 显示汉字
  u8g2.drawStr(0, 20, "你好，世界！");
  u8g2.drawStr(0, 40, "OLED显示测试");
  u8g2.drawStr(0, 60, "请检查库版本");
  
  u8g2.sendBuffer();
  delay(3000);
}

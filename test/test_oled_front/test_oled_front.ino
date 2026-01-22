#include <Wire.h>
#include "font.h"  // 引入自定义字体库

// 定义SSD1315 OLED的I2C地址（根据实际模块修改，通常是0x3C或0x3D）
#define OLED_ADDR   0x3C
// 屏幕分辨率（0.96寸通常为128x64）
#define WIDTH       128
#define HEIGHT      64

void setup() {
  Wire.begin();       // 初始化I2C通信
  OLED_Init();        // 初始化OLED屏幕
  OLED_Clear();       // 清屏
  
  // 显示汉字（坐标x从0开始，y按页划分，每页8像素）
  // 假设font.h中包含"你好，世界！"的点阵数据
  OLED_ShowChinese(0, 0, 0);   // 显示第0个汉字"你"
  OLED_ShowChinese(16, 0, 1);  // 显示第1个汉字"好"
  OLED_ShowChinese(32, 0, 2);  // 显示第2个汉字"，"
  OLED_ShowChinese(48, 0, 3);  // 显示第3个汉字"世"
  OLED_ShowChinese(64, 0, 4);  // 显示第4个汉字"界"
  OLED_ShowChinese(80, 0, 5);  // 显示第5个汉字"！"
  
  // 显示一行英文（假设font.h支持ASCII字符）
  OLED_ShowString(0, 2, "SSD1315 Display");
}

void loop() {
  // 无需循环操作，静态显示
}

// OLED初始化函数
void OLED_Init() {
  // 发送SSD1315初始化命令（具体命令需参考你的模块数据手册）
  OLED_WriteCommand(0xAE);  // 关闭显示
  OLED_WriteCommand(0x00);  // 设置列地址低4位
  OLED_WriteCommand(0x10);  // 设置列地址高4位
  OLED_WriteCommand(0x40);  // 设置起始行
  OLED_WriteCommand(0xB0);  // 设置页地址
  OLED_WriteCommand(0x81);  // 设置对比度
  OLED_WriteCommand(0xFF);  // 最大对比度
  OLED_WriteCommand(0xA1);  // 左右反置（根据屏幕实际显示调整）
  OLED_WriteCommand(0xA6);  // 正常显示（非反显）
  OLED_WriteCommand(0xA8);  // 设置多路复用率
  OLED_WriteCommand(0x3F);  // 64行
  OLED_WriteCommand(0xA4);  // 显示全部点阵（不强制点亮）
  OLED_WriteCommand(0xD3);  // 设置显示偏移
  OLED_WriteCommand(0x00);  // 无偏移
  OLED_WriteCommand(0xD5);  // 设置时钟分频
  OLED_WriteCommand(0x80);  // 默认值
  OLED_WriteCommand(0xD9);  // 设置预充电周期
  OLED_WriteCommand(0xF1);  // 默认值
  OLED_WriteCommand(0xDA);  // 设置COM引脚配置
  OLED_WriteCommand(0x12);  // 默认值
  OLED_WriteCommand(0xDB);  // 设置VCOMH电压
  OLED_WriteCommand(0x40);  // 默认值
  OLED_WriteCommand(0xAF);  // 开启显示
}

// 向OLED写入命令
void OLED_WriteCommand(byte cmd) {
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x00);  // 命令模式（Co=0, D/C#=0）
  Wire.write(cmd);
  Wire.endTransmission();
}

// 向OLED写入数据
void OLED_WriteData(byte data) {
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x40);  // 数据模式（Co=0, D/C#=1）
  Wire.write(data);
  Wire.endTransmission();
}

// 清屏函数
void OLED_Clear() {
  for (int page = 0; page < 8; page++) {
    OLED_WriteCommand(0xB0 + page);  // 设置页地址
    OLED_WriteCommand(0x00);         // 列地址低4位
    OLED_WriteCommand(0x10);         // 列地址高4位
    for (int col = 0; col < 128; col++) {
      OLED_WriteData(0x00);  // 写入0，清除该点
    }
  }
}

// 显示汉字函数（x:列坐标, y:页坐标(0-7), index:汉字在font.h中的索引）
void OLED_ShowChinese(int x, int y, int index) {
  // 假设每个汉字为16x16点阵，占用32字节（font.h中定义为unsigned char HZ_K[][32]）
  OLED_WriteCommand(0xB0 + y);                  // 设置页地址
  OLED_WriteCommand(0x00 + (x & 0x0F));         // 列地址低4位
  OLED_WriteCommand(0x10 + ((x >> 4) & 0x0F));  // 列地址高4位
  
  // 从font.h中读取汉字点阵数据并写入
  for (int i = 0; i < 16; i++) {
    OLED_WriteData(HZ_K[index][i]);       // 上半部分8行
  }
  
  // 处理16x16点阵的下半部分（第二页）
  OLED_WriteCommand(0xB0 + y + 1);              // 下一页
  OLED_WriteCommand(0x00 + (x & 0x0F));         // 同一列地址
  OLED_WriteCommand(0x10 + ((x >> 4) & 0x0F));
  
  for (int i = 16; i < 32; i++) {
    OLED_WriteData(HZ_K[index][i]);       // 下半部分8行
  }
}

// 显示字符串函数（x:列坐标, y:页坐标, str:要显示的字符串）
void OLED_ShowString(int x, int y, char* str) {
  int i = 0;
  while (str[i] != '\0') {
    // 假设ASCII字符为8x16点阵，占用16字节（font.h中定义为unsigned char ASC16[]）
    OLED_WriteCommand(0xB0 + y);
    OLED_WriteCommand(0x00 + (x & 0x0F));
    OLED_WriteCommand(0x10 + ((x >> 4) & 0x0F));
    
    // 写入字符上半部分
    for (int j = 0; j < 8; j++) {
      OLED_WriteData(ASC16[(str[i] - 0x20) * 16 + j]);
    }
    
    // 写入字符下半部分
    OLED_WriteCommand(0xB0 + y + 1);
    OLED_WriteCommand(0x00 + (x & 0x0F));
    OLED_WriteCommand(0x10 + ((x >> 4) & 0x0F));
    
    for (int j = 8; j < 16; j++) {
      OLED_WriteData(ASC16[(str[i] - 0x20) * 16 + j]);
    }
    
    x += 8;  // 每个ASCII字符占8列
    i++;
  }
}

#include <OneWire.h>
#include <DallasTemperature.h>

// 定义DS18B20连接的引脚（这里使用数字引脚2）
#define ONE_WIRE_BUS 2

// 初始化单总线
OneWire oneWire(ONE_WIRE_BUS);

// 初始化DallasTemperature库
DallasTemperature sensors(&oneWire);

void setup() {
  // 初始化串口通信，波特率9600
  Serial.begin(9600);
  
  // 等待串口连接
  while (!Serial) {
    ; // 等待串口就绪
  }
  
  // 初始化温度传感器
  sensors.begin();
  
  Serial.println("DS18B20温度传感器测试程序启动...");
}

void loop() {
  // 发送命令获取温度
  sensors.requestTemperatures();
  
  // 读取温度值（摄氏度）
  float temperatureC = sensors.getTempCByIndex(0);
  
  // 检查是否读取成功
  if (temperatureC != DEVICE_DISCONNECTED_C) {
    Serial.print("当前温度: ");
    Serial.print(temperatureC);
    Serial.println(" °C");
  } else {
    Serial.println("无法读取传感器数据，请检查连接！");
  }
  
  // 每1秒读取一次
  delay(1000);
}
    

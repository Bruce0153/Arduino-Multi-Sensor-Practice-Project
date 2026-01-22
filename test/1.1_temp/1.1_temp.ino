


#include <OneWire.h>
#include <DallasTemperature.h>

// 定义DS18B20连接的引脚
#define ONE_WIRE_BUS 2

// 定义LED引脚
#define LED_LOW 5    // 温度低于10度时闪烁的LED
#define LED_MID 6    // 温度10-20度时常亮的LED
#define LED_HIGH 7   // 温度高于20度时闪烁的LED

// 初始化单总线和温度传感器库
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// 用于LED闪烁的变量
unsigned long previousMillis = 0;
const long interval = 500;  // 闪烁间隔时间(毫秒)
bool ledState = LOW;

void setup() {
  // 初始化LED引脚为输出模式
  pinMode(LED_LOW, OUTPUT);
  pinMode(LED_MID, OUTPUT);
  pinMode(LED_HIGH, OUTPUT);
  
  // 初始化串口通信
  Serial.begin(9600);
  
  // 初始化温度传感器
  sensors.begin();
  
  // 初始状态关闭所有LED
  digitalWrite(LED_LOW, LOW);
  digitalWrite(LED_MID, LOW);
  digitalWrite(LED_HIGH, LOW);
  
  Serial.println("温度控制LED系统启动...");
}

void loop() {
  // 读取温度
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);
  
  // 检查温度读取是否成功
  if (temperature != DEVICE_DISCONNECTED_C) {
    Serial.print("当前温度: ");
    Serial.print(temperature);
    Serial.println(" °C");
    
    // 控制LED状态
    controlLEDs(temperature);
  } else {
    Serial.println("无法读取传感器数据，请检查连接！");
    // 传感器出错时关闭所有LED
    turnOffAllLEDs();
  }
  
  delay(1000);  // 每秒读取一次温度
}

// 根据温度控制LED
void controlLEDs(float temp) {
  // 先关闭所有LED
  turnOffAllLEDs();
  
  // 获取当前时间用于LED闪烁控制
  unsigned long currentMillis = millis();
  
  // 温度低于10度：LOW LED闪烁
  if (temp < 20.0) {
    // 定时翻转LED状态实现闪烁
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_LOW, ledState);
    }
  }
  // 温度在10-20度之间：MID LED常亮
  else if (temp >= 20.0 && temp <= 25.0) {
    digitalWrite(LED_MID, HIGH);
  }
  // 温度高于20度：HIGH LED闪烁
  else {
    // 定时翻转LED状态实现闪烁
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LED_HIGH, ledState);
    }
  }
}

// 关闭所有LED
void turnOffAllLEDs() {
  digitalWrite(LED_LOW, LOW);
  digitalWrite(LED_MID, LOW);
  digitalWrite(LED_HIGH, LOW);
}
    

// 定义引脚
const int sensorPin = A0;       // 颜色传感器连接A0
const int ledPin = 5;           // LED连接引脚5
const int buzzerPin = 12;         // 蜂鸣器连接引脚6
const int resetButtonPin = 0;   // 复位按钮连接引脚2

// 定义变量
float sensorValue = 0;          // 存储传感器值
float threshold = 50.0;         // 报警阈值（可根据需要调整）
bool alarmActive = false;       // 报警状态标志
unsigned long previousMillis = 0; // 用于LED闪烁的时间变量
const long interval = 500;      // LED闪烁间隔（毫秒）

void setup() {
  // 初始化引脚模式
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(resetButtonPin, INPUT_PULLUP); // 按钮使用内部上拉电阻
  
  // 初始化串口通信
  Serial.begin(115200);
  Serial.println("TS300B颜色传感器报警系统启动...");
  Serial.print("报警阈值设置为: ");
  Serial.println(threshold);
  
  // 初始状态关闭LED和蜂鸣器
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);
}

void loop() {
  // 读取传感器值并转换
  sensorValue = analogRead(sensorPin);
  sensorValue = (sensorValue * 100 / 1024); // 转换为0-100范围
  
  // 打印传感器值
  Serial.print("传感器值: ");
  Serial.println(sensorValue);
  
  // 读取复位按钮状态（LOW表示按下）
  bool resetPressed = (digitalRead(resetButtonPin) == LOW);
  
  // 如果按下复位按钮，停止报警
  if (resetPressed) {
    alarmActive = false;
    stopAlarm();
    delay(100); // 简单消抖
  }
  
  // 检测传感器值是否超过阈值，激活报警
  if (sensorValue < threshold && !alarmActive) {
    alarmActive = true;
    Serial.println("超过阈值，启动报警！");
  }
  
  // 如果报警激活，执行报警动作
  if (alarmActive) {
    triggerAlarm();
  }
  
  delay(300); // 检测间隔
}

// 触发报警函数
void triggerAlarm() {
  unsigned long currentMillis = millis();
  
  // 控制LED和蜂鸣器同步闪烁
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // 翻转LED和蜂鸣器状态
    int state = digitalRead(ledPin);
    digitalWrite(ledPin, !state);
    digitalWrite(buzzerPin, !state);
  }
}

// 停止报警函数
void stopAlarm() {
  digitalWrite(ledPin, LOW);    // 关闭LED
  digitalWrite(buzzerPin, LOW); // 关闭蜂鸣器
  Serial.println("报警已重置");
}

#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* 引脚 */
const int BUZZER_PIN = 8;      // 蜂鸣器（tone）
const int BUTTON_PIN = 7;      // 复位按钮（默认上拉，按下为低）
const int OLED_RESET = -1;     // I2C OLED 常用 -1

/* OLED */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* 设备 */
RTC_DS3231 rtc;

/* 喂食时间（小时与分钟） */
const uint8_t NUM_FEEDS = 2;
uint8_t feedHours[NUM_FEEDS]   = {9, 17};
uint8_t feedMinutes[NUM_FEEDS] = {0,  0};

/* 触发标记与跨日清零 */
bool triggeredToday[NUM_FEEDS] = {false, false};
int lastDay = -1;

/* 按钮消抖 */
const bool BUTTON_USE_PULLUP = true;
const bool BUTTON_ACTIVE_LOW = true;  // 按下为低电平
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
int lastButtonReading = HIGH;
int stableButtonState = HIGH;

/* 提醒标志（不用状态机，仅布尔） */
bool alertActive = false;

/* 仅在内存里保存“上次喂食时间”（断电会丢） */
int lastFeedYear=0, lastFeedMon=0, lastFeedDay=0, lastFeedHour=0, lastFeedMin=0;

/* 轻柔音乐 */
int melody[] = {392, 440, 494, 523, 494, 440, 392};
int noteDurations[] = {300, 300, 300, 400, 300, 300, 500};
int melodyLength = sizeof(melody)/sizeof(melody[0]);

void playGentleMelody() {
  for (int i=0; i<melodyLength; i++) {
    tone(BUZZER_PIN, melody[i], noteDurations[i]);
    delay(noteDurations[i] + 30);
  }
  noTone(BUZZER_PIN);
}

void oledShowCentered(const String& l1, const String& l2="", const String& l3="") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  int16_t x1,y1; uint16_t w,h;
  if (l1.length()) { display.getTextBounds(l1,0,0,&x1,&y1,&w,&h); display.setCursor((SCREEN_WIDTH-w)/2,10); display.println(l1); }
  if (l2.length()) { display.getTextBounds(l2,0,0,&x1,&y1,&w,&h); display.setCursor((SCREEN_WIDTH-w)/2,28); display.println(l2); }
  if (l3.length()) { display.getTextBounds(l3,0,0,&x1,&y1,&w,&h); display.setCursor((SCREEN_WIDTH-w)/2,46); display.println(l3); }
  display.display();
}

void showClockScreen(const DateTime& now) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  char tbuf[9];
  snprintf(tbuf, sizeof(tbuf), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  display.setCursor(8, 4);
  display.println(tbuf);

  display.setTextSize(1);
  char dbuf[16];
  snprintf(dbuf, sizeof(dbuf), "%04d-%02d-%02d", now.year(), now.month(), now.day());
  display.setCursor(8, 28);
  display.println(dbuf);

  display.setCursor(8, 42);
  display.print("Plan: ");
  for (uint8_t i=0;i<NUM_FEEDS;i++){
    display.print(feedHours[i]); display.print(":");
    if (feedMinutes[i]<10) display.print("0");
    display.print(feedMinutes[i]);
    display.print(triggeredToday[i] ? "(done) " : " ");
  }

  // 显示上次喂食（仅上电后有效）
  display.setCursor(8, 54);
  if (lastFeedYear>0) {
    char lbuf[24];
    snprintf(lbuf, sizeof(lbuf), "Last:%02d:%02d", lastFeedHour, lastFeedMin);
    display.print(lbuf);
  } else {
    display.print("Last: --:--");
  }

  display.display();
}

bool buttonPressed() {
  int reading = digitalRead(BUTTON_PIN);
  if (BUTTON_ACTIVE_LOW) reading = (reading==LOW) ? HIGH : LOW;

  if (reading != lastButtonReading) lastDebounceTime = millis();
  if (millis()-lastDebounceTime > debounceDelay) {
    if (reading != stableButtonState) {
      stableButtonState = reading;
      if (stableButtonState == HIGH) { lastButtonReading = reading; return true; }
    }
  }
  lastButtonReading = reading;
  return false;
}

void triggerFeedingAlert() {
  oledShowCentered("小乌龟饿啦","该喂食啦～","喂完按复位按钮");
  playGentleMelody();
  alertActive = true;
}

void completeFeedingAndRecord(const DateTime& now) {
  lastFeedYear = now.year();
  lastFeedMon  = now.month();
  lastFeedDay  = now.day();
  lastFeedHour = now.hour();
  lastFeedMin  = now.minute();

  char l2[24];
  snprintf(l2, sizeof(l2), "%02d:%02d 喂食完成", now.hour(), now.minute());
  oledShowCentered("谢谢！", l2, "祝小龟健康成长");
  delay(1200);
  alertActive = false;
}

void checkPlanAndMaybeTrigger(const DateTime& now) {
  if (lastDay != now.day()) {
    for (uint8_t i=0;i<NUM_FEEDS;i++) triggeredToday[i] = false;
    lastDay = now.day();
  }
  if (alertActive) return;

  for (uint8_t i=0;i<NUM_FEEDS;i++) {
    if (!triggeredToday[i] &&
        now.hour()==feedHours[i] &&
        now.minute()==feedMinutes[i] &&
        now.second()==0) {
      triggeredToday[i] = true;
      triggerFeedingAlert();
      break;
    }
  }
}

unsigned long lastScreenUpdate = 0;

void setup() {
  if (BUTTON_USE_PULLUP) {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    lastButtonReading = HIGH; stableButtonState = HIGH;
  } else {
    pinMode(BUTTON_PIN, INPUT);
    lastButtonReading = LOW;  stableButtonState = LOW;
  }

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;); // OLED init 失败则停住
  }
  display.clearDisplay(); display.display();

  // RTC
  if (!rtc.begin()) { oledShowCentered("RTC未检测到","请检查连线"); for(;;); }
  if (rtc.lostPower()) { rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); }

  // 启动画面
  oledShowCentered("系统启动","计划:09:00/17:00","祝小龟健康");
  delay(1000);

  DateTime now = rtc.now();
  lastDay = now.day();
}

void loop() {
  DateTime now = rtc.now();

  if (!alertActive && millis()-lastScreenUpdate>250) {
    showClockScreen(now);
    lastScreenUpdate = millis();
  }

  checkPlanAndMaybeTrigger(now);

  if (alertActive && buttonPressed()) {
    completeFeedingAndRecord(now);
  }
}

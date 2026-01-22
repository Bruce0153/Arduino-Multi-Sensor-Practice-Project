# Arduino-Multi-Sensor-Practice-Project
本项目是一个基于 Arduino 的 智能龟缸监测与提醒系统，已完成实际硬件搭建与调试，用于实时监测乌龟饲养环境，并通过屏幕、灯光和声音进行直观提醒。  系统聚焦养龟过程中最核心、最实用的三个需求：
🌡️ 水温监测与异常提醒 
💧 水质（浊度）检测与换水提醒 
⏰ 定时喂食提醒 
适用于 Arduino 初学者 / 创客实践 / 中小学科创项目 / 实物展示。

This is a **simple Arduino-based practice project** built for learning and demonstration purposes.

The project integrates several basic sensors and output devices to form a small monitoring system.  
It is **not intended to be a practical or commercial product**, but rather a hands-on exercise to demonstrate:

- basic hardware wiring
- sensor reading
- simple logic control
- modular Arduino programming

---

## Project Purpose

The main purpose of uploading this project is:

- to document a **complete physical build**
- to show **hands-on ability** with Arduino hardware
- to demonstrate the process of testing, integrating, and debugging multiple modules

The system itself is intentionally simple and limited in functionality.

---

## Overview

The project consists of several independent modules:

- Temperature sensing
- Light / turbidity-related detection
- Timed reminder logic
- OLED display output

Each module was first tested separately, and then combined into simple integrated versions.

---

## Project Structure

Project
│
├─ test/                         # 功能测试代码
│  ├─ 1.1_temp/                  # 水温检测测试
│  │  └─ 1.1_temp.ino
│  │
│  ├─ 1.2_light/                 # 水质 / 光学检测测试
│  │  └─ 1.2_light.ino
│  │
│  ├─ 1.3_food/                  # 喂食提醒模块测试
│  │  └─ 1.3_food.ino
│  │
│  ├─ test_oled/                 # OLED 基础测试
│  │  └─ test_oled.ino
│  │
│  ├─ test_oled_front/           # OLED 前端显示测试
│  │  └─ test_oled_front.ino
│  │
│  └─ test_temp/                 # 温度模块单独测试
│     └─ test_temp.ino
│
├─ 1.mix                         # 功能整合程序（版本 1）
├─ 2.mix                         # 功能整合程序（版本 2）
├─ 3.mix                         # 功能整合程序（最终版）
│
├─ 温度.txt                      # 水温记录数据示例
├─ 浊度.txt                      # 水质（浊度）记录数据示例
│
└─ README.md

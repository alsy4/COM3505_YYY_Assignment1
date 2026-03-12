#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// 调试开关
extern bool startupDBG;
extern bool netDBG;

// 全局变量
extern unsigned long loopIteration;
extern String MAC_ADDRESS;

// 函数声明
#pragma once

// Windows头文件
#define WIN32_LEAN_AND_MEAN     // 减少Windows头文件体积
#include <windows.h>
#include <windowsx.h>

// GDI+头文件
#include <objidl.h>
#include <gdiplus.h>
#include <graphics.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

// 标准库
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <iostream>
#include <conio.h>

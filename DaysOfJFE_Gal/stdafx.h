#pragma once

// Windowsͷ�ļ�
#define WIN32_LEAN_AND_MEAN     // ����Windowsͷ�ļ����
#include <windows.h>
#include <windowsx.h>

// GDI+ͷ�ļ�
#include <objidl.h>
#include <gdiplus.h>
#include <graphics.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

// ��׼��
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <iostream>
#include <conio.h>

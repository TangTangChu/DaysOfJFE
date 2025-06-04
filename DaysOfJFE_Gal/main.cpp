#include "stdafx.h"
#include "WindowManager.h"
#include "WindowPanel.h"
#include "HomeWindow.h"
#include "GameWindow.h"
#include "LoadingWindow.h"
#pragma comment(lib, "gdiplus.lib")

int main() {
	initgraph(1600, 900, SHOWCONSOLE);
	setbkcolor(WHITE);
	cleardevice();
	BeginBatchDraw();
	settextcolor(BLACK);
	std::wcout.imbue(std::locale("zh_CN"));
	HDC hdc = GetImageHDC(NULL);
	WindowManager wm = WindowManager(hdc);

	auto hw = std::make_shared<HomeWindow>();
	auto gw = std::make_shared<GameWindow>();

	wm.AddWindow(hw);
	wm.AddWindow(gw);
	wm.SwitchWindow(1);
	auto flashimages = std::make_unique<std::vector<std::unique_ptr<Gdiplus::Image>>>();
	flashimages->push_back(std::make_unique<Gdiplus::Image>(L"src/Image/misc/mainflash.png"));
	flashimages->push_back(std::make_unique<Gdiplus::Image>(L"src/Image/misc/attention.png"));

	auto lw = std::make_shared<LoadingWindow>(std::move(flashimages), 1700, 1000);
	lw->SetFinalHook([&wm]() {wm.SwitchWindow(0); });
	wm.SwitchWindow(lw);
	lw->StartAnimation();
	wm.Redraw();
	gw->LoadScript(L"script121.yaml");                                                                        
	gw->LoadScene(1);
	ExMessage ex;
	while (true) {
		if (peekmessage(&ex, EX_MOUSE | EX_KEY)) {
			if (ex.message == WM_MOUSEMOVE) {
				continue;
			}
			wm.DispatchExMessage(ex);
		}
		cleardevice();
		wm.Redraw();
		FlushBatchDraw();
		Sleep(1);
	}

	FlushBatchDraw();
	_getch();
	EndBatchDraw();
	closegraph();
	return 0;
}
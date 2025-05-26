#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "Controls.h"
#include "WindowPanel.h"
#include <gdiplus.h>
#include "IGlobalEvent.h"

class WindowManager {
private:
	std::vector<std::shared_ptr<WindowPanel>> windows;
	std::shared_ptr<WindowPanel> currentWindow;
	HDC hdc;
	IGlobalEvent m_globalEvent;
	enum Layer { BACKGROUND, MIDGROUND, FOREGROUND };
	bool isShowDialog;
public:
	void AddWindow(std::shared_ptr<WindowPanel> window);

	void SwitchWindow(int index);
	void SwitchWindow(std::shared_ptr<WindowPanel> window);
	std::shared_ptr<WindowPanel> GetWindow(int index);
	void AddBackgroundControl(std::shared_ptr<Controls> ctrl);

	void AddMidgroundControl(std::shared_ptr<Controls> ctrl);

	void AddForegroundControl(std::shared_ptr<Controls> ctrl);

	/// <summary>
	/// 通用接口，默认添加当前活动窗口的中层控件区
	/// </summary>
	/// <param name="ctrl">控件对象</param>
	/// <param name="layer">指定控件层，默认为中间层</param>
	/// <returns></returns>
	bool AddControl(std::shared_ptr<Controls> ctrl, int layer = MIDGROUND);

	void RemoveControl(std::shared_ptr<Controls> ctrl);

	void ClearAllControls();

	// 设置当前窗口的背景，必须使用 Image * = new Image()来创建
	void SetBackground(Image img);

	void ClearBackground();

	void Redraw();
	void DispatchExMessage(const ExMessage& ex);
	void ShowDialog(std::wstring text, std::function<void()> yes, std::function<void()> no);
	void CloseDialog();
	WindowManager(HDC hdc);
};

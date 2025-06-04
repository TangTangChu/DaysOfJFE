#include "stdafx.h"
#include "WindowManager.h"
#include <algorithm>
WindowManager::WindowManager(HDC hdc) :hdc(hdc), m_globalEvent(this) {
}
void WindowManager::AddWindow(std::shared_ptr<WindowPanel> window)
{
	window->SetGlobalEvent(&m_globalEvent);
	windows.push_back(window);
}
std::shared_ptr<WindowPanel> WindowManager::GetWindow(int index) {
	return windows[index];
}
void WindowManager::SwitchWindow(int index)
{
	if (index >= 0 && index < windows.size())
	{
		currentWindow = windows[index];
		cleardevice();
		Redraw();
		FlushBatchDraw();
	}
}
void WindowManager::SwitchWindow(std::shared_ptr<WindowPanel> window) {
	currentWindow = window;
}
void WindowManager::AddBackgroundControl(std::shared_ptr<Controls> ctrl)
{
	if (currentWindow)
		currentWindow->AddBackgroundControl(ctrl);
}

void WindowManager::AddMidgroundControl(std::shared_ptr<Controls> ctrl)
{
	if (currentWindow)
		currentWindow->AddMidgroundControl(ctrl);
}

void WindowManager::AddForegroundControl(std::shared_ptr<Controls> ctrl)
{
	if (currentWindow)
		currentWindow->AddForegroundControl(ctrl);
}

/// <summary>
/// 通用接口，默认添加当前活动窗口的中层控件区
/// </summary>
/// <param name="ctrl">控件对象</param>
/// <param name="layer">指定控件层，默认为中间层</param>
/// <returns></returns>
bool WindowManager::AddControl(std::shared_ptr<Controls> ctrl, int layer)
{
	if (currentWindow)
	{
		if (layer == BACKGROUND)
		{
			currentWindow->AddBackgroundControl(ctrl);
		}
		else if (layer == MIDGROUND)
		{
			currentWindow->AddControl(ctrl);
		}
		else if (layer == FOREGROUND)
		{
			currentWindow->AddForegroundControl(ctrl);
		}
		else
		{
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

void WindowManager::RemoveControl(std::shared_ptr<Controls> ctrl)
{
	if (currentWindow)
		currentWindow->RemoveControl(ctrl);
}

void WindowManager::ClearAllControls()
{
	if (currentWindow)
		currentWindow->ClearAllControls();
}

// 设置当前窗口的背景
void WindowManager::SetBackground(Image img)
{
	if (currentWindow)
		currentWindow->SetBackground(img);
}

void WindowManager::ClearBackground()
{
	if (currentWindow)
		currentWindow->ClearBackground();
}

void WindowManager::Redraw() {
	if (auto bg = currentWindow->GetBackground()) {
		Graphics graphics(hdc);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);


		float alpha = std::clamp(currentWindow->BgAlpha / 255.0f, 0.0f, 1.0f);


		ColorMatrix matrix = {
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, alpha, 0.0f,  
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};


		static ImageAttributes imgAttr;
		imgAttr.SetColorMatrix(&matrix);

		RectF destRect(0, 0, 1600, 900);
		REAL imageWidth = (REAL)currentWindow->background->GetWidth();
		REAL imageHeight = (REAL)currentWindow->background->GetHeight();
		REAL imageRatio = imageWidth / imageHeight;
		REAL destRatio = destRect.Width / destRect.Height;

		RectF srcRect(0, 0, imageWidth, imageHeight);
		if (imageRatio > destRatio) {
			REAL requiredWidth = imageHeight * destRatio;
			srcRect.X = (imageWidth - requiredWidth) / 2;
			srcRect.Width = requiredWidth;
		}
		else {
			REAL requiredHeight = imageWidth / destRatio;
			srcRect.Y = (imageHeight - requiredHeight) / 2;
			srcRect.Height = requiredHeight;
		}


		graphics.DrawImage(
			currentWindow->background.get(),
			destRect,
			srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height,
			UnitPixel,
			&imgAttr
		);
	}
	for (auto& control : currentWindow->GetAllControls()) {
		if (control->IsVisible()) {
			control->Draw(hdc);
		}
	}
}

void WindowManager::DispatchExMessage(const ExMessage& ex) {
	if (!currentWindow) return;
	currentWindow->HandleExMessage(ex);
}

void WindowManager::ShowDialog(std::wstring text, std::function<void()> yes, std::function<void()> no) {
	if (!isShowDialog) {
	}
}
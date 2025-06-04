#pragma once
#include <memory>
#include <vector>
#include "Controls.h"
#include <gdiplus.h>
#include "IGlobalEvent.h"
#include <graphics.h>
class WindowPanel {
protected:
	IGlobalEvent* m_globalEvent = nullptr;
	std::vector<std::shared_ptr<Controls>> allControls;
	std::function<void()> OnMouseRBUPHook;
	std::function<void(ExMessage)> OnKeyDownHook;
	std::function<void(ExMessage)> OnKeyUpHook;
private:
	std::vector<std::shared_ptr<Controls>> backgroundControls; // 背景层
	std::vector<std::shared_ptr<Controls>> midgroundControls;  // 中层
	std::vector<std::shared_ptr<Controls>> foregroundControls; // 前景层
public:
	Controls* pressedControl = nullptr;
	BYTE BgAlpha = 255;
	std::unique_ptr<Image> background;
	void SetGlobalEvent(IGlobalEvent* globalEvent);
	void AddBackgroundControl(std::shared_ptr<Controls> ctrl);
	void AddMidgroundControl(std::shared_ptr<Controls> ctrl);
	void AddForegroundControl(std::shared_ptr<Controls> ctrl);

	/// <summary>
	/// 通用添加方法，默认添加至中层
	/// </summary>
	/// <param name="ctrl"></param>
	void AddControl(std::shared_ptr<Controls> ctrl);

	bool RemoveBackgroundControl(std::shared_ptr<Controls> ctrl);

	bool RemoveMidgroundControl(std::shared_ptr<Controls> ctrl);

	bool RemoveForegroundControl(std::shared_ptr<Controls> ctrl);

	// 在全部控件列表中查找并移除控件
	bool RemoveControl(std::shared_ptr<Controls> ctrl);

	// 清空所有层
	void ClearAllControls();

	// 获取所有控件（按渲染顺序）
	std::vector<std::shared_ptr<Controls>> GetAllControls() const;
	void SetBackground(const Gdiplus::Image& img);
	void ClearBackground();
	Controls* FindTopControlAt(int x, int y);
	Image* GetBackground() const;
	virtual void HandleExMessage(ExMessage ex);
	/*virtual void HandleControlClickEvent(int x, int y);*/
	virtual void HandleMouseDown(int x, int y);
	virtual void HandleMouseUp(int x, int y);

	/*virtual void HandleControlHoverEvent(int x, int y);*/
};

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
	/// ͨ�ýӿڣ�Ĭ����ӵ�ǰ����ڵ��в�ؼ���
	/// </summary>
	/// <param name="ctrl">�ؼ�����</param>
	/// <param name="layer">ָ���ؼ��㣬Ĭ��Ϊ�м��</param>
	/// <returns></returns>
	bool AddControl(std::shared_ptr<Controls> ctrl, int layer = MIDGROUND);

	void RemoveControl(std::shared_ptr<Controls> ctrl);

	void ClearAllControls();

	// ���õ�ǰ���ڵı���������ʹ�� Image * = new Image()������
	void SetBackground(Image img);

	void ClearBackground();

	void Redraw();
	void DispatchExMessage(const ExMessage& ex);
	void ShowDialog(std::wstring text, std::function<void()> yes, std::function<void()> no);
	void CloseDialog();
	WindowManager(HDC hdc);
};

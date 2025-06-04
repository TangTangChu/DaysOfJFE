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
	std::vector<std::shared_ptr<Controls>> backgroundControls; // ������
	std::vector<std::shared_ptr<Controls>> midgroundControls;  // �в�
	std::vector<std::shared_ptr<Controls>> foregroundControls; // ǰ����
public:
	Controls* pressedControl = nullptr;
	BYTE BgAlpha = 255;
	std::unique_ptr<Image> background;
	void SetGlobalEvent(IGlobalEvent* globalEvent);
	void AddBackgroundControl(std::shared_ptr<Controls> ctrl);
	void AddMidgroundControl(std::shared_ptr<Controls> ctrl);
	void AddForegroundControl(std::shared_ptr<Controls> ctrl);

	/// <summary>
	/// ͨ����ӷ�����Ĭ��������в�
	/// </summary>
	/// <param name="ctrl"></param>
	void AddControl(std::shared_ptr<Controls> ctrl);

	bool RemoveBackgroundControl(std::shared_ptr<Controls> ctrl);

	bool RemoveMidgroundControl(std::shared_ptr<Controls> ctrl);

	bool RemoveForegroundControl(std::shared_ptr<Controls> ctrl);

	// ��ȫ���ؼ��б��в��Ҳ��Ƴ��ؼ�
	bool RemoveControl(std::shared_ptr<Controls> ctrl);

	// ������в�
	void ClearAllControls();

	// ��ȡ���пؼ�������Ⱦ˳��
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

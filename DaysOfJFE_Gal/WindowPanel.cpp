#include "stdafx.h"
#include "WindowPanel.h"

void WindowPanel::SetGlobalEvent(IGlobalEvent* globalEvent)
{
	this->m_globalEvent = globalEvent;
}
void WindowPanel::AddBackgroundControl(std::shared_ptr<Controls> ctrl)
{
	backgroundControls.push_back(ctrl);
	allControls = GetAllControls();
}
void WindowPanel::AddMidgroundControl(std::shared_ptr<Controls> ctrl)
{
	midgroundControls.push_back(ctrl);
	allControls = GetAllControls();
}
void WindowPanel::AddForegroundControl(std::shared_ptr<Controls> ctrl)
{
	foregroundControls.push_back(ctrl);
	allControls = GetAllControls();
}

/// <summary>
/// ͨ����ӷ�����Ĭ��������в�
/// </summary>
/// <param name="ctrl"></param>
void WindowPanel::AddControl(std::shared_ptr<Controls> ctrl)
{
	AddMidgroundControl(ctrl);
	allControls = GetAllControls();
}

bool WindowPanel::RemoveBackgroundControl(std::shared_ptr<Controls> ctrl)
{
	auto it = std::find(backgroundControls.begin(), backgroundControls.end(), ctrl);
	if (it != backgroundControls.end())
	{
		backgroundControls.erase(it);
		allControls = GetAllControls();
		return true;
	}
	allControls = GetAllControls();
	return false;
}

bool WindowPanel::RemoveMidgroundControl(std::shared_ptr<Controls> ctrl)
{
	auto it = std::find(midgroundControls.begin(), midgroundControls.end(), ctrl);
	if (it != midgroundControls.end())
	{
		midgroundControls.erase(it);
		allControls = GetAllControls();
		return true;
	}
	return false;
}

bool WindowPanel::RemoveForegroundControl(std::shared_ptr<Controls> ctrl)
{
	auto it = std::find(foregroundControls.begin(), foregroundControls.end(), ctrl);
	if (it != foregroundControls.end())
	{
		foregroundControls.erase(it);
		allControls = GetAllControls();
		return true;
	}
	return false;
}

// ��ȫ���ؼ��б��в��Ҳ��Ƴ��ؼ�
bool WindowPanel::RemoveControl(std::shared_ptr<Controls> ctrl)
{
	if (!RemoveBackgroundControl(ctrl) &&
		!RemoveMidgroundControl(ctrl) &&
		!RemoveForegroundControl(ctrl))
	{
		allControls = GetAllControls();
		return false;
	}
	allControls = GetAllControls();
	return true;
}

// ������в�
void WindowPanel::ClearAllControls()
{
	backgroundControls.clear();
	midgroundControls.clear();
	foregroundControls.clear();
	allControls = GetAllControls();
}
void WindowPanel::HandleExMessage(ExMessage ex) {
	//std::cout << std::format("MESSAGE:{}|X:{} Y:{}", ex.message, ex.x, ex.y) << std::endl;
	if (ex.message == WM_LBUTTONDOWN)
	{
		HandleMouseDown(ex.x, ex.y);
	}
	else if (ex.message == WM_LBUTTONUP) {
		HandleMouseUp(ex.x, ex.y);
	}
	else if (ex.message == WM_RBUTTONUP) {
		if (OnMouseRBUPHook) {
			OnMouseRBUPHook();
		}
	}
	else if (ex.message == WM_KEYDOWN)
	{
		if (OnKeyDownHook) {
			OnKeyDownHook(ex);
		}
	}
	else if (ex.message == WM_KEYUP) {
		if (OnKeyUpHook) {
			OnKeyUpHook(ex);
		}
	}
}
// ��ȡ���пؼ�������Ⱦ˳��
std::vector<std::shared_ptr<Controls>> WindowPanel::GetAllControls() const
{
	std::vector<std::shared_ptr<Controls>> all;
	all.reserve(backgroundControls.size() + midgroundControls.size() + foregroundControls.size());
	all.insert(all.end(), backgroundControls.begin(), backgroundControls.end());
	all.insert(all.end(), midgroundControls.begin(), midgroundControls.end());
	all.insert(all.end(), foregroundControls.begin(), foregroundControls.end());
	return all;
}
void WindowPanel::SetBackground(const Gdiplus::Image& img) { background.reset(const_cast<Gdiplus::Image&>(img).Clone()); }
void WindowPanel::ClearBackground() { background.reset(); }
Image* WindowPanel::GetBackground() const { return background.get(); }

void WindowPanel::HandleMouseDown(int x, int y) {
	this->pressedControl = FindTopControlAt(x, y);
	if (this->pressedControl != nullptr) {
		this->pressedControl->OnMouseDown(x, y);
	}
}

void WindowPanel::HandleMouseUp(int x, int y) {
	Controls* releasedControl = FindTopControlAt(x, y);
	if (pressedControl != releasedControl && pressedControl) {
		pressedControl->OnMouseReset();
		//std::cout << "Up�¼�ʱ����ǰDown�¼���Ӧ�ؼ�����" << std::endl;
	}
	else if (pressedControl == releasedControl && pressedControl) {
		pressedControl->OnMouseUp(x, y);
		pressedControl->OnMouseReset();
	}
	this->pressedControl = nullptr;
}

Controls* WindowPanel::FindTopControlAt(int x, int y) {
	for (auto it = allControls.rbegin(); it != allControls.rend(); ++it) {
		if ((*it)->IsVisible() && (*it)->IsEnabled() && (*it)->Contains(x, y)) {
			return it->get();
		}
	}
	return nullptr;
}
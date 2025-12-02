#pragma once
#include "app/IGlobalEvent.h"
#include "gfx/Canvas.h"
#include "gfx/IRenderer.h"
#include "platform/PlatformEvent.h"
#include "ui/Controls.h"
#include <functional>
#include <memory>
#include <vector>

class WindowPanel {
  protected:
    IGlobalEvent *m_globalEvent = nullptr;

    std::vector<std::shared_ptr<Controls>> backgroundControls;
    std::vector<std::shared_ptr<Controls>> midgroundControls;
    std::vector<std::shared_ptr<Controls>> foregroundControls;
    mutable bool controlsDirty = true;
    mutable std::vector<std::shared_ptr<Controls>> allControlsCache;

    std::weak_ptr<Controls> pressedControl;

    ImageHandle background;

    std::function<void(int, int)> OnMouseMoveHook;
    std::function<void(int, int)> OnKeyDownHook;
    std::function<void(int, int)> OnKeyUpHook;
    std::function<void()> OnMouseRBUPHook;

    void markDirty() const;
    void rebuildCache() const;
    const std::vector<std::shared_ptr<Controls>> &getAllControlsCached() const;

  public:
    WindowPanel() = default;
    virtual ~WindowPanel() = default;
    virtual bool HasCustomBackground() const { return false; }
    virtual void DrawCustomBackground(Canvas &c, IRenderer &r) {}
    void SetGlobalEvent(IGlobalEvent *globalEvent);

    void AddBackgroundControl(std::shared_ptr<Controls> ctrl);
    void AddMidgroundControl(std::shared_ptr<Controls> ctrl);
    void AddForegroundControl(std::shared_ptr<Controls> ctrl);

    bool RemoveBackgroundControl(std::shared_ptr<Controls> ctrl);
    bool RemoveMidgroundControl(std::shared_ptr<Controls> ctrl);
    bool RemoveForegroundControl(std::shared_ptr<Controls> ctrl);

    bool RemoveControl(const std::shared_ptr<Controls> &ctrl);
    void ClearAllControls();

    std::vector<std::shared_ptr<Controls>> GetAllControls() const;

    void SetBackground(const ImageHandle &img);
    void ClearBackground();
    ImageHandle *GetBackground() const;

    Controls *FindTopControlAt(int x, int y);

    virtual void HandleEvent(const PlatformEvent &e);

    void SetOnMouseMoveHook(std::function<void(int, int)> hook);
    void SetOnKeyDownHook(std::function<void(int, int)> hook);
    void SetOnKeyUpHook(std::function<void(int, int)> hook);
    void SetOnMouseRBUPHook(std::function<void()> hook);

  protected:
    virtual void HandleMouseDown(int x, int y);
    virtual void HandleMouseUp(int x, int y);
};

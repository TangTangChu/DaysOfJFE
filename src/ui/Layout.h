#pragma once
#include "Controls.h"
#include <algorithm>
#include <memory>
#include <vector>


class LayoutControl : public Controls {
  protected:
    std::vector<std::shared_ptr<Controls>> children;
    std::weak_ptr<Controls> pressedChild;
    std::weak_ptr<Controls> hoveredChild;

  public:
    LayoutControl();
    virtual ~LayoutControl() = default;

    void AddChild(std::shared_ptr<Controls> child);
    void RemoveChild(std::shared_ptr<Controls> child);
    void ClearChildren();
    const std::vector<std::shared_ptr<Controls>> &GetChildren() const {
        return children;
    }

    void Draw(IRenderer &r) override;

    void OnMouseDown(int x, int y) override;
    void OnMouseUp(int x, int y) override;
    void OnMouseReset() override;
    void OnMouseEnter(int x, int y) override;
    void OnMouseLeave(int x, int y) override;
    void OnMouseWheel(float deltaX, float deltaY) override;
    void OnKeyDown(int key, int mods) override;
    void OnKeyUp(int key, int mods) override;

    void OnWindowResize(int width, int height) override;

    virtual void UpdateLayout() {}

  protected:
    Controls *FindChildAt(int x, int y);
};

class StackPanel : public LayoutControl {
  public:
    enum class Orientation { Vertical, Horizontal };
    enum class Alignment { Start, Center, End, Stretch };

    StackPanel(Orientation o = Orientation::Vertical);

    void SetOrientation(Orientation o);
    void SetSpacing(int s);
    void SetAlignment(Alignment align);

    void UpdateLayout() override;
    void OnWindowResize(int width, int height) override;

  private:
    Orientation orientation;
    Alignment alignment = Alignment::Start;
    int spacing = 5;
};

#pragma once
#include "gfx/IRenderer.h"
#include "ui/Controls.h"
#include <functional>

class ClickPaper : public Controls {
  public:
    ClickPaper(std::function<void()> onClick) {
        SetOnClick(std::move(onClick));
    }

    void Draw(IRenderer &) override {}
    void OnMouseUp(int x, int y) override;
};

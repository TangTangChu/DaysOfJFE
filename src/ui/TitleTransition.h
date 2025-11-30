#pragma once
#include "gfx/IRenderer.h"
#include "ui/Controls.h"
#include <string>

class TitleTransition : public Controls {
  private:
    std::string text;

  public:
    explicit TitleTransition(const std::string &t = "Title") : text(t) {}

    void SetText(const std::string &t) { text = t; }

    void Draw(IRenderer &r) override;
};

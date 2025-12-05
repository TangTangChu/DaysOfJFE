#pragma once
#include "gfx/IRenderer.h"
#include "gfx/geom/Geom.h"
#include "ui/Controls.h"
#include <functional>
#include <string>

class Button : public Controls {
  protected:
    std::string text;
    Color textColor;
    gfx::CornerRadius radius{20.f, 20.f, 20.f, 20.f};

    Button(const std::string &text, std::function<void()> onClick,
           Color textColor)
        : text(text), textColor(textColor) {
        SetOnClick(std::move(onClick));
        fontStyle = 1;
    }

  public:
    virtual void Draw(IRenderer &r) override = 0;
    void SetText(const std::string &newText) { text = newText; }
    virtual ~Button() = default;
};

class PrimaryButton : public Button {
  private:
    Color btnColor;
    Color hoverBgColor{96, 165, 250, 255};
    Color finaBrush;

  public:
    PrimaryButton(const std::string &text, std::function<void()> onClick,
                  Color textColor = {255, 255, 255, 255},
                  Color backgroundColor = {59, 130, 246, 255})
        : Button(text, std::move(onClick), textColor),
          btnColor(backgroundColor), finaBrush(backgroundColor) {
        SetOnMouseDownHook([this]() { finaBrush = hoverBgColor; });
        SetOnMouseUpPreHook([this]() { finaBrush = btnColor; });
        SetOnMouseResetHook([this]() { finaBrush = btnColor; });
    }

    void setBackgroundColor(Color c) {
        btnColor = c;
        finaBrush = c;
    }
    void setHoverBgColor(Color c) { hoverBgColor = c; }
    void SetCornerRadius(gfx::CornerRadius rad) { radius = rad; }
    void SetCornerRadius(float r) { radius = gfx::CornerRadius{r, r, r, r}; }
    void SetCornerRadius(int r) { SetCornerRadius((float)r); }

    void Draw(IRenderer &r) override;
};

class TextButton : public Button {
  public:
    TextButton(const std::string &text, std::function<void()> onClick,
               Color textColor = {59, 130, 246, 255})
        : Button(text, std::move(onClick), textColor) {}

    void Draw(IRenderer &r) override;
};

#pragma once
#include "Layout.h"
#include "TextBlock.h"

class DialogBox : public LayoutControl {
  public:
    DialogBox();

    void SetName(const std::string &name);
    void SetText(const std::string &text);
    void SetTextImmediate(const std::string &text);
    void ClearText();
    bool IsAnimating() const;
    void SkipAnimation();

    void UpdateLayout() override;
    void Draw(IRenderer &r) override;

    void SetBackgroundColor(Color c) { bgColor = c; }
    void SetBorderColor(Color c) { borderColor = c; }

  private:
    std::shared_ptr<TextBlock> nameBlock;
    std::shared_ptr<TextBlock> msgBlock;

    Color bgColor{0, 0, 0, 180};
    Color borderColor{255, 255, 255, 200};

    int padding = 30;
};

#pragma once
#include "gfx/IRenderer.h"
#include "ui/Controls.h"
#include "utils/Utf8Helper.h"
#include <atomic>
#include <mutex>
#include <string>
#include <thread>

class TextBlock : public Controls {
  private:
    std::string Text;
    std::string ShowText;
    Color TextColor;

    TextHAlign hAlign = TextHAlign::Left;
    TextVAlign vAlign = TextVAlign::Top;

    std::atomic<bool> m_running{false};
    std::thread m_thread;
    std::mutex m_mutex;
    bool m_loopAnimation = false;

  public:
    TextBlock(const std::string &text, int fontsize = 14,
              Color textColor = {255, 255, 255, 255})
        : Text(text), ShowText(text), TextColor(textColor) {
        this->fontSize = fontsize;
    }

    ~TextBlock() override { stopAnimation(); }

    void Draw(IRenderer &r) override;

    void SetAlign(TextHAlign h, TextVAlign v) {
        hAlign = h;
        vAlign = v;
    }

    void SetText(const std::string &newText);
    bool IsAnimating() const;

    void TextAnimation();
    void startAnimation(bool loop = false);
    void stopAnimation();
    void showFullText();
    void clearText();
};

#pragma once
#include "gfx/IRenderer.h"
#include "ui/Controls.h"
#include "utils/Utf8Helper.h"
#include <atomic>
#include <mutex>
#include <string>
#include <thread>

class OutlineText : public Controls {
  private:
    int outlineWidth = 1;

    std::string text;
    std::string showtext;

    Color outlineColor;
    Color textColor;

    std::atomic<bool> m_running{false};
    std::thread m_thread;
    std::mutex m_mutex;
    bool m_loopAnimation = false;

  public:
    OutlineText(const std::string &text, int fontsize = 14, int thick = 1,
                Color outlineColor = {255, 25, 25, 25},
                Color textColor = {255, 255, 255, 255})
        : outlineWidth(thick), text(text), showtext(text),
          outlineColor(outlineColor), textColor(textColor) {
        this->fontSize = fontsize;
    }

    ~OutlineText() override { stopAnimation(); }

    void SetText(const std::string &newText);
    bool IsAnimating() const;

    void Draw(IRenderer &r) override;

    void TextAnimation();
    void startAnimation(bool loop = false);
    void stopAnimation();
    void showFullText();
    void clearText();
};

#include "ui/TextBlock.h"
#include <chrono>

void TextBlock::SetText(const std::string &newText) {
    std::lock_guard<std::mutex> lock(m_mutex);
    Text = newText;
    if (!m_running)
        ShowText = newText;
}

bool TextBlock::IsAnimating() const { return m_running; }

void TextBlock::Draw(IRenderer &r) {
    if (!IsVisible())
        return;

    std::string current;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        current = ShowText;
    }

    float anchorX = (float)GetX();
    float anchorY = (float)GetY();
    float w = (float)GetWidth();
    float h = (float)GetHeight();

    if (hAlign == TextHAlign::Center)
        anchorX = (float)GetX() + w * 0.5f;
    else if (hAlign == TextHAlign::Right)
        anchorX = (float)GetX() + w;

    if (vAlign == TextVAlign::Middle)
        anchorY = (float)GetY() + h * 0.5f;
    else if (vAlign == TextVAlign::Bottom)
        anchorY = (float)GetY() + h;

    r.drawText(current, anchorX, anchorY, (float)fontSize, TextColor, hAlign,
               vAlign);
}

void TextBlock::TextAnimation() {
    auto start = std::chrono::steady_clock::now();
    size_t charIndex = 0;
    size_t len = Utf8Length(Text);

    while (m_running && charIndex <= len) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - start)
                .count();

        if (elapsed >= (15 * (int)charIndex)) {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                ShowText = Utf8Prefix(Text, charIndex);
            }
            charIndex++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (m_running && m_loopAnimation) {
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        start = std::chrono::steady_clock::now();
        charIndex = 0;
    }
}

void TextBlock::startAnimation(bool loop) {
    if (m_running)
        stopAnimation();
    m_loopAnimation = loop;
    m_running = true;
    m_thread = std::thread(&TextBlock::TextAnimation, this);
}

void TextBlock::stopAnimation() {
    m_running = false;
    if (m_thread.joinable())
        m_thread.join();
}

void TextBlock::showFullText() {
    stopAnimation();
    std::lock_guard<std::mutex> lock(m_mutex);
    ShowText = Text;
}

void TextBlock::clearText() {
    stopAnimation();
    std::lock_guard<std::mutex> lock(m_mutex);
    ShowText.clear();
}

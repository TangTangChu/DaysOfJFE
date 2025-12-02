#include "ui/OutlineText.h"
#include "gfx/Canvas.h"
#include <chrono>


void OutlineText::SetText(const std::string &newText) {
    std::lock_guard<std::mutex> lock(m_mutex);
    text = newText;
    if (!m_running)
        showtext = newText;
}

bool OutlineText::IsAnimating() const { return m_running; }

void OutlineText::Draw(IRenderer &r) {
    if (!IsVisible())
        return;

    std::string current;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        current = showtext;
    }

    Canvas c(r);
    TextStyle ts;
    ts.sizePx = (float)fontSize;
    ts.color = textColor;
    ts.stroke = true;
    ts.strokeColor = outlineColor;
    ts.strokeWidth = (float)outlineWidth;
    c.text(current, gfx::Vec2{bounds.x, bounds.y}, ts, AlignH::Left,
           AlignV::Top);
}

void OutlineText::TextAnimation() {
    while (m_running) {
        size_t len = Utf8Length(text);
        for (size_t i = 0; i <= len; i++) {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                showtext = Utf8Prefix(text, i);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (!m_running)
                break;
        }

        if (!m_loopAnimation)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
    }

    if (!m_running) {
        std::lock_guard<std::mutex> lock(m_mutex);
        showtext = text;
    }
}

void OutlineText::startAnimation(bool loop) {
    if (m_running)
        stopAnimation();
    m_loopAnimation = loop;
    m_running = true;
    m_thread = std::thread(&OutlineText::TextAnimation, this);
}

void OutlineText::stopAnimation() {
    m_running = false;
    if (m_thread.joinable())
        m_thread.join();
}

void OutlineText::showFullText() {
    stopAnimation();
    std::lock_guard<std::mutex> lock(m_mutex);
    showtext = text;
}

void OutlineText::clearText() {
    stopAnimation();
    std::lock_guard<std::mutex> lock(m_mutex);
    showtext.clear();
}

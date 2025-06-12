#include "stdafx.h"
#include "TextBlock.h"

void TextBlock::SetText(const std::wstring& newText)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	Text = newText;
	if (!m_running)
	{
		ShowText = newText;
	}
}
bool TextBlock::IsAnimating() const
{
	return m_running;
}
void TextBlock::Draw(HDC hdc)
{
	try {
		if (!IsVisible())
			return;
		Graphics graphics(hdc);
		graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
		graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
		Font font(&fontFamily, REAL(fontSize), fontStyle);
		std::wstring currentText;
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			currentText = ShowText;
		}
		SolidBrush solidbrush(TextColor);
		RectF layoutRect((REAL)X, (REAL)Y, (REAL)Width, (REAL)Height);

		graphics.DrawString(currentText.c_str(), -1, &font, layoutRect, format ? format.get() : nullptr, &solidbrush);
	}
	catch (...) {
	}
}

void TextBlock::TextAnimation()
{
	auto start = std::chrono::steady_clock::now();
	int charIndex = 0;

	while (m_running && charIndex <= Text.length())
	{
		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

		if (elapsed >= (15 * charIndex))
		{
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				ShowText = Text.substr(0, charIndex);
			}
			charIndex++;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void TextBlock::startAnimation(bool loop)
{
	if (m_running)
		stopAnimation();
	m_loopAnimation = loop;
	m_running = true;
	m_thread = std::thread(&TextBlock::TextAnimation, this);
}

void TextBlock::stopAnimation()
{
	m_running = false;
	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

void TextBlock::showFullText()
{
	stopAnimation();
	std::lock_guard<std::mutex> lock(m_mutex);
	ShowText = Text;
}

void TextBlock::clearText()
{
	stopAnimation();
	std::lock_guard<std::mutex> lock(m_mutex);
	ShowText = L"";
}
void TextBlock::SetStringFormat(std::unique_ptr<StringFormat> newFormat) {
	format = std::move(newFormat);
}
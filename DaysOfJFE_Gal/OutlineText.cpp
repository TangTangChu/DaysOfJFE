#include "stdafx.h"
#include "OutlineText.h"

void OutlineText::SetText(const std::wstring& newText)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	text = newText;
	if (!m_running)
	{
		showtext = newText;
	}
}
bool OutlineText::IsAnimating() const
{
	return m_running;
}
void OutlineText::Draw(HDC hdc)
{
	if (!IsVisible())
		return;
	Graphics graphics(hdc);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

	FontFamily fontFamily(L"Î¢ÈíÑÅºÚ");
	GraphicsPath path;
	StringFormat format;

	std::wstring currentText;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		currentText = showtext;
	}

	format.SetFormatFlags(StringFormatFlagsLineLimit);
	path.AddString(
		currentText.c_str(),
		-1,
		&fontFamily,
		FontStyleRegular,
		(REAL)fontSize,
		RectF(REAL(X), REAL(Y), REAL(Width), REAL(Height)),
		&format);

	Pen outlinePen(outlineColor, (REAL)outlineWidth);
	outlinePen.SetLineJoin(LineJoinRound);
	graphics.DrawPath(&outlinePen, &path);

	SolidBrush textBrush(textColor);
	graphics.FillPath(&textBrush, &path);
}

void OutlineText::TextAnimation()
{
	while (m_running)
	{
		for (int i = 0; i <= text.length(); i++)
		{
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				showtext = text.substr(0, i);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (!m_running)
				break;
		}

		if (!m_loopAnimation)
			break;
		std::this_thread::sleep_for(std::chrono::milliseconds(600));
	}

	if (!m_running)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		showtext = text;
	}
}

void OutlineText::startAnimation(bool loop)
{
	if (m_running)
		stopAnimation();
	m_loopAnimation = loop;
	m_running = true;
	m_thread = std::thread(&OutlineText::TextAnimation, this);
}

void OutlineText::stopAnimation()
{
	m_running = false;
	if (m_thread.joinable())
	{
		m_thread.join();
	}
}

void OutlineText::showFullText()
{
	stopAnimation();
	std::lock_guard<std::mutex> lock(m_mutex);
	showtext = text;
}

void OutlineText::clearText()
{
	stopAnimation();
	std::lock_guard<std::mutex> lock(m_mutex);
	showtext = L"";
}
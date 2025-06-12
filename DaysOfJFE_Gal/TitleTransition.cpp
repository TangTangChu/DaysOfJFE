#include "stdafx.h"
#include "TitleTransition.h"

TitleTransition::TitleTransition(std::wstring text) :Text(text) {
}

void TitleTransition::Draw(HDC hdc) {
	Graphics graphics(hdc);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
	graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	SolidBrush brush(Color(255, 0, 0, 0));
	SolidBrush brush2(Color(255, 255, 255, 255));
	graphics.FillRectangle(&brush, Rect{ 0,0,1600,900 });
	StringFormat sf;
	sf.SetAlignment(StringAlignmentCenter);
	sf.SetLineAlignment(StringAlignmentCenter);
	Font font(L"ÐÂËÎÌå", 68, 1);
	graphics.DrawString(Text.c_str(), -1, &font, RectF{ 0,0,1600,900 }, &sf, &brush2);
}

void TitleTransition::SetText(std::wstring text) {
	this->Text = text;
}
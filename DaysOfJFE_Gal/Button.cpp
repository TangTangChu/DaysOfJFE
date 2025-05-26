#include "stdafx.h"
#include "Button.h"

void Button::SetText(const std::wstring& newText) { text = newText; }

void PrimaryButton::setBackgroundColor(Color color)
{
	btnColor = color;
}

void PrimaryButton::Draw(HDC hdc)
{
	if (!IsVisible())
		return;
	Graphics graphics(hdc);

	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
	graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

	GraphicsPath path;
	path.AddArc(rect.X, rect.Y, radius.A, radius.A, 180, 90);
	path.AddArc(rect.GetRight() - radius.B, rect.Y, radius.B, radius.B, 270, 90);
	path.AddArc(rect.GetRight() - radius.C, rect.GetBottom() - radius.C, radius.C, radius.C, 0, 90);
	path.AddArc(rect.X, rect.GetBottom() - radius.D, radius.D, radius.D, 90, 90);
	path.CloseFigure();

	SolidBrush brush(FinaBrush);
	graphics.FillPath(&brush, &path);
	graphics.SetTextContrast(170);
	SolidBrush textBrush(Color(255, 255, 255, 255));
	Font font(L"Î¢ÈíÑÅºÚ", (REAL)fontSize, fontStyle);
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	RectF layoutRect((REAL)X, (REAL)Y, (REAL)Width, (REAL)Height);
	graphics.DrawString(
		text.c_str(),
		-1,
		&font,
		layoutRect,
		&format,
		&textBrush);
}

void PrimaryButton::setHoverBgColor(Color color) {
	hoverBgColor = color;
}
void PrimaryButton::SetCornerRadius(CornerRadius rad) {
	this->radius = rad;
}
void PrimaryButton::SetCornerRadius(int r) {
	this->radius = CornerRadius{ r,r,r,r };
}
void TextButton::Draw(HDC hdc)
{
	Graphics graphics(hdc);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	SolidBrush textBrush(textColor);
	Font font(L"Î¢ÈíÑÅºÚ", (REAL)fontSize, fontStyle);
	StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	format.SetLineAlignment(StringAlignmentCenter);
	graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
	RectF layoutRect((REAL)X, (REAL)Y, (REAL)Width, (REAL)Height);
	graphics.DrawString(
		text.c_str(),
		-1,
		&font,
		layoutRect,
		&format,
		&textBrush);
}
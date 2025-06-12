#include "stdafx.h"
#include "ImageContainer.h"

void ImageContainer::SetAlpha(BYTE alphaValue)
{
	this->alpha = alphaValue;
	// 当alpha改变时更新矩阵
	matrix = {
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, alpha / 255.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};
	imgAttr.SetColorMatrix(&matrix);
}

void ImageContainer::SetImage(std::unique_ptr<Image> img)
{
	image = std::move(img);
	ratioCached = false;
}

BYTE ImageContainer::GetAlpha() {
	return this->alpha;
}

void ImageContainer::SetMode(int mode)
{
	this->Mode = mode;
}

void ImageContainer::SetRectF(RectF rectF)
{
	this->rectp = rectF;
}

void ImageContainer::Draw(HDC hdc)
{
	if (!IsVisible() || !image)
		return;

	Graphics graphics(hdc);
	graphics.SetClip(this->rectp);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);

	if (!ratioCached && image) {
		cachedImageRatio = static_cast<REAL>(image->GetWidth()) / image->GetHeight();
		ratioCached = true;
	}

	REAL destRatio = rectp.Width / rectp.Height;

	switch (this->Mode) {
	case 1: // 原始模式
		graphics.DrawImage(
			image.get(),
			Rect(static_cast<int>(rectp.X), static_cast<int>(rectp.Y),
				static_cast<int>(rectp.Width), static_cast<int>(rectp.Height)),
			0, 0, image->GetWidth(), image->GetHeight(),
			UnitPixel,
			&imgAttr);
		break;

	case 2: // 保持比例，调整目标矩形
	{
		RectF adjustedRect = rectp;
		if (cachedImageRatio > destRatio) {
			adjustedRect.Height = rectp.Width / cachedImageRatio;
			adjustedRect.Y += (rectp.Height - adjustedRect.Height) / 2;
		}
		else {
			adjustedRect.Width = rectp.Height * cachedImageRatio;
			adjustedRect.X += (rectp.Width - adjustedRect.Width) / 2;
		}

		graphics.DrawImage(
			image.get(),
			adjustedRect,
			0, 0, image->GetWidth(), image->GetHeight(),
			UnitPixel,
			&imgAttr);
	}
	break;

	case 3: // 保持比例，调整源矩形
	{
		RectF srcRect(0, 0, static_cast<REAL>(image->GetWidth()), static_cast<REAL>(image->GetHeight()));
		if (cachedImageRatio > destRatio) {
			REAL requiredHeight = image->GetWidth() / destRatio;
			srcRect.Y = (image->GetHeight() - requiredHeight) / 2;
			srcRect.Height = requiredHeight;
		}
		else {
			REAL requiredWidth = image->GetHeight() * destRatio;
			srcRect.X = (image->GetWidth() - requiredWidth) / 2;
			srcRect.Width = requiredWidth;
		}

		graphics.DrawImage(
			image.get(),
			rectp,
			srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height,
			UnitPixel,
			&imgAttr);
	}
	break;
	}
}
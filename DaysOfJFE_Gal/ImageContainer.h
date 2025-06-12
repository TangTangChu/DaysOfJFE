#pragma once
#include "Controls.h"
#include <memory>
#include <string>
#include <graphics.h>
#include <gdiplus.h>
using namespace Gdiplus;

class ImageContainer : public Controls {
private:
	std::unique_ptr<Image> image;
	int Mode = 1;
	RectF rectp;
	BYTE alpha = 255;

	mutable REAL cachedImageRatio = 0.0f;
	mutable bool ratioCached = false;

	mutable ImageAttributes imgAttr;
	mutable ColorMatrix matrix;

public:
	ImageContainer() = default;

	void SetAlpha(BYTE alphaValue);
	BYTE GetAlpha();
	void SetImage(std::unique_ptr<Image> img);

	void SetMode(int mode);
	void SetRectF(RectF rectF);

	void Draw(HDC hdc);
};

#include "ui/ImageContainer.h"
#include "gfx/Canvas.h"
#include <algorithm>

void ImageContainer::SetAlpha(unsigned char alphaValue) { alpha = alphaValue; }

unsigned char ImageContainer::GetAlpha() const { return alpha; }

void ImageContainer::SetImage(const ImageHandle &img) {
    image = img;
    ratioCached = false;
}

void ImageContainer::ClearImage() {
    image.reset();
    ratioCached = false;
}

void ImageContainer::SetMode(int mode) { Mode = mode; }

void ImageContainer::Draw(IRenderer &r) {
    if (!IsVisible() || !image.has_value() || !(*image))
        return;

    const ImageHandle &img = *image;
    if (img.width <= 0 || img.height <= 0 || bounds.w <= 0 || bounds.h <= 0)
        return;

    if (!ratioCached) {
        cachedImageRatio = (float)img.width / (float)img.height;
        ratioCached = true;
    }

    Canvas c(r);
    float destRatio = bounds.w / bounds.h;
    float a = std::clamp(alpha / 255.0f, 0.0f, 1.0f);

    gfx::Rect src = gfx::Rect::XYWH(0, 0, (float)img.width, (float)img.height);
    gfx::Rect dst = bounds;

    switch (Mode) {
    case 1: // Stretch
        c.image(img, src, dst, a);
        break;

    case 2: { // Contain（等比完整显示）
        gfx::Rect adjusted = bounds;
        if (cachedImageRatio > destRatio) {
            adjusted.h = bounds.w / cachedImageRatio;
            adjusted.y += (bounds.h - adjusted.h) * 0.5f;
        } else {
            adjusted.w = bounds.h * cachedImageRatio;
            adjusted.x += (bounds.w - adjusted.w) * 0.5f;
        }
        c.image(img, src, adjusted, a);
    } break;

    case 3: { // Cover（等比裁切铺满）
        gfx::Rect cropped = src;
        if (cachedImageRatio > destRatio) {
            float requiredHeight = (float)img.width / destRatio;
            cropped.y = ((float)img.height - requiredHeight) * 0.5f;
            cropped.h = requiredHeight;
        } else {
            float requiredWidth = (float)img.height * destRatio;
            cropped.x = ((float)img.width - requiredWidth) * 0.5f;
            cropped.w = requiredWidth;
        }
        c.image(img, cropped, bounds, a);
    } break;

    default:
        c.image(img, src, dst, a);
        break;
    }
}

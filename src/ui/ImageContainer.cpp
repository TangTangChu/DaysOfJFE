#include "ui/ImageContainer.h"
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

void ImageContainer::SetRectF(RectF rectF) { rectp = rectF; }

void ImageContainer::Draw(IRenderer &r) {
    if (!IsVisible() || !image.has_value() || !(*image))
        return;

    const ImageHandle &img = *image;

    if (img.width <= 0 || img.height <= 0 || rectp.Width <= 0 ||
        rectp.Height <= 0)
        return;

    if (!ratioCached) {
        cachedImageRatio = (float)img.width / (float)img.height;
        ratioCached = true;
    }

    float destRatio = rectp.Width / rectp.Height;
    float a = std::clamp(alpha / 255.0f, 0.0f, 1.0f);

    switch (Mode) {
    case 1:
        r.drawImage(img, 0, 0, (float)img.width, (float)img.height, rectp.X,
                    rectp.Y, rectp.Width, rectp.Height, a);
        break;

    case 2: {
        RectF adjusted = rectp;

        if (cachedImageRatio > destRatio) {
            adjusted.Height = rectp.Width / cachedImageRatio;
            adjusted.Y += (rectp.Height - adjusted.Height) / 2.0f;
        } else {
            adjusted.Width = rectp.Height * cachedImageRatio;
            adjusted.X += (rectp.Width - adjusted.Width) / 2.0f;
        }

        r.drawImage(img, 0, 0, (float)img.width, (float)img.height, adjusted.X,
                    adjusted.Y, adjusted.Width, adjusted.Height, a);
    } break;

    case 3: {
        float sx = 0, sy = 0, sw = (float)img.width, sh = (float)img.height;

        if (cachedImageRatio > destRatio) {
            float requiredHeight = (float)img.width / destRatio;
            sy = ((float)img.height - requiredHeight) / 2.0f;
            sh = requiredHeight;
        } else {
            float requiredWidth = (float)img.height * destRatio;
            sx = ((float)img.width - requiredWidth) / 2.0f;
            sw = requiredWidth;
        }

        r.drawImage(img, sx, sy, sw, sh, rectp.X, rectp.Y, rectp.Width,
                    rectp.Height, a);
    } break;

    default:
        r.drawImage(img, 0, 0, (float)img.width, (float)img.height, rectp.X,
                    rectp.Y, rectp.Width, rectp.Height, a);
        break;
    }
}

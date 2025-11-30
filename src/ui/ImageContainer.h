#pragma once
#include "gfx/IRenderer.h"
#include "ui/Controls.h"
#include <optional>

struct RectF {
    float X = 0, Y = 0, Width = 0, Height = 0;
};

class ImageContainer : public Controls {
  private:
    std::optional<ImageHandle> image;
    int Mode = 1;
    RectF rectp;
    unsigned char alpha = 255;

    mutable float cachedImageRatio = 0.0f;
    mutable bool ratioCached = false;

  public:
    ImageContainer() = default;

    void SetAlpha(unsigned char alphaValue);
    unsigned char GetAlpha() const;

    void SetImage(const ImageHandle &img);
    void ClearImage();

    void SetMode(int mode);
    void SetRectF(RectF rectF);

    void Draw(IRenderer &r) override;
};

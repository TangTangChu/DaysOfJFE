#pragma once
#include "gfx/IRenderer.h"
#include "ui/Controls.h"
#include <optional>

class ImageContainer : public Controls {
  private:
    std::optional<ImageHandle> image;
    int Mode = 1;
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

    void SetBounds(const gfx::Rect &r) {
        Controls::SetBounds(r);
        ratioCached = false;
    }

    void Draw(IRenderer &r) override;
};

#pragma once
#include "Controls.h"
#include "include/core/SkImage.h"
#include "include/core/SkBitmap.h"
#include <memory>
#include <string>

class ImageContainer : public Controls {
private:
    sk_sp<SkImage> image;
    std::unique_ptr<SkBitmap> bitmap;  // 保持 bitmap 生命周期
    int mode = 1;  // 1=原始大小, 2=适应容器, 3=裁剪填充
    SkRect rectF;
    uint8_t alpha = 255;
    
    // 缓存优化
    mutable float cachedImageRatio = 0.0f;
    mutable float cachedDestRatio = 0.0f;
    mutable bool ratioCached = false;

public:
    ImageContainer() = default;

    void SetAlpha(uint8_t alphaValue);
    uint8_t GetAlpha() const;
    void SetImage(sk_sp<SkImage> img);
    void SetImageFromFile(const std::string& path);
    
    void SetMode(int mode);
    void SetRectF(const SkRect& rect);

    void Draw(SkCanvas* canvas) override;
};


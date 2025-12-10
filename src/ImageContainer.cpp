/**
 * @file ImageContainer.cpp
 * @brief 图片容器 - 用于显示背景、立绘、UI图片
 * 
 * 支持功能:
 * - 多种显示模式: 居中/拉伸/适应/原始尺寸
 * - 透明度控制
 * - 图片缓存和懒加载
 * - 使用SkCodec解码图片
 */
#include "ImageContainer.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkBitmap.h"
#include "include/codec/SkCodec.h"
#include <iostream>

void ImageContainer::SetAlpha(uint8_t alphaValue) {
    alpha = alphaValue;
}

uint8_t ImageContainer::GetAlpha() const {
    return alpha;
}

void ImageContainer::SetImage(sk_sp<SkImage> img) {
    image = std::move(img);
    ratioCached = false;  // 清除缓存
}

void ImageContainer::SetImageFromFile(const std::string& path) {
    std::cout << "开始加载图片: " << path << std::endl;
    
    sk_sp<SkData> data = SkData::MakeFromFileName(path.c_str());
    if (!data) {
        std::cerr << "数据读取失败: " << path << std::endl;
        return;
    }
    std::cout << "数据读取成功, 大小: " << data->size() << " 字节" << std::endl;
    
    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(data);
    if (!codec) {
        std::cerr << "Codec 创建失败: " << path << std::endl;
        return;
    }
    
    SkImageInfo info = codec->getInfo();
    std::cout << "图片信息: " << info.width() << " x " << info.height() << std::endl;
    
    // 创建新的 bitmap 并保存到成员变量
    bitmap = std::make_unique<SkBitmap>();
    bitmap->allocPixels(info);
    std::cout << "Bitmap 分配成功" << std::endl;
    
    if (codec->getPixels(info, bitmap->getPixels(), bitmap->rowBytes()) != SkCodec::kSuccess) {
        std::cerr << "像素读取失败" << std::endl;
        bitmap.reset();
        return;
    }
    std::cout << "像素读取成功" << std::endl;
    
    // 关键：设置 bitmap 为不可变，确保 pixelRef 有效
    bitmap->setImmutable();
    std::cout << "Bitmap 设置为不可变" << std::endl;
    
    // 从 bitmap 创建 image - 使用 makeShader 的方式创建独立副本
    // asImage() 可能导致 pixelRef 失效，改用深拷贝
    auto pixmap = bitmap->pixmap();
    image = SkImages::RasterFromPixmapCopy(pixmap);
    std::cout << "RasterFromPixmapCopy 返回, image 有效=" << (image != nullptr) << std::endl;
    
    if (!image) {
        std::cerr << "bitmap.asImage() 失败" << std::endl;
        bitmap.reset();
        return;
    }
    
    std::cout << "图片加载成功: " << path << " size=" << image->width() << "x" << image->height() << std::endl;
    ratioCached = false;  // 清除缓存
}


void ImageContainer::SetMode(int m) {
    mode = m;
}

void ImageContainer::SetRectF(const SkRect& rect) {
    rectF = rect;
    Width = rect.width();
    Height = rect.height();
    X = rect.x();
    Y = rect.y();
}

void ImageContainer::Draw(SkCanvas* canvas) {
    if (!IsVisible() || !image) {
        return;
    }

    SkPaint paint;
    paint.setAlpha(alpha);
    paint.setAntiAlias(true);

    if (mode == 1) {
        // 拉伸到指定尺寸
        SkRect dest = SkRect::MakeXYWH(X, Y, Width, Height);
        canvas->drawImageRect(image, dest, SkSamplingOptions(), &paint);
    }
    else if (mode == 2) {
        // 适应容器 - 保持比例
        SkRect dest = SkRect::MakeXYWH(X, Y, Width, Height);
        
        // 使用缓存的比例
        if (!ratioCached) {
            cachedImageRatio = static_cast<float>(image->width()) / image->height();
            cachedDestRatio = static_cast<float>(Width) / Height;
            ratioCached = true;
        }
        float imageRatio = cachedImageRatio;
        float containerRatio = cachedDestRatio;
        
        if (imageRatio > containerRatio) {
            // 图片更宽，以宽度为准
            float newHeight = Width / imageRatio;
            float offsetY = (Height - newHeight) / 2;
            dest = SkRect::MakeXYWH(X, Y + offsetY, Width, newHeight);
        } else {
            // 图片更高，以高度为准
            float newWidth = Height * imageRatio;
            float offsetX = (Width - newWidth) / 2;
            dest = SkRect::MakeXYWH(X + offsetX, Y, newWidth, Height);
        }
        
        canvas->drawImageRect(image, dest, SkSamplingOptions(), &paint);
    }
    else if (mode == 3) {
        // 裁剪填充 - 填充整个容器
        SkRect dest = SkRect::MakeXYWH(X, Y, Width, Height);
        
        // 使用缓存的比例
        if (!ratioCached) {
            cachedImageRatio = static_cast<float>(image->width()) / image->height();
            cachedDestRatio = static_cast<float>(Width) / Height;
            ratioCached = true;
        }
        float imageRatio = cachedImageRatio;
        float containerRatio = cachedDestRatio;
        
        SkRect src;
        if (imageRatio > containerRatio) {
            // 图片更宽，裁剪宽度
            float newWidth = image->height() * containerRatio;
            float offsetX = (image->width() - newWidth) / 2;
            src = SkRect::MakeXYWH(offsetX, 0, newWidth, image->height());
        } else {
            // 图片更高，裁剪高度
            float newHeight = image->width() / containerRatio;
            float offsetY = (image->height() - newHeight) / 2;
            src = SkRect::MakeXYWH(0, offsetY, image->width(), newHeight);
        }
        
        canvas->drawImageRect(image, src, dest, SkSamplingOptions(), &paint,
                            SkCanvas::kFast_SrcRectConstraint);
    }
}

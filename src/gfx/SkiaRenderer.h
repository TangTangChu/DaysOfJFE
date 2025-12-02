#pragma once
#include "gfx/IRenderer.h"
#include <memory>
#include <string>

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontMetrics.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRRect.h"
#include "include/core/SkRect.h"
#include "include/core/SkSurface.h"

#include "skia/gpu/ganesh/GrBackendSurface.h"
#include "skia/gpu/ganesh/GrDirectContext.h"
#include "skia/gpu/ganesh/SkSurfaceGanesh.h"
#include "skia/gpu/ganesh/gl/GrGLInterface.h"
#include "skia/gpu/ganesh/gl/win/GrGLMakeWinInterface.h"

class SkiaRenderer final : public IRenderer {
  public:
    SkiaRenderer() = default;
    ~SkiaRenderer() override;
    void initGL(int fbw, int fbh, bool flipY = false);
    void resizeIfNeeded(int fbw, int fbh);
    void beginFrame();
    void endFrame();
    int width() const override { return m_width; }
    int height() const override { return m_height; }
    void clear(Color c) override;

    void drawRect(const gfx::Rect &rc, Color c, bool filled = true,
                  float strokeWidth = 1.0f) override;

    void drawRoundRect(const gfx::Rect &rc, float radius, Color c,
                       bool filled = true, float strokeWidth = 1.0f) override;

    void drawRRect(const gfx::Rect &rc, const gfx::CornerRadius &rad, Color c,
                   bool filled = true, float strokeWidth = 1.0f) override;

    void drawLine(gfx::Vec2 a, gfx::Vec2 b, Color c,
                  float strokeWidth = 1.0f) override;

    void drawCircle(gfx::Vec2 center, float r, Color c, bool filled = true,
                    float strokeWidth = 1.0f) override;

    void drawImageRect(const ImageHandle &img, const gfx::Rect &src,
                       const gfx::Rect &dst, float alpha = 1.0f) override;

    TextMetrics measureText(const std::string &utf8, float sizePx) override;

    void drawText(const std::string &utf8, gfx::Vec2 pos, float sizePx, Color c,
                  TextHAlign hAlign = TextHAlign::Left,
                  TextVAlign vAlign = TextVAlign::Baseline) override;

    void drawTextStroke(const std::string &utf8, gfx::Vec2 pos, float sizePx,
                        Color fill, Color stroke, float strokeWidth,
                        TextHAlign hAlign = TextHAlign::Left,
                        TextVAlign vAlign = TextVAlign::Baseline) override;

  private:
    int m_width = 0;
    int m_height = 0;
    bool m_flipY = false;

    sk_sp<GrDirectContext> m_context;
    sk_sp<SkSurface> m_surface;
    SkCanvas *m_canvas = nullptr;

    void recreateSurface();

    static SkColor toSkColor(Color c);
    static SkPaint makePaint(Color c, bool filled, float strokeWidth);

    void computeTextPos(const std::string &utf8, const SkFont &font,
                        TextHAlign hAlign, TextVAlign vAlign, float x, float y,
                        float &outX, float &outBaselineY) const;
};

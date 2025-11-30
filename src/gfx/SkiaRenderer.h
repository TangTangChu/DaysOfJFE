#pragma once
#include "gfx/IRenderer.h"
#include <memory>
#include <string>

// core (路径仍是 include/core)
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
    void initGL(int fbw, int fbh, bool flipY = true);
    void resizeIfNeeded(int fbw, int fbh);
    void beginFrame();
    void endFrame();
    int width() const override { return m_width; }
    int height() const override { return m_height; }
    void clear(Color c) override;
    void drawImage(const ImageHandle &img, float sx, float sy, float sw,
                   float sh, float dx, float dy, float dw, float dh,
                   float alpha = 1.0f) override;

    void drawRect(float x, float y, float w, float h, Color c,
                  bool filled = true, float strokeWidth = 1.0f) override;

    void drawRoundRect(float x, float y, float w, float h, float radius,
                       Color c, bool filled = true,
                       float strokeWidth = 1.0f) override;

    void drawRRect(float x, float y, float w, float h, CornerRadius rad,
                   Color c, bool filled = true,
                   float strokeWidth = 1.0f) override;

    void drawLine(float x1, float y1, float x2, float y2, Color c,
                  float strokeWidth = 1.0f) override;

    void drawCircle(float cx, float cy, float r, Color c, bool filled = true,
                    float strokeWidth = 1.0f) override;

    void drawText(const std::string &utf8, float x, float y, float sizePx,
                  Color c, TextHAlign hAlign = TextHAlign::Left,
                  TextVAlign vAlign = TextVAlign::Baseline) override;

    void drawTextStroke(const std::string &utf8, float x, float y, float sizePx,
                        Color fill, Color stroke, float strokeWidth,
                        TextHAlign hAlign = TextHAlign::Left,
                        TextVAlign vAlign = TextVAlign::Baseline) override;

  private:
    int m_width = 0;
    int m_height = 0;
    bool m_flipY = true;

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

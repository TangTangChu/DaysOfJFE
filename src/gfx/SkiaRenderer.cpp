#include "gfx/SkiaRenderer.h"
#include <algorithm>

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkSurface.h"
#include "include/core/SkTextBlob.h"
#include "include/utils/SkTextUtils.h"
#include <glad/glad.h>

#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"

SkiaRenderer::~SkiaRenderer() {
    if (m_context && m_surface) {
        m_context->flush(m_surface.get());
        m_context->submit(GrSyncCpu::kNo);
    }
    m_surface.reset();
    m_context.reset();
}

void SkiaRenderer::initGL(int fbw, int fbh, bool flipY) {
    m_width = fbw;
    m_height = fbh;
    m_flipY = flipY;

    auto glIf = GrGLMakeNativeInterface();
    m_context = GrDirectContexts::MakeGL(glIf);
    recreateSurface();
}

void SkiaRenderer::resizeIfNeeded(int fbw, int fbh) {
    if (fbw == m_width && fbh == m_height)
        return;
    m_width = fbw;
    m_height = fbh;
    recreateSurface();
}

void SkiaRenderer::beginFrame() {
    if (!m_surface)
        recreateSurface();
    m_canvas = m_surface ? m_surface->getCanvas() : nullptr;
    if (!m_canvas)
        return;

    if (m_flipY) {
        m_canvas->save();
        m_canvas->translate(0, (SkScalar)m_height);
        m_canvas->scale(1, -1);
    }
}

void SkiaRenderer::endFrame() {
    if (!m_canvas)
        return;

    if (m_flipY)
        m_canvas->restore();

    if (m_context && m_surface) {
        m_context->flush(m_surface.get());
        m_context->submit(GrSyncCpu::kNo);
    }

    m_canvas = nullptr;
}

void SkiaRenderer::recreateSurface() {
    if (!m_context)
        return;

    GLint fbo = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);

    GLint samples = 0;
    glGetIntegerv(GL_SAMPLES, &samples);

    GLint stencil = 8;

    GrGLFramebufferInfo fbInfo;
    fbInfo.fFBOID = (GrGLuint)fbo;
    fbInfo.fFormat = GL_RGBA8;

    GrBackendRenderTarget backendRT = GrBackendRenderTargets::MakeGL(
        m_width, m_height, samples, stencil, fbInfo);

    SkSurfaceProps props(0, kUnknown_SkPixelGeometry);

    m_surface = SkSurfaces::WrapBackendRenderTarget(
        m_context.get(), backendRT, kBottomLeft_GrSurfaceOrigin,
        kRGBA_8888_SkColorType, nullptr, &props);

    if (!m_surface) {
        SkImageInfo info = SkImageInfo::Make(
            m_width, m_height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
        m_surface = SkSurfaces::RenderTarget(m_context.get(),
                                             skgpu::Budgeted::kNo, info);
    }

    m_canvas = m_surface ? m_surface->getCanvas() : nullptr;
}

SkColor SkiaRenderer::toSkColor(Color c) {
    return SkColorSetARGB(c.a, c.r, c.g, c.b);
}

SkPaint SkiaRenderer::makePaint(Color c, bool filled, float strokeWidth) {
    SkPaint p;
    p.setAntiAlias(true);
    p.setColor(toSkColor(c));
    if (filled) {
        p.setStyle(SkPaint::kFill_Style);
    } else {
        p.setStyle(SkPaint::kStroke_Style);
        p.setStrokeWidth(strokeWidth);
    }
    return p;
}

void SkiaRenderer::clear(Color c) {
    if (!m_canvas)
        return;
    m_canvas->clear(toSkColor(c));
}

void SkiaRenderer::drawImage(const ImageHandle &img, float sx, float sy,
                             float sw, float sh, float dx, float dy, float dw,
                             float dh, float alpha) {
    if (!m_canvas || !img.image)
        return;

    SkRect src = SkRect::MakeXYWH(sx, sy, sw, sh);
    SkRect dst = SkRect::MakeXYWH(dx, dy, dw, dh);

    SkPaint p;
    p.setAntiAlias(true);
    p.setAlphaf(std::clamp(alpha, 0.0f, 1.0f));

    SkSamplingOptions sampling(SkFilterMode::kLinear, SkMipmapMode::kLinear);

    m_canvas->drawImageRect(img.image, src, dst, sampling, &p,
                            SkCanvas::kFast_SrcRectConstraint);
}

void SkiaRenderer::drawRect(float x, float y, float w, float h, Color c,
                            bool filled, float strokeWidth) {
    if (!m_canvas)
        return;
    SkPaint p = makePaint(c, filled, strokeWidth);
    m_canvas->drawRect(SkRect::MakeXYWH(x, y, w, h), p);
}

void SkiaRenderer::drawRoundRect(float x, float y, float w, float h,
                                 float radius, Color c, bool filled,
                                 float strokeWidth) {
    if (!m_canvas)
        return;
    SkPaint p = makePaint(c, filled, strokeWidth);
    SkRRect rr =
        SkRRect::MakeRectXY(SkRect::MakeXYWH(x, y, w, h), radius, radius);
    m_canvas->drawRRect(rr, p);
}

void SkiaRenderer::drawRRect(float x, float y, float w, float h,
                             CornerRadius rad, Color c, bool filled,
                             float strokeWidth) {
    if (!m_canvas)
        return;
    SkPaint p = makePaint(c, filled, strokeWidth);

    SkRect rect = SkRect::MakeXYWH(x, y, w, h);
    SkVector radii[4] = {
        {rad.A, rad.A}, {rad.B, rad.B}, {rad.C, rad.C}, {rad.D, rad.D}};

    SkRRect rr;
    rr.setRectRadii(rect, radii);
    m_canvas->drawRRect(rr, p);
}

void SkiaRenderer::drawLine(float x1, float y1, float x2, float y2, Color c,
                            float strokeWidth) {
    if (!m_canvas)
        return;
    SkPaint p = makePaint(c, false, strokeWidth);
    m_canvas->drawLine(x1, y1, x2, y2, p);
}

void SkiaRenderer::drawCircle(float cx, float cy, float r, Color c, bool filled,
                              float strokeWidth) {
    if (!m_canvas)
        return;
    SkPaint p = makePaint(c, filled, strokeWidth);
    m_canvas->drawCircle(cx, cy, r, p);
}

void SkiaRenderer::computeTextPos(const std::string &utf8, const SkFont &font,
                                  TextHAlign hAlign, TextVAlign vAlign, float x,
                                  float y, float &outX,
                                  float &outBaselineY) const {
    SkRect bounds;
    font.measureText(utf8.c_str(), utf8.size(), SkTextEncoding::kUTF8, &bounds);

    float textW = bounds.width();

    outX = x;
    switch (hAlign) {
    case TextHAlign::Left:
        outX -= bounds.left();
        break;
    case TextHAlign::Center:
        outX -= textW * 0.5f + bounds.left();
        break;
    case TextHAlign::Right:
        outX -= textW + bounds.left();
        break;
    }

    SkFontMetrics fm;
    font.getMetrics(&fm);

    outBaselineY = y;
    switch (vAlign) {
    case TextVAlign::Baseline:
        break;
    case TextVAlign::Top:
        outBaselineY = y - fm.fAscent;
        break;
    case TextVAlign::Middle:
        outBaselineY = y - (fm.fAscent + fm.fDescent) * 0.5f;
        break;
    case TextVAlign::Bottom:
        outBaselineY = y - fm.fDescent;
        break;
    }
}

void SkiaRenderer::drawText(const std::string &utf8, float x, float y,
                            float sizePx, Color c, TextHAlign hAlign,
                            TextVAlign vAlign) {
    if (!m_canvas)
        return;

    SkFont font(nullptr, sizePx);

    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(toSkColor(c));
    paint.setStyle(SkPaint::kFill_Style);

    float dx, baselineY;
    computeTextPos(utf8, font, hAlign, vAlign, x, y, dx, baselineY);

    m_canvas->drawString(utf8.c_str(), dx, baselineY, font, paint);
}

void SkiaRenderer::drawTextStroke(const std::string &utf8, float x, float y,
                                  float sizePx, Color fill, Color stroke,
                                  float strokeWidth, TextHAlign hAlign,
                                  TextVAlign vAlign) {
    if (!m_canvas)
        return;

    SkFont font(nullptr, sizePx);

    float dx, baselineY;
    computeTextPos(utf8, font, hAlign, vAlign, x, y, dx, baselineY);

    SkPaint strokePaint;
    strokePaint.setAntiAlias(true);
    strokePaint.setColor(toSkColor(stroke));
    strokePaint.setStyle(SkPaint::kStroke_Style);
    strokePaint.setStrokeWidth(strokeWidth);

    SkPaint fillPaint;
    fillPaint.setAntiAlias(true);
    fillPaint.setColor(toSkColor(fill));
    fillPaint.setStyle(SkPaint::kFill_Style);

    m_canvas->drawString(utf8.c_str(), dx, baselineY, font, strokePaint);
    m_canvas->drawString(utf8.c_str(), dx, baselineY, font, fillPaint);
}

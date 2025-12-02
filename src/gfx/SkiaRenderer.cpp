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
static inline SkRect toSkRect(const gfx::Rect &r) {
    return SkRect::MakeXYWH(r.x, r.y, r.w, r.h);
}

static inline SkPoint toSkPoint(gfx::Vec2 v) { return SkPoint::Make(v.x, v.y); }

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

    // if (m_flipY) {
    //     m_canvas->save();
    //     m_canvas->translate(0, (SkScalar)m_height);
    //     m_canvas->scale(1, -1);
    // }
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

void SkiaRenderer::drawImageRect(const ImageHandle &img, const gfx::Rect &src,
                                 const gfx::Rect &dst, float alpha) {
    if (!m_canvas || !img.image)
        return;

    SkPaint p;
    p.setAntiAlias(true);
    p.setAlphaf(std::clamp(alpha, 0.0f, 1.0f));

    SkSamplingOptions sampling(SkFilterMode::kLinear, SkMipmapMode::kLinear);

    m_canvas->drawImageRect(img.image, toSkRect(src), toSkRect(dst), sampling,
                            &p, SkCanvas::kFast_SrcRectConstraint);
}

void SkiaRenderer::drawRect(const gfx::Rect &rc, Color c, bool filled,
                            float strokeWidth) {
    if (!m_canvas)
        return;
    SkPaint p = makePaint(c, filled, strokeWidth);
    m_canvas->drawRect(toSkRect(rc), p);
}

void SkiaRenderer::drawRoundRect(const gfx::Rect &rc, float radius, Color c,
                                 bool filled, float strokeWidth) {
    if (!m_canvas)
        return;
    SkPaint p = makePaint(c, filled, strokeWidth);
    SkRRect rr = SkRRect::MakeRectXY(toSkRect(rc), radius, radius);
    m_canvas->drawRRect(rr, p);
}

void SkiaRenderer::drawRRect(const gfx::Rect &rc, const gfx::CornerRadius &rad,
                             Color c, bool filled, float strokeWidth) {
    if (!m_canvas)
        return;
    SkPaint p = makePaint(c, filled, strokeWidth);

    SkRect rect = toSkRect(rc);
    SkVector radii[4] = {
        {rad.tl, rad.tl}, {rad.tr, rad.tr}, {rad.br, rad.br}, {rad.bl, rad.bl}
    };

    SkRRect rr;
    rr.setRectRadii(rect, radii);
    m_canvas->drawRRect(rr, p);
}

void SkiaRenderer::drawLine(gfx::Vec2 a, gfx::Vec2 b, Color c,
                            float strokeWidth) {
    if (!m_canvas)
        return;
    SkPaint p = makePaint(c, false, strokeWidth);
    m_canvas->drawLine(a.x, a.y, b.x, b.y, p);
}

void SkiaRenderer::drawCircle(gfx::Vec2 center, float r, Color c, bool filled,
                              float strokeWidth) {
    if (!m_canvas)
        return;
    SkPaint p = makePaint(c, filled, strokeWidth);
    m_canvas->drawCircle(center.x, center.y, r, p);
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
TextMetrics SkiaRenderer::measureText(const std::string &utf8, float sizePx) {
    TextMetrics tm;
    SkFont font(nullptr, sizePx);

    SkRect bounds;
    font.measureText(utf8.c_str(), utf8.size(), SkTextEncoding::kUTF8, &bounds);

    tm.w = bounds.width();

    SkFontMetrics fm;
    font.getMetrics(&fm);
    tm.ascent = -fm.fAscent;
    tm.descent = fm.fDescent;
    tm.h = tm.ascent + tm.descent;
    return tm;
}

void SkiaRenderer::drawText(const std::string &utf8, gfx::Vec2 pos,
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
    computeTextPos(utf8, font, hAlign, vAlign, pos.x, pos.y, dx, baselineY);

    m_canvas->drawString(utf8.c_str(), dx, baselineY, font, paint);
}

void SkiaRenderer::drawTextStroke(const std::string &utf8, gfx::Vec2 pos,
                                  float sizePx, Color fill, Color stroke,
                                  float strokeWidth, TextHAlign hAlign,
                                  TextVAlign vAlign) {
    if (!m_canvas)
        return;

    SkFont font(nullptr, sizePx);

    float dx, baselineY;
    computeTextPos(utf8, font, hAlign, vAlign, pos.x, pos.y, dx, baselineY);

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

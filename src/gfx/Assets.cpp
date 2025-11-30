#include "Assets.h"

#include "include/codec/SkCodec.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"

ImageHandle Assets::LoadImage(const char *path) {
    ImageHandle h;

    auto data = SkData::MakeFromFileName(path);
    if (!data)
        return h;

    auto img = SkImages::DeferredFromEncodedData(data);
    if (!img)
        return h;

    h.image = img;
    h.width = img->width();
    h.height = img->height();
    return h;
}

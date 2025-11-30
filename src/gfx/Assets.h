#pragma once
#include "IRenderer.h"

class Assets {
  public:
    static ImageHandle LoadImage(const char *path);
    static ImageHandle LoadImage(const std::string &path) {
        return LoadImage(path.c_str());
    }
};

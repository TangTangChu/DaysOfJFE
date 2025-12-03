#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


#include "include/core/SkFont.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkTypeface.h"

struct FontStyle {
    enum Weight {
        kInvisible = 0,
        kThin = 100,
        kExtraLight = 200,
        kLight = 300,
        kNormal = 400,
        kMedium = 500,
        kSemiBold = 600,
        kBold = 700,
        kExtraBold = 800,
        kBlack = 900,
        kExtraBlack = 1000
    };

    enum Slant { kUpright, kItalic, kOblique };

    Weight weight = Weight::kNormal;
    Slant slant = Slant::kUpright;
    float width = 1.0f; // 1.0 = normal width
};

class FontManager {
  public:
    static FontManager &Instance();

    // 初始化字体管理器
    bool Initialize(const std::string &defaultFontPath = "");

    // 注册字体文件
    bool RegisterFont(const std::string &name, const std::string &filePath);

    // 从内存加载字体
    bool RegisterFontFromMemory(const std::string &name, const void *data,
                                size_t size);

    // 设置默认字体
    void SetDefaultFont(const std::string &name);

    // 获取字体
    sk_sp<SkTypeface> GetTypeface(const std::string &name = "");
    sk_sp<SkTypeface> GetTypeface(const std::string &name,
                                  const FontStyle &style);

    // 创建SkFont对象
    SkFont CreateFont(const std::string &name = "", float size = 14.0f,
                      const FontStyle &style = FontStyle());

    // 列出所有可用字体
    std::vector<std::string> GetAvailableFonts() const;

    // 检查字体是否存在
    bool HasFont(const std::string &name) const;

    // 设置字体系列
    void SetFontFamily(const std::vector<std::string> &family);

  private:
    FontManager() = default;
    FontManager(const FontManager &) = delete;
    FontManager &operator=(const FontManager &) = delete;

    struct FontEntry {
        sk_sp<SkTypeface> typeface;
        std::string filePath;
        bool isMemoryLoaded = false;
    };

    std::unordered_map<std::string, FontEntry> m_fonts;
    std::vector<std::string> m_fontFamily; // 后备字体链
    std::string m_defaultFont;
    sk_sp<SkFontMgr> m_fontMgr;

    sk_sp<SkTypeface> loadTypefaceFromFile(const std::string &filePath);
    sk_sp<SkTypeface> findSystemFont(const std::string &name);
};
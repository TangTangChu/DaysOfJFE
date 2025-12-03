#include "gfx/FontManager.h"
#include "include/core/SkFontMgr.h"
#if defined(_WIN32)
#include "include/ports/SkTypeface_win.h"
#elif defined(__APPLE__)
#include "include/ports/SkFontMgr_mac_ct.h"
#else
#include "include/ports/SkFontMgr_fontconfig.h"
#endif

#include "include/core/SkStream.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

FontManager &FontManager::Instance() {
    static FontManager instance;
    return instance;
}

bool FontManager::Initialize(const std::string &defaultFontPath) {
#if defined(_WIN32)
    m_fontMgr = SkFontMgr_New_DirectWrite(); // Windows 默认推荐 DirectWrite
#elif defined(__APPLE__)
    m_fontMgr = SkFontMgr_New_CoreText(nullptr);
#else
    m_fontMgr = SkFontMgr_New_FontConfig(nullptr);
#endif

    if (!m_fontMgr) {
        m_fontMgr = SkFontMgr::RefEmpty();
    }

    if (!defaultFontPath.empty()) {
        if (fs::exists(defaultFontPath)) {
            auto typeface = loadTypefaceFromFile(defaultFontPath);
            if (typeface) {
                m_fonts["default"] =
                    FontEntry{typeface, defaultFontPath, false};
                m_defaultFont = "default";
                std::cout << "Loaded default font: " << defaultFontPath
                          << std::endl;
                return true;
            }
        }
    }

    auto systemDefault = m_fontMgr->legacyMakeTypeface(nullptr, SkFontStyle());
    if (systemDefault) {
        m_fonts["system_default"] = FontEntry{systemDefault, "", false};
        m_defaultFont = "system_default";
    }

    m_fontFamily = {"default", "system_default", "Arial", "sans-serif"};

    return true;
}

bool FontManager::RegisterFont(const std::string &name,
                               const std::string &filePath) {
    if (!fs::exists(filePath)) {
        std::cerr << "Font file not found: " << filePath << std::endl;
        return false;
    }

    auto typeface = loadTypefaceFromFile(filePath);
    if (!typeface) {
        std::cerr << "Failed to load font: " << filePath << std::endl;
        return false;
    }

    m_fonts[name] = FontEntry{typeface, filePath, false};
    std::cout << "Registered font '" << name << "' from: " << filePath
              << std::endl;
    return true;
}

bool FontManager::RegisterFontFromMemory(const std::string &name,
                                         const void *data, size_t size) {
    if (!data || size == 0) {
        return false;
    }

    auto stream = std::make_unique<SkMemoryStream>(data, size, true);
    auto typeface = m_fontMgr->makeFromStream(std::move(stream));

    if (typeface) {
        m_fonts[name] = FontEntry{typeface, "", true};
        return true;
    }

    return false;
}

void FontManager::SetDefaultFont(const std::string &name) {
    if (m_fonts.find(name) != m_fonts.end()) {
        m_defaultFont = name;
    }
}

sk_sp<SkTypeface> FontManager::GetTypeface(const std::string &name) {
    if (name.empty() && !m_defaultFont.empty()) {
        return GetTypeface(m_defaultFont);
    }

    auto it = m_fonts.find(name);
    if (it != m_fonts.end()) {
        return it->second.typeface;
    }

    auto systemTypeface = findSystemFont(name);
    if (systemTypeface) {
        m_fonts[name] = FontEntry{systemTypeface, "", false};
        return systemTypeface;
    }

    // 返回默认字体
    return GetTypeface(m_defaultFont);
}

sk_sp<SkTypeface> FontManager::GetTypeface(const std::string &name,
                                           const FontStyle &style) {
    auto baseTypeface = GetTypeface(name);
    if (!baseTypeface || !m_fontMgr)
        return baseTypeface;

    SkFontStyle skStyle(static_cast<SkFontStyle::Weight>(style.weight),
                        static_cast<SkFontStyle::Width>(style.width * 100),
                        style.slant == FontStyle::kItalic
                            ? SkFontStyle::Slant::kItalic_Slant
                        : style.slant == FontStyle::kOblique
                            ? SkFontStyle::Slant::kOblique_Slant
                            : SkFontStyle::Slant::kUpright_Slant);

    SkString familyName;
    baseTypeface->getFamilyName(&familyName);

    if (auto matched =
            m_fontMgr->matchFamilyStyle(familyName.c_str(), skStyle)) {
        return matched;
    }
    return baseTypeface;
}

SkFont FontManager::CreateFont(const std::string &name, float size,
                               const FontStyle &style) {
    auto typeface = GetTypeface(name, style);
    if (!typeface) {
        typeface = GetTypeface(m_defaultFont, style);
    }

    if (typeface) {
        return SkFont(typeface, size);
    }

    // 最后备选：系统默认
    return SkFont(nullptr, size);
}

std::vector<std::string> FontManager::GetAvailableFonts() const {
    std::vector<std::string> fonts;
    for (const auto &pair : m_fonts) {
        fonts.push_back(pair.first);
    }
    return fonts;
}

bool FontManager::HasFont(const std::string &name) const {
    return m_fonts.find(name) != m_fonts.end();
}

void FontManager::SetFontFamily(const std::vector<std::string> &family) {
    m_fontFamily = family;
}

sk_sp<SkTypeface>
FontManager::loadTypefaceFromFile(const std::string &filePath) {
    if (!m_fontMgr)
        return nullptr;
    return m_fontMgr->makeFromFile(filePath.c_str(), 0);
}

sk_sp<SkTypeface> FontManager::findSystemFont(const std::string &name) {
    if (!m_fontMgr) {
        return nullptr;
    }

    SkFontStyle style;
    return m_fontMgr->matchFamilyStyle(name.c_str(), style);
}
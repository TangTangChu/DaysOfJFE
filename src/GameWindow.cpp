/**
 * @file GameWindow.cpp
 * @brief 游戏主窗口实现 - 处理对话、立绘、场景等核心游戏逻辑
 * 
 * 负责功能:
 * - 对话系统: 打字机动画、角色名显示
 * - 立绘管理: 显示/隐藏/高亮/表情切换
 * - 场景控制: 背景切换、章节过渡
 * - 脚本解析: YAML脚本执行
 * - UI交互: 功能按钮、自动播放
 */
#include "GameWindow.h"
#include "SettingWindow.h"
#include "IGlobalEvent.h"
#include <filesystem>
#include <chrono>
#include <thread>
#include <iostream>

namespace {
// 提取文件名，防止脚本里带有 src/image/background/... 路径导致找不到资源
std::string GetFilename(const std::string& path) {
    auto pos = path.find_last_of("/\\");
    if (pos == std::string::npos) return path;
    return path.substr(pos + 1);
}

// 解析角色标识（既支持 name 也支持 id）
std::string ResolveCharacterName(const GameScript& gs, const std::string& nameOrId) {
    // 优先按名称查
    if (gs.GetCharacterDef(nameOrId)) return nameOrId;
    // 再按 id 查
    if (auto def = gs.GetCharacterDefById(nameOrId)) {
        // 返回 id，后续使用时也会同时登记 name 和 id，便于 hide
        return nameOrId;
    }
    return nameOrId; // fallback
}
}

GameWindow::GameWindow() 
    : tbk(std::make_shared<TextBlock>("", 24))
    , tbk2(std::make_shared<OutlineText>("", 28, 2))
    , ic1(std::make_shared<ImageContainer>())
    , Bg1(std::make_shared<ImageContainer>())
    , Bg2(std::make_shared<ImageContainer>()) {
    InitializeUI();
}

void GameWindow::InitializeUI() {
    // 设置下一段对话的回调
    nextDialog = [this]() {
        if (!ic1->IsVisible()) {
            ReverseDialogVisibility();
            return;
        }
        if (!scriptQueue.empty()) {
            SkipDialogueAnimation();
            scriptQueue.pop();
            ProcessNextItem();
        }
    };

    // 鼠标右键隐藏/显示对话框
    OnMouseRBUPHook = [this]() {
        ReverseDialogVisibility();
    };

    // 键盘事件
    OnKeyDownHook = [this](SDL_Keycode key) {
        if (key == SDLK_ESCAPE) {
            ReverseDialogVisibility();
        } else if (key == SDLK_SPACE || key == SDLK_RETURN) {
            nextDialog();
        }
    };

    constexpr float kWinW = 1600.f;
    constexpr float kWinH = 900.f;

    // 背景设置
    Bg1->SetAlpha(0);
    Bg2->SetAlpha(255);
    Bg1->SetRectF(SkRect::MakeWH(kWinW, kWinH));
    Bg2->SetRectF(SkRect::MakeWH(kWinW, kWinH));
    Bg1->SetMode(3);
    Bg2->SetMode(3);

    // 点击区域
    cp1 = std::make_shared<ClickPaper>(nextDialog);
    cp1->SetRect(SkRect::MakeWH(kWinW, kWinH));

    // 对话框背景
    SkRect rectf2 = SkRect::MakeXYWH(0, 615, kWinW, 285);
    ic1->SetRectF(rectf2);
    ic1->SetMode(3);
    ic1->SetImageFromFile("assets/image/misc/footer.png");

    // 角色名文本框
    tbk->SetPosition(55, 620);
    tbk->SetSize(400, 50);
    tbk->SetFontSize(24);
    tbk->SetFontStyle(1);

    // 对话文本框
    tbk2->SetPosition(38, 710);
    tbk2->SetSize(1520, 170);

    // 标题转场
    titletrs = std::make_shared<TitleTransition>();
    titletrs->SetVisible(false);

    // 场景选择窗口
    lsw = std::make_shared<LoadSceneWindow>(
        [this]() { LoadScene(1); m_globalEvent->RequestWindowSwitch(2); },
        [this]() { LoadScene(2); m_globalEvent->RequestWindowSwitch(2); },
        [this]() { LoadScene(3); m_globalEvent->RequestWindowSwitch(2); },
        [this]() { LoadScene(4); m_globalEvent->RequestWindowSwitch(2); },
        [this]() { LoadScene(5); m_globalEvent->RequestWindowSwitch(2); },
        [this]() { m_globalEvent->RequestWindowSwitch(2); }
    );

    // 功能按钮
    btn_save = std::make_shared<PrimaryButton>("保存", [this]() {
        auto sw = std::make_shared<SettingWindow>(
            [this]() { m_globalEvent->RequestWindowSwitch(2); },
            "(ᗜˬᗜ) 很遗憾，我没写这个\\n这游戏要什么保存"
        );
        m_globalEvent->RequestWindowSwitch(sw);
    });

    btn_load = std::make_shared<PrimaryButton>("读取", [this]() {
        auto sw = std::make_shared<SettingWindow>(
            [this]() { m_globalEvent->RequestWindowSwitch(2); },
            "(ᗜˬᗜ) 很遗憾，我没写这个\\n这么短的剧情你就将就着看吧"
        );
        m_globalEvent->RequestWindowSwitch(sw);
    });

    btn_qsave = std::make_shared<PrimaryButton>("快速保存", [this]() {
        auto sw = std::make_shared<SettingWindow>(
            [this]() { m_globalEvent->RequestWindowSwitch(2); },
            "(ᗜˬᗜ) 很遗憾，我没写这个\\n这么短的剧情你就将就着看吧"
        );
        m_globalEvent->RequestWindowSwitch(sw);
    });

    btn_qload = std::make_shared<PrimaryButton>("快速读取场景", [this]() {
        m_globalEvent->RequestWindowSwitch(lsw);
    });

    btn_autoplay = std::make_shared<PrimaryButton>("自动播放", [this]() {
        if (isautoplay) {
            timer.Stop();
            isautoplay = false;
            btn_autoplay->SetText("自动播放");
        } else {
            isautoplay = true;
            btn_autoplay->SetText("关闭自动播放");
            timer.Start(2500, [this]() { nextDialog(); });
        }
    });

    btn_setting = std::make_shared<PrimaryButton>("设定", [this]() {
        auto sw = std::make_shared<SettingWindow>(
            [this]() { m_globalEvent->RequestWindowSwitch(2); },
            "(ᗜˬᗜ) 很遗憾，我没写这个\n这游戏要什么设置"
        );
        m_globalEvent->RequestWindowSwitch(sw);
    });

    // 按钮位置设置
    btn_save->SetPosition(1400, 670);
    btn_save->SetFontSize(16);
    btn_save->SetFontStyle(1);
    btn_save->SetSize(200, 30);
    btn_save->SetCornerRadius(CornerRadius{35, 10, 10, 35});

    btn_load->SetPosition(1400, 705);
    btn_load->SetFontSize(16);
    btn_load->SetFontStyle(1);
    btn_load->SetSize(200, 30);
    btn_load->SetCornerRadius(CornerRadius{35, 10, 10, 35});

    btn_qsave->SetPosition(1400, 740);
    btn_qsave->SetFontSize(16);
    btn_qsave->SetFontStyle(1);
    btn_qsave->SetSize(200, 30);
    btn_qsave->SetCornerRadius(CornerRadius{35, 10, 10, 35});

    btn_qload->SetPosition(1400, 775);
    btn_qload->SetFontSize(16);
    btn_qload->SetFontStyle(1);
    btn_qload->SetSize(200, 30);
    btn_qload->SetCornerRadius(CornerRadius{35, 10, 10, 35});

    btn_autoplay->SetPosition(1400, 810);
    btn_autoplay->SetFontSize(16);
    btn_autoplay->SetFontStyle(1);
    btn_autoplay->SetSize(200, 30);
    btn_autoplay->SetCornerRadius(CornerRadius{35, 10, 10, 35});

    btn_setting->SetPosition(1400, 845);
    btn_setting->SetFontSize(16);
    btn_setting->SetFontStyle(1);
    btn_setting->SetSize(200, 30);
    btn_setting->SetCornerRadius(CornerRadius{35, 10, 10, 35});

    // 添加控件
    AddBackgroundControl(Bg1);
    AddBackgroundControl(Bg2);
    AddForegroundControl(ic1);
    AddForegroundControl(tbk);
    AddForegroundControl(tbk2);
    AddForegroundControl(cp1);
    AddForegroundControl(btn_save);
    AddForegroundControl(btn_load);
    AddForegroundControl(btn_qsave);
    AddForegroundControl(btn_qload);
    AddForegroundControl(btn_autoplay);
    AddForegroundControl(btn_setting);
    AddForegroundControl(titletrs);

    // 初始化章节过渡图片（最上层）
    transitionImage = std::make_shared<ImageContainer>();
    transitionImage->SetImageFromFile("assets/image/misc/过渡-1.png");
    transitionImage->SetRectF(SkRect::MakeXYWH(0, 0, 1600, 900));
    transitionImage->SetMode(1);  // 拉伸模式
    transitionImage->SetAlpha(0);  // 初始隐藏
    transitionImage->SetVisible(false);
    AddForegroundControl(transitionImage);

    // 对话框相关控件列表
    dialogControls.push_back(ic1);
    dialogControls.push_back(tbk);
    dialogControls.push_back(tbk2);
    dialogControls.push_back(btn_save);
    dialogControls.push_back(btn_load);
    dialogControls.push_back(btn_qsave);
    dialogControls.push_back(btn_qload);
    dialogControls.push_back(btn_autoplay);
    dialogControls.push_back(btn_setting);
}

void GameWindow::SetCharacterName(const std::string& name) { 
    tbk->SetText(name);
    tbk->SetVisible(true);
}

void GameWindow::ShowDialogue(const std::string& dialogue) {
    tbk2->SetText(dialogue);
    tbk2->StartAnimation();  // 启用逐字动画
}

void GameWindow::ShowDialogueImmediate(const std::string& dialogue) {
    tbk2->SetText(dialogue);
    tbk2->ShowFullText();
}

void GameWindow::ClearDialogue() { 
    tbk2->ClearText(); 
}

bool GameWindow::IsDialogueFinished() const { 
    return !tbk2->IsAnimating(); 
}

void GameWindow::SkipDialogueAnimation() { 
    tbk2->ShowFullText(); 
}

bool GameWindow::LoadScript(const std::string& filename) {
    return gameScript.LoadFromFile(filename);
}

void GameWindow::OnShow() {
    // 首次显示时自动加载第一个场景
    if (!sceneLoaded) {
        LoadScene(0);
        sceneLoaded = true;
    }
}

void GameWindow::LoadScene(int sceneId) {
    std::cout << "加载场景: " << sceneId << std::endl;
    
    const Chapter& chapter = gameScript.GetChapter();
    if (sceneId < 0 || sceneId >= static_cast<int>(chapter.scenes.size())) {
        std::cerr << "无效的场景ID: " << sceneId << std::endl;
        return;
    }

    const Scene& scene = chapter.scenes[sceneId];
    
    // 清空脚本队列
    while (!scriptQueue.empty()) scriptQueue.pop();
    
    // 加载脚本序列
    for (const auto& item : scene.scriptSequence) {
        scriptQueue.push(item);
    }
    
    // 设置背景
    if (!scene.background.empty()) {
        std::string bgFile = GetFilename(scene.background);
        std::string bgPath = "assets/image/background/" + bgFile;
        Bg2->SetImageFromFile(bgPath);
    }
    
    // 清除旧角色并加载初始角色站位
    ClearCharacters();
    for (const auto& ch : scene.characters) {
        ShowCharacter(ch.ref_name, ch.current_sprite, ch.offset.empty() ? nullptr : &ch.offset);
    }
    
    // 处理第一项
    ProcessNextItem();
}

void GameWindow::ProcessNextItem() {
    if (scriptQueue.empty()) {
        std::cout << "脚本播放完毕" << std::endl;
        return;
    }

    auto& item = scriptQueue.front();
    
    if (std::holds_alternative<Dialogue>(item)) {
        const Dialogue& dialogue = std::get<Dialogue>(item);
        SetCharacterName(dialogue.character);
        ShowDialogue(dialogue.text);
        
        // 更新角色立绘显示状态（说话角色高亮/显示，其他角色变暗）
        UpdateCharacterHighlight(dialogue.character);
        
        // 如果对话指定了表情切换
        if (!dialogue.sprite.empty()) {
            UpdateCharacterSprite(dialogue.character, dialogue.sprite);
        }
        
        isWaitingInput = true;
    } else if (std::holds_alternative<Command>(item)) {
        try {
            const Command& cmd = std::get<Command>(item);
            ExecuteCommand(cmd);
            scriptQueue.pop();
        } catch (const std::exception& e) {
            std::cerr << "命令执行失败: " << e.what() << std::endl;
            scriptQueue.pop();
        }
        ProcessNextItem();  // 命令立即执行，继续下一项
    }
}

void GameWindow::ExecuteCommand(const Command& cmd) {
    std::cout << "执行命令: " << cmd.type << " " << cmd.action << std::endl;
    
    if (cmd.type == "bg") {
        if (cmd.action == "title") {
            // 显示章节标题，4秒后自动隐藏
            titletrs->SetText(cmd.param);
            titletrs->SetVisible(true);
            std::thread([this]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(4000));
                titletrs->SetVisible(false);
            }).detach();
            return;
        }
        
        if (cmd.action == "change") {
            std::string bgFile = GetFilename(cmd.param);
            std::string bgPath = "assets/image/background/" + bgFile;
            
            auto data = SkData::MakeFromFileName(bgPath.c_str());
            if (data) {
                auto image = SkImages::DeferredFromEncodedData(data);
                if (image) {
                    SwitchBackground(image);
                } else {
                    std::cerr << "背景图片解码失败: " << bgPath << std::endl;
                }
            } else {
                std::cerr << "背景图片加载失败: " << bgPath << std::endl;
            }
        }
    } else if (cmd.type == "music") {
        // TODO: 实现音乐播放
        std::cout << "播放音乐: " << cmd.param << std::endl;
    } else if (cmd.type == "chapter") {
        // 章节跳转命令
        if (cmd.action == "drop") {
            try {
                int sceneId = std::stoi(cmd.param);
                
                // 显示过渡图片并执行淡入淡出动画
                transitionImage->SetVisible(true);
                std::thread([this, sceneId]() {
                    constexpr int fadeTime = 500;  // 淡入/淡出时间(ms)
                    constexpr int displayTime = 1000;  // 显示时间(ms)
                    constexpr int frameInterval = 16;  // ~60fps
                    
                    // 淡入
                    auto start = std::chrono::steady_clock::now();
                    while (true) {
                        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - start).count();
                        if (elapsed >= fadeTime) break;
                        
                        float progress = static_cast<float>(elapsed) / fadeTime;
                        transitionImage->SetAlpha(static_cast<uint8_t>(255 * progress));
                        std::this_thread::sleep_for(std::chrono::milliseconds(frameInterval));
                    }
                    transitionImage->SetAlpha(255);
                    
                    // 在过渡图片完全显示时加载新场景
                    std::this_thread::sleep_for(std::chrono::milliseconds(displayTime / 2));
                    LoadScene(sceneId);
                    std::this_thread::sleep_for(std::chrono::milliseconds(displayTime / 2));
                    
                    // 淡出
                    start = std::chrono::steady_clock::now();
                    while (true) {
                        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - start).count();
                        if (elapsed >= fadeTime) break;
                        
                        float progress = static_cast<float>(elapsed) / fadeTime;
                        transitionImage->SetAlpha(static_cast<uint8_t>(255 * (1.0f - progress)));
                        std::this_thread::sleep_for(std::chrono::milliseconds(frameInterval));
                    }
                    transitionImage->SetAlpha(0);
                    transitionImage->SetVisible(false);
                }).detach();
            } catch (const std::exception& e) {
                std::cerr << "章节跳转失败: " << e.what() << std::endl;
            }
        }
    } else if (cmd.type == "character") {
        // YAML中使用的是 "character" 命令
        auto key = ResolveCharacterName(gameScript, cmd.param);
        if (cmd.action == "hide") {
            auto it = characterContainers.find(key);
            if (it != characterContainers.end()) {
                it->second->SetAlpha(0);
            }
        } else if (cmd.action == "show") {
            auto it = characterContainers.find(key);
            if (it != characterContainers.end()) {
                it->second->SetAlpha(255);
            }
        }
    } else if (cmd.type == "char") {
        auto key = ResolveCharacterName(gameScript, cmd.param);
        if (cmd.action == "hide") {
            // 隐藏角色
            auto it = characterContainers.find(key);
            if (it != characterContainers.end()) {
                it->second->SetAlpha(0);
            }
            return;
        }

        // 显示角色（使用 normal 或指定 sprite）
        std::string spriteKeyCmd = cmd.action.empty() || cmd.action == "show" ? "normal" : cmd.action;
        ShowCharacter(key, spriteKeyCmd);
    }
}

void GameWindow::ShowCharacter(const std::string& nameOrId,
                               const std::string& sprite,
                               const std::vector<int>* offsetOverride) {
    const CharacterDef* def = gameScript.GetCharacterDef(nameOrId);
    if (!def) def = gameScript.GetCharacterDefById(nameOrId);
    if (!def) return;

    // 决定存储 key：优先使用角色 id（若存在），否则使用传入 name
    std::string storeKey = !def->id.empty() ? def->id : nameOrId;

    std::string spriteKey = sprite.empty() ? "normal" : sprite;
    std::string spritePath;
    if (!def->sprites.empty()) {
        auto it = def->sprites.find(spriteKey);
        if (it != def->sprites.end()) spritePath = it->second;
        else spritePath = def->sprites.begin()->second; // fallback to first
    }
    if (spritePath.empty()) return;

    std::string spriteFile = GetFilename(spritePath);
    std::string fullPath = "assets/image/character/" + spriteFile;
    if (!std::filesystem::exists(fullPath)) {
        std::cerr << "立绘文件不存在: " << fullPath << std::endl;
        return;
    }
    auto ic = std::make_shared<ImageContainer>();
    ic->SetMode(3);
    ic->SetAlpha(0);  // 初始隐藏，通过对话显示

    CharacterInstance inst;
    inst.ref_name = nameOrId;
    inst.current_sprite = spriteKey;
    inst.offset = offsetOverride ? *offsetOverride : def->offset;
    ic->SetRectF(CalculateCharacterPosition(*def, inst));
    
    // 在添加到控件树前加载图片
    ic->SetImageFromFile(fullPath);
    
    std::cout << "显示立绘: " << nameOrId << " sprite=" << spriteFile
              << " pos=" << ic->GetBounds().x() << "," << ic->GetBounds().y()
              << " w=" << ic->GetBounds().width() << " h=" << ic->GetBounds().height()
              << " visible=" << ic->IsVisible()
              << std::endl;

    // 替换或新增（同时登记 name 和 id，便于 hide）
    auto eraseKey = [&](const std::string& k) {
        auto it = characterContainers.find(k);
        if (it != characterContainers.end()) {
            RemoveControl(it->second);
            characterContainers.erase(it);
        }
    };
    eraseKey(nameOrId);
    eraseKey(storeKey);

    AddControl(ic);  // 立绘添加到Controls层，使其位于对话框下方
    characterContainers[storeKey] = ic;
    characterContainers[nameOrId] = ic;
    
    std::cout << "立绘已添加到窗口: " << storeKey << " 引用计数=" << ic.use_count() << std::endl;
    std::cout << "验证立绘状态: visible=" << ic->IsVisible() << std::endl;
}

void GameWindow::UpdateCharacterHighlight(const std::string& speakingChar) {
    for (auto& pair : characterContainers) {
        if (pair.first == speakingChar) {
            // 说话角色：如果隐藏则显示，已显示则保持高亮
            if (pair.second->GetAlpha() == 0) {
                pair.second->SetAlpha(255);
            }
        } else {
            // 非说话角色：如果已显示则变暗，保持隐藏状态不变
            if (pair.second->GetAlpha() > 0) {
                pair.second->SetAlpha(150);
            }
        }
    }
}

void GameWindow::UpdateCharacterSprite(const std::string& charName, const std::string& spriteName) {
    auto it = characterContainers.find(charName);
    if (it == characterContainers.end()) return;

    const CharacterDef* def = gameScript.GetCharacterDef(charName);
    if (!def) def = gameScript.GetCharacterDefById(charName);
    if (!def || def->sprites.empty()) return;

    auto spriteIt = def->sprites.find(spriteName);
    if (spriteIt == def->sprites.end()) return;

    std::string spriteFile = GetFilename(spriteIt->second);
    std::string fullPath = "assets/image/character/" + spriteFile;
    
    if (!std::filesystem::exists(fullPath)) {
        std::cerr << "表情文件不存在: " << fullPath << std::endl;
        return;
    }

    it->second->SetImageFromFile(fullPath);
    std::cout << "切换表情: " << charName << " -> " << spriteName << std::endl;
}

void GameWindow::ClearCharacters() {
    for (auto& pair : characterContainers) {
        RemoveControl(pair.second);
    }
    characterContainers.clear();
}

void GameWindow::ReverseDialogVisibility() {
    for (auto& ctrl : dialogControls) {
        ctrl->SetVisible(!ctrl->IsVisible());
    }
}

void GameWindow::SwitchBackground(sk_sp<SkImage> bg) {
    Bg2->SetImage(bg);
}

SkRect GameWindow::CalculateCharacterPosition(const CharacterDef& def, const CharacterInstance& instance) {
    float x = 0, y = 0;
    
    if (def.base_pos == "left") {
        x = 200;
    } else if (def.base_pos == "center") {
        x = 540;
    } else if (def.base_pos == "right") {
        x = 880;
    }
    
    y = 100;
    
    // 应用偏移
    if (!instance.offset.empty() && instance.offset.size() >= 2) {
        x += instance.offset[0];
        y += instance.offset[1];
    } else if (!def.offset.empty() && def.offset.size() >= 2) {
        x += def.offset[0];
        y += def.offset[1];
    }
    
    return SkRect::MakeXYWH(x, y, 400, 600);
}

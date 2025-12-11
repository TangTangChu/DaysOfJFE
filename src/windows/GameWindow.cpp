#include "windows/GameWindow.h"

#include "app/ApplicationContext.h"
#include "gfx/Assets.h"
#include "gfx/geom/Geom.h"
#include "windows/LoadSceneWindow.h"
#include "windows/SettingWindow.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <chrono>
#include <format>
#include <iostream>
#include <thread>

using namespace script;

auto fadePair = [](std::shared_ptr<ImageContainer> out,
                   std::shared_ptr<ImageContainer> in, int totalMs) {
    std::thread([out, in, totalMs]() {
        constexpr int frameInterval = 16;
        auto start = std::chrono::steady_clock::now();
        int frame_count = 0;
        while (true) {
            auto elapsed =
                std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::steady_clock::now() - start)
                    .count();
            float p = std::clamp(elapsed / (totalMs * 1000.f), 0.f, 1.f);
            float e = p * p;
            in->SetAlpha((unsigned char)(255 * e));
            out->SetAlpha((unsigned char)(255 * (1 - e)));
            if (p >= 1.f)
                break;
            auto target = start + std::chrono::milliseconds(frameInterval *
                                                            ++frame_count);
            std::this_thread::sleep_until(target);
        }
    }).detach();
};

GameWindow::GameWindow()
    : dialogBox(std::make_shared<DialogBox>()),
      buttonStack(
          std::make_shared<StackPanel>(StackPanel::Orientation::Vertical)),
      Bg1(std::make_shared<ImageContainer>()),
      Bg2(std::make_shared<ImageContainer>()) {
    InitializeUI();
}

void GameWindow::InitializeUI() {
    nextDialog = [this]() {
        if (!dialogBox->IsVisible()) {
            ReverseDialogVisility();
            return;
        }

        if (!IsDialogueFinished()) {
            SkipDialogueAnimation();
            return;
        }

        if (runtime_) {
            runtime_->Next();
        }
    };

    SetOnMouseRBUPHook([this]() { ReverseDialogVisility(); });

    SetOnKeyDownHook([this](int key, int) {
        if (key == GLFW_KEY_ESCAPE) {
            ReverseDialogVisility();
        } else if (key == GLFW_KEY_SPACE || key == GLFW_KEY_ENTER) {
            nextDialog();
        }
    });

    Bg1->SetAlpha(0);
    Bg2->SetAlpha(255);
    Bg1->SetBounds(gfx::Rect::XYWH(0, 0, ViewW, ViewH));
    Bg2->SetBounds(gfx::Rect::XYWH(0, 0, ViewW, ViewH));
    Bg1->SetMode(3); // Aspect Fill?
    Bg2->SetMode(3);

    cp1 = std::make_shared<ClickPaper>(nextDialog);
    cp1->SetBounds(gfx::Rect{0, 0, ViewW, ViewH});

    dialogBox->SetBounds(gfx::Rect::XYWH(20, 600, 1300, 280)); // Initial pos
    dialogBox->UpdateLayout();
    titletrs = std::make_shared<TitleTransition>();
    titletrs->SetVisible(false);

    lsw = std::make_shared<LoadSceneWindow>(
        [this]() {
            StartNewGame();
            if (applicationContext)
                applicationContext->RequestWindowSwitch(1);
        },
        [this]() {
            StartNewGame();
            if (applicationContext)
                applicationContext->RequestWindowSwitch(1);
        },
        [this]() {
            StartNewGame();
            if (applicationContext)
                applicationContext->RequestWindowSwitch(1);
        },
        [this]() {
            StartNewGame();
            if (applicationContext)
                applicationContext->RequestWindowSwitch(1);
        },
        [this]() {
            StartNewGame();
            if (applicationContext)
                applicationContext->RequestWindowSwitch(1);
        },
        [this]() {
            if (applicationContext)
                applicationContext->RequestWindowSwitch(1);
        });

    buttonStack->SetSpacing(5);
    buttonStack->SetPosition(1400, 670);
    buttonStack->SetSize(200, 200); // Bounds for stack

    auto createBtn = [&](const std::string &text, std::function<void()> cb) {
        auto btn = std::make_shared<PrimaryButton>(text, cb);
        btn->SetFontSize(16);
        btn->SetFontStyle(1);
        btn->SetSize(200, 30);
        btn->SetCornerRadius(gfx::CornerRadius{35.f, 10.f, 10.f, 35.f});
        return btn;
    };

    auto btn_save = createBtn("保存", [this]() {
        if (applicationContext) {
            auto sw = std::make_shared<SettingWindow>(
                [this]() { applicationContext->RequestWindowSwitch(1); },
                "(ᗜˬᗜ) 很遗憾，我没写这个\n这游戏要什么保存");
            applicationContext->RequestWindowSwitch(sw);
        }
    });

    auto btn_load = createBtn("读取", [this]() {
        if (applicationContext) {
            auto sw = std::make_shared<SettingWindow>(
                [this]() { applicationContext->RequestWindowSwitch(1); },
                "(ᗜˬᗜ) 很遗憾，我没写这个\n这么短的剧情你就将就着看吧");
            applicationContext->RequestWindowSwitch(sw);
        }
    });

    auto btn_qsave = createBtn("快速保存", [this]() {
        if (applicationContext) {
            auto sw = std::make_shared<SettingWindow>(
                [this]() { applicationContext->RequestWindowSwitch(1); },
                "(ᗜˬᗜ) 很遗憾，我没写这个");
            applicationContext->RequestWindowSwitch(sw);
        }
    });

    auto btn_qload = createBtn("快速读取场景", [this]() {
        if (applicationContext)
            applicationContext->RequestWindowSwitch(lsw);
    });

    btn_autoplay = createBtn("自动播放", [this]() {
        if (isautoplay) {
            timer.stop();
            isautoplay = false;
            btn_autoplay->SetText("自动播放");
        } else {
            isautoplay = true;
            btn_autoplay->SetText("关闭自动播放");
            timer.start(2500, [this]() { nextDialog(); });
        }
    });

    auto btn_setting = createBtn("设定", [this]() {
        if (applicationContext) {
            auto sw = std::make_shared<SettingWindow>(
                [this]() { applicationContext->RequestWindowSwitch(1); },
                "(ᗜˬᗜ) 很遗憾，我没写这个");
            applicationContext->RequestWindowSwitch(sw);
        }
    });

    buttonStack->AddChild(btn_save);
    buttonStack->AddChild(btn_load);
    buttonStack->AddChild(btn_qsave);
    buttonStack->AddChild(btn_qload);
    buttonStack->AddChild(btn_autoplay);
    buttonStack->AddChild(btn_setting);

    // Add everything to window
    AddBackgroundControl(Bg1);
    AddBackgroundControl(Bg2);

    AddForegroundControl(dialogBox);
    AddForegroundControl(buttonStack);
    AddForegroundControl(cp1);
    RemoveForegroundControl(dialogBox);
    RemoveForegroundControl(buttonStack);
    RemoveForegroundControl(cp1);

    AddForegroundControl(dialogBox);
    AddForegroundControl(cp1);
    AddForegroundControl(buttonStack);

    AddForegroundControl(titletrs);

    dialogControls = {dialogBox, buttonStack};
}

void GameWindow::OnWindowResize(int width, int height) {
    windowWidth_ = width;
    windowHeight_ = height;

    // 计算保持 16:9 比例的内容视口区域
    const float targetAspect = ViewW / ViewH; // 16:9
    const float windowAspect = static_cast<float>(width) /
                               static_cast<float>(height == 0 ? 1 : height);

    float contentW = 0.f;
    float contentH = 0.f;
    float offsetX = 0.f;
    float offsetY = 0.f;

    if (windowAspect > targetAspect) {
        // 窗口较宽，高度为限制
        contentH = static_cast<float>(height);
        contentW = contentH * targetAspect;
        offsetX = (static_cast<float>(width) - contentW) * 0.5f;
        offsetY = 0.f;
    } else {
        // 窗口较窄，宽度为限制
        contentW = static_cast<float>(width);
        contentH = contentW / targetAspect;
        offsetX = 0.f;
        offsetY = (static_cast<float>(height) - contentH) * 0.5f;
    }

    contentViewport_ = gfx::Rect::XYWH(offsetX, offsetY, contentW, contentH);

    const float scale = contentW / ViewW;
    // 背景与点击区域都限制在 16:9 内容区域内
    if (Bg1)
        Bg1->SetBounds(contentViewport_);
    if (Bg2)
        Bg2->SetBounds(contentViewport_);
    if (cp1)
        cp1->SetBounds(contentViewport_);

    if (dialogBox) {
        float hDesign = ViewH * 0.35f;
        if (hDesign < 200.f)
            hDesign = 200.f;
        float wDesign = ViewW - 250.f;
        if (wDesign < 600.f)
            wDesign = 600.f; // Min width

        const float yDesign = ViewH - hDesign - 20.f;
        const float xDesign = 20.f;

        const float x = contentViewport_.x + xDesign * scale;
        const float y = contentViewport_.y + yDesign * scale;
        const float w = wDesign * scale;
        const float h = hDesign * scale;

        dialogBox->SetBounds(gfx::Rect::XYWH(x, y, w, h));
        dialogBox->UpdateLayout();
    }

    if (buttonStack) {
        const float x = contentViewport_.x + contentViewport_.w - 220.f * scale;
        const float y = contentViewport_.y + contentViewport_.h - 250.f * scale;
        buttonStack->SetPosition(static_cast<int>(x), static_cast<int>(y));
    }

    LayoutChoicePanel();

    WindowPanel::OnWindowResize(width, height);
}

void GameWindow::SetCharacterName(const std::string &name) {
    dialogBox->SetName(name);
}

void GameWindow::ShowDialogue(const std::string &dialogue) {
    dialogBox->SetText(dialogue);
}

void GameWindow::ShowDialogueImmediate(const std::string &dialogue) {
    dialogBox->SetTextImmediate(dialogue);
}

void GameWindow::ClearDialogue() { dialogBox->ClearText(); }

bool GameWindow::IsDialogueFinished() const {
    return !dialogBox->IsAnimating();
}

void GameWindow::SkipDialogueAnimation() { dialogBox->SkipAnimation(); }

void GameWindow::StartNewGame() {
    try {
        auto loadedChapter = ScriptYamlLoader::LoadChapterFromFile(
            "assets/scripts/demo_script.yaml");
        if (loadedChapter.id.empty()) {
            std::cerr
                << "Failed to load script: assets/scripts/demo_script.yaml"
                << std::endl;
            chapters_.clear();
            Chapter ch;
            ch.id = "fallback";
            Scene sc;
            sc.id = "start";
            Dialogue dlg;
            dlg.text = "找不到剧本文件 "
                       "assets/scripts/demo_script.yaml，已回退到内置模式。";
            sc.sequence.push_back(dlg);
            ch.scenes.emplace(sc.id, sc);
            chapters_.push_back(ch);
        } else {
            chapters_.clear();
            chapters_.push_back(std::move(loadedChapter));
        }
    } catch (const std::exception &e) {
        std::cerr << "YAML Load Error: " << e.what() << std::endl;
        return;
    }

    RuntimeCallbacks cbs;

    cbs.onSceneEnter = [this](const Chapter &chapter, const Scene &scene) {
        currentChapter_ = &chapter;
        SetupSceneVisual(chapter, scene);
    };

    cbs.onDialogue = [this](const Dialogue &dlg) {
        if (dlg.tag == DialogueTag::Dialogue) {
            if (!dlg.displayName.empty())
                SetCharacterName(dlg.displayName);
            else
                SetCharacterName(dlg.speakerId);

            ShowDialogue(std::format("「{}」", dlg.text));
        } else {
            SetCharacterName("");
            ShowDialogue(dlg.text);
        }

        if (!dlg.speakerId.empty()) {
            UpdateCharacterHighlight(dlg.speakerId);
            if (!dlg.sprite.empty()) {
                UpdateCharacterSprite(dlg.speakerId, dlg.sprite);
            }
        }
    };

    cbs.onBg = [this](const BgCommand &cmd) { HandleBgCommand(cmd); };
    cbs.onSound = [this](const SoundCommand &cmd) { HandleSoundCommand(cmd); };
    cbs.onCharacter = [this](const CharacterCommand &cmd) {
        HandleCharacterCommand(cmd);
    };

    cbs.onChoice = [this](const Choice &choice) {
        currentChoiceId_ = choice.id;

        if (!choicePanel_) {
            choicePanel_ =
                std::make_shared<StackPanel>(StackPanel::Orientation::Vertical);
            choicePanel_->SetSpacing(10);
            AddForegroundControl(choicePanel_);
        }

        choicePanel_->ClearChildren();

        const float scale = contentViewport_.w / ViewW;
        float btnWidth = 400.f * scale;
        float btnHeight = 40.f * scale;
        if (btnWidth < 200.f)
            btnWidth = 200.f;
        if (btnHeight < 30.f)
            btnHeight = 30.f;

        for (const auto &opt : choice.options) {
            auto btn = std::make_shared<PrimaryButton>(
                opt.text, [this, choiceId = choice.id, optionId = opt.id]() {
                    if (!runtime_)
                        return;
                    runtime_->Choose(choiceId, optionId);
                    if (choicePanel_) {
                        choicePanel_->SetVisible(false);
                        choicePanel_->ClearChildren();
                    }
                    runtime_->Next();
                });

            btn->SetSize(static_cast<int>(btnWidth),
                         static_cast<int>(btnHeight));
            btn->SetFontSize(static_cast<int>(16 * scale));
            choicePanel_->AddChild(btn);
        }

        choicePanel_->SetVisible(true);
        LayoutChoicePanel();
    };

    runtime_ = std::make_unique<ScriptRuntime>(chapters_, std::move(cbs));

    currentChapter_ = &chapters_.front();
    std::string startScene = "scene1";
    if (!chapters_.empty() && !chapters_[0].scenes.empty()) {
        startScene = chapters_[0].scenes.begin()->first;
    }

    runtime_->Start(currentChapter_->id, startScene);
    runtime_->Next();
}

void GameWindow::SetupSceneVisual(const Chapter &chapter, const Scene &scene) {
    ClearCharacters();

    if (!scene.background.empty()) {
        ImageHandle bgImg = Assets::LoadImage(scene.background.c_str());
        if (!bgImg) {
            std::cout << "背景加载失败: " << scene.background << std::endl;
        } else {
            SwitchBackground(bgImg);
        }
    }

    for (const auto &character : scene.characters) {
        auto it = chapter.characters.find(character.ref_name);
        if (it == chapter.characters.end())
            continue;

        const CharacterDef &def = it->second;

        auto spIt = def.sprites.find(character.sprite);
        if (spIt == def.sprites.end())
            continue;

        ImageHandle charImg = Assets::LoadImage(spIt->second.c_str());
        if (!charImg)
            continue;

        auto container = std::make_shared<ImageContainer>();
        gfx::Rect charRect = CalculateCharacterPosition(def, character);
        container->SetBounds(charRect);
        container->SetMode(3);
        container->SetImage(charImg);
        container->SetAlpha(0);

        characterContainers[character.ref_name] = container;
        AddMidgroundControl(container);
    }
}

void GameWindow::HandleBgCommand(const BgCommand &cmd) {
    if (cmd.action == "change") {
        if (!cmd.image.empty()) {
            ImageHandle img = Assets::LoadImage(cmd.image.c_str());
            if (img)
                SwitchBackground(img);
        }
    } else if (cmd.action == "title") {
        titletrs->SetText(cmd.text);
        titletrs->SetVisible(true);
        std::thread([t = titletrs]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(4000));
            t->SetVisible(false);
        }).detach();
    }
}

void GameWindow::HandleSoundCommand(const SoundCommand &cmd) {
    if (!applicationContext)
        return;
    auto &audio = applicationContext->audio;
    if (cmd.channel == "bgm") {
        if (cmd.action == "play")
            audio.PlayBgm(cmd.file, true);
        else if (cmd.action == "pause")
            audio.PauseBgm();
        else if (cmd.action == "stop")
            audio.StopBgm();
        else if (cmd.action == "volume")
            audio.SetBgmVolume(cmd.volume);
    } else if (cmd.channel == "voice") {
        if (cmd.action == "play")
            audio.PlayVoice(cmd.file);
        else if (cmd.action == "pause")
            audio.PauseVoice();
        else if (cmd.action == "stop")
            audio.StopVoice();
        else if (cmd.action == "volume")
            audio.SetVoiceVolume(cmd.volume);
    } else if (cmd.channel == "sfx") {
        if (cmd.action == "play")
            audio.PlaySfx(cmd.file);
        else if (cmd.action == "stop")
            audio.StopSfx();
        else if (cmd.action == "volume")
            audio.SetSfxVolume(cmd.volume);
    }
}

void GameWindow::HandleCharacterCommand(const CharacterCommand &cmd) {
    if (cmd.action == "show") {
        if (auto it = characterContainers.find(cmd.targetId);
            it != characterContainers.end()) {
            it->second->SetAlpha(255);
            UpdateCharacterHighlight(cmd.targetId);
        }
    } else if (cmd.action == "hide") {
        if (auto it = characterContainers.find(cmd.targetId);
            it != characterContainers.end()) {
            it->second->SetAlpha(0);
        }
    } else if (cmd.action == "sprite") {
        if (!cmd.sprite.empty()) {
            UpdateCharacterSprite(cmd.targetId, cmd.sprite);
        }
    }
}

void GameWindow::UpdateCharacterHighlight(const std::string &speakingChar) {
    for (auto &pair : characterContainers) {
        if (pair.first == speakingChar) {
            if (pair.second->GetAlpha() == 0)
                pair.second->SetAlpha(255);
        } else {
            if (pair.second->GetAlpha() > 0)
                pair.second->SetAlpha(150);
        }
    }
}

void GameWindow::UpdateCharacterSprite(const std::string &charName,
                                       const std::string &spriteName) {
    auto it = characterContainers.find(charName);
    if (it == characterContainers.end())
        return;
    if (!currentChapter_)
        return;

    auto defIt = currentChapter_->characters.find(charName);
    if (defIt == currentChapter_->characters.end())
        return;

    const CharacterDef &def = defIt->second;
    auto spIt = def.sprites.find(spriteName);
    if (spIt == def.sprites.end())
        return;

    ImageHandle newImg = Assets::LoadImage(spIt->second.c_str());
    if (newImg)
        it->second->SetImage(newImg);
}

void GameWindow::ClearCharacters() {
    for (auto &pair : characterContainers) {
        RemoveControl(pair.second);
    }
    characterContainers.clear();
}

void GameWindow::LayoutChoicePanel() {
    if (!choicePanel_ || !choicePanel_->IsVisible())
        return;

    const float scale = contentViewport_.w / ViewW;
    float initialWidth = contentViewport_.w * 0.5f;
    if (initialWidth < 300.f * scale)
        initialWidth = 300.f * scale;

    float tempX =
        contentViewport_.x + (contentViewport_.w - initialWidth) * 0.5f;
    float tempY = contentViewport_.y + contentViewport_.h * 0.4f;

    choicePanel_->SetBounds(static_cast<int>(tempX), static_cast<int>(tempY),
                            static_cast<int>(initialWidth), 0);
    choicePanel_->UpdateLayout();

    int panelW = choicePanel_->GetWidth();
    int panelH = choicePanel_->GetHeight();

    float finalX = contentViewport_.x + (contentViewport_.w - panelW) * 0.5f;
    float finalY = contentViewport_.y + (contentViewport_.h - panelH) * 0.5f;

    if (dialogBox) {
        const auto &dlg = dialogBox->GetBounds();
        finalY = dlg.y - panelH - 20.f * scale;
        if (finalY < contentViewport_.y + 20.f * scale)
            finalY = contentViewport_.y + 20.f * scale;
    }

    choicePanel_->SetPosition(static_cast<int>(finalX),
                              static_cast<int>(finalY));
    choicePanel_->UpdateLayout();
}

gfx::Rect
GameWindow::CalculateCharacterPosition(const CharacterDef &def,
                                       const CharacterInstance &instance) {
    const std::vector<int> &usedOffset =
        instance.offset.empty() ? def.offset : instance.offset;

    float designX = 0.f;
    float designY = 300.f;

    if (def.base_pos == "left") {
        designX = 100.f + usedOffset[0];
        designY += usedOffset[1];
    } else if (def.base_pos == "right") {
        designX = 800.f + usedOffset[0];
        designY += usedOffset[1];
    } else {
        // center
        designX = 450.f + usedOffset[0];
        designY += usedOffset[1];
    }

    const float designW = 300.f;
    const float designH = 500.f;

    const float scale = contentViewport_.w / ViewW;

    const float x = contentViewport_.x + designX * scale;
    const float y = contentViewport_.y + designY * scale;
    const float w = designW * scale;
    const float h = designH * scale;

    return gfx::Rect::XYWH(x, y, w, h);
}

void GameWindow::ReverseDialogVisility() {
    for (auto &c : dialogControls) {
        c->SetVisible(!c->IsVisible());
    }
}

void GameWindow::SwitchBackground(const ImageHandle &bg) {
    auto fadeInBg = Bg1;
    auto fadeOutBg = Bg2;

    if (Bg1->GetAlpha() == 0) {
        Bg1->SetImage(bg);
        fadeInBg = Bg1;
        fadeOutBg = Bg2;
    } else {
        Bg2->SetImage(bg);
        fadeInBg = Bg2;
        fadeOutBg = Bg1;
    }

    fadePair(fadeOutBg, fadeInBg, 400);
}

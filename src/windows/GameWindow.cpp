#include "windows/GameWindow.h"
#include "gfx/Assets.h"
#include "windows/LoadSceneWindow.h"
#include "windows/SettingWindow.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <chrono>
#include <format>
#include <iostream>
#include <thread>

GameWindow::GameWindow()
    : tbk(std::make_shared<TextBlock>("", 24)),
      tbk2(std::make_shared<TextBlock>("", 28, Color{0, 0, 0, 255})),
      ic1(std::make_shared<ImageContainer>()),
      Bg1(std::make_shared<ImageContainer>()),
      Bg2(std::make_shared<ImageContainer>()) {
    InitializeUI();
}

void GameWindow::InitializeUI() {
    nextDialog = [this]() {
        if (!ic1->IsVisible()) {
            ReverseDialogVisility();
            return;
        }
        if (!scriptQueue.empty()) {
            SkipDialogueAnimation();
            scriptQueue.pop();
            ProcessNextItem();
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
    Bg1->SetRectF(RectF{0.f, 0.f, 1600.f, 900.f});
    Bg2->SetRectF(RectF{0.f, 0.f, 1600.f, 900.f});
    Bg1->SetMode(3);
    Bg2->SetMode(3);

    cp1 = std::make_shared<ClickPaper>(nextDialog);
    cp1->SetRect(Rect{0, 0, 1600, 900});

    ic1->SetRectF(RectF{0.f, 615.f, 1335.f, 285.f});
    ic1->SetMode(3);
    ic1->SetImage(Assets::LoadImage("assets/image/misc/footer.png"));

    tbk->SetPosition(45, 630);
    tbk->SetSize(400, 50);
    tbk->SetFontStyle(1);
    tbk->SetAlign(TextHAlign::Center, TextVAlign::Middle);

    tbk2->SetPosition(38, 690);
    tbk2->SetSize(1300, 185);
    tbk2->SetFontStyle(1);

    titletrs = std::make_shared<TitleTransition>();
    titletrs->SetVisible(false);

    lsw = std::make_shared<LoadSceneWindow>(
        [this]() {
            LoadScene(1);
            if (m_globalEvent)
                m_globalEvent->RequestWindowSwitch(1);
        },
        [this]() {
            LoadScene(2);
            if (m_globalEvent)
                m_globalEvent->RequestWindowSwitch(1);
        },
        [this]() {
            LoadScene(3);
            if (m_globalEvent)
                m_globalEvent->RequestWindowSwitch(1);
        },
        [this]() {
            LoadScene(4);
            if (m_globalEvent)
                m_globalEvent->RequestWindowSwitch(1);
        },
        [this]() {
            LoadScene(5);
            if (m_globalEvent)
                m_globalEvent->RequestWindowSwitch(1);
        },
        [this]() {
            if (m_globalEvent)
                m_globalEvent->RequestWindowSwitch(1);
        });

    btn_save = std::make_shared<PrimaryButton>("保存", [this]() {
        if (!m_globalEvent)
            return;
        auto sw = std::make_shared<SettingWindow>(
            [this]() {
                if (m_globalEvent)
                    m_globalEvent->RequestWindowSwitch(1);
            },
            "(ᗜˬᗜ) "
            "很遗憾，我没写这个\n这游戏要什么保存\n志乃：「你该不会是在想反正之"
            "后可"
            "以读档吧。这种想法志乃我可不喜欢」\n————『想要传达给你的爱恋』");
        m_globalEvent->RequestWindowSwitch(sw);
    });

    btn_load = std::make_shared<PrimaryButton>("读取", [this]() {
        if (!m_globalEvent)
            return;
        auto sw = std::make_shared<SettingWindow>(
            [this]() {
                if (m_globalEvent)
                    m_globalEvent->RequestWindowSwitch(1);
            },
            "(ᗜˬᗜ) 很遗憾，我没写这个\n这么短的剧情你就将就着看吧");
        m_globalEvent->RequestWindowSwitch(sw);
    });

    btn_qsave = std::make_shared<PrimaryButton>("快速保存", [this]() {
        if (!m_globalEvent)
            return;
        auto sw = std::make_shared<SettingWindow>(
            [this]() {
                if (m_globalEvent)
                    m_globalEvent->RequestWindowSwitch(1);
            },
            "(ᗜˬᗜ) 很遗憾，我没写这个\n这么短的剧情你就将就着看吧");
        m_globalEvent->RequestWindowSwitch(sw);
    });

    btn_qload = std::make_shared<PrimaryButton>("快速读取场景", [this]() {
        if (m_globalEvent)
            m_globalEvent->RequestWindowSwitch(lsw);
    });

    btn_autoplay = std::make_shared<PrimaryButton>("自动播放", [this]() {
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

    btn_setting = std::make_shared<PrimaryButton>("设定", [this]() {
        if (!m_globalEvent)
            return;
        auto sw = std::make_shared<SettingWindow>(
            [this]() {
                if (m_globalEvent)
                    m_globalEvent->RequestWindowSwitch(1);
            },
            "(ᗜˬᗜ) 很遗憾，我没写这个\n这游戏要什么设置，又没有奇奇怪怪的设置");
        m_globalEvent->RequestWindowSwitch(sw);
    });

    auto setupBtn = [](auto &b, int x, int y) {
        b->SetPosition(x, y);
        b->SetFontSize(16);
        b->SetFontStyle(1);
        b->SetSize(200, 30);
        b->SetCornerRadius(CornerRadius{35.f, 10.f, 10.f, 35.f});
    };

    setupBtn(btn_save, 1400, 670);
    setupBtn(btn_load, 1400, 705);
    setupBtn(btn_qsave, 1400, 740);
    setupBtn(btn_qload, 1400, 775);
    setupBtn(btn_autoplay, 1400, 810);
    setupBtn(btn_setting, 1400, 845);

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

    dialogControls = {ic1,       tbk,       tbk2,         btn_save,   btn_load,
                      btn_qsave, btn_qload, btn_autoplay, btn_setting};
}

void GameWindow::SetCharacterName(const std::string &name) {
    tbk->SetText(name);
}

void GameWindow::ShowDialogue(const std::string &dialogue) {
    tbk2->SetText(dialogue);
    tbk2->startAnimation(false);
}

void GameWindow::ShowDialogueImmediate(const std::string &dialogue) {
    tbk2->SetText(dialogue);
    tbk2->showFullText();
}

void GameWindow::ClearDialogue() { tbk2->clearText(); }

bool GameWindow::IsDialogueFinished() const { return !tbk2->IsAnimating(); }

void GameWindow::SkipDialogueAnimation() { tbk2->showFullText(); }

bool GameWindow::LoadScript(const std::string &filename) {
    return gameScript.loadFromFile(filename);
}

void GameWindow::LoadScene(int sceneId) {
    scriptQueue = std::queue<ScriptVariant>();
    ClearCharacters();

    const Chapter &chapter = gameScript.getChapter();

    for (const auto &scene : chapter.scenes) {
        if (scene.scene_id != sceneId)
            continue;

        ImageHandle bgImg = Assets::LoadImage(scene.background.c_str());
        if (!bgImg) {
            std::cout << "背景加载失败: " << scene.background << std::endl;
        } else {
            SwitchBackground(bgImg);
        }

        for (const auto &character : scene.characters) {
            const CharacterDef *def =
                gameScript.getCharacterDef(character.ref_name);
            if (!def || !def->hasSprites())
                continue;

            auto spIt = def->sprites.find(character.current_sprite);
            if (spIt == def->sprites.end())
                continue;

            ImageHandle charImg = Assets::LoadImage(spIt->second.c_str());
            if (!charImg)
                continue;

            auto container = std::make_shared<ImageContainer>();
            RectF charRect = CalculateCharacterPosition(*def, character);

            container->SetRectF(charRect);
            container->SetMode(3);
            container->SetImage(charImg);
            container->SetAlpha(0);

            characterContainers[character.ref_name] = container;
            AddMidgroundControl(container);
        }

        for (const auto &item : scene.scriptSequence) {
            scriptQueue.push(item);
        }

        ProcessNextItem();
        break;
    }
}

void GameWindow::ProcessNextItem() {
    if (scriptQueue.empty())
        return;

    const auto &item = scriptQueue.front();

    if (std::holds_alternative<Command>(item)) {
        try {
            ExecuteCommand(std::get<Command>(item));
            scriptQueue.pop();
        } catch (...) {
            scriptQueue.pop();
        }
        ProcessNextItem();
        return;
    }

    const Dialogue &dlg = std::get<Dialogue>(item);

    if (dlg.tag == DialogueType) {
        if (!dlg.displayname.empty())
            SetCharacterName(dlg.displayname);
        else
            SetCharacterName(dlg.character);

        ShowDialogue(std::format("「{}」", dlg.text));
    } else {
        SetCharacterName("");
        ShowDialogue(dlg.text);
    }

    if (auto it = characterContainers.find(dlg.character);
        it != characterContainers.end()) {
        it->second->SetAlpha(255);
    }

    if (!dlg.sprite.empty()) {
        UpdateCharacterSprite(dlg.character, dlg.sprite);
    }
    UpdateCharacterHighlight(dlg.character);
}

void GameWindow::ExecuteCommand(const Command &cmd) {
    if (!m_globalEvent)
        return;

    if (cmd.type == "sound") {
        if (cmd.soundtype == "bgm") {
            if (cmd.action == "play") {
                m_globalEvent->bgm.Load(cmd.param);
                m_globalEvent->bgm.Play(true);
            } else if (cmd.action == "pause") {
                m_globalEvent->bgm.Pause();
            } else if (cmd.action == "volumn") {
                m_globalEvent->bgm.SetVolume(std::stof(cmd.param));
            }
        } else if (cmd.soundtype == "voice") {
            if (cmd.action == "play") {
                m_globalEvent->voice.Load(cmd.param);
                m_globalEvent->voice.Play(false);
            } else if (cmd.action == "pause") {
                m_globalEvent->voice.Pause();
            } else if (cmd.action == "volumn") {
                m_globalEvent->voice.SetVolume(std::stof(cmd.param));
            }
        } else if (cmd.soundtype == "sfx") {
            if (cmd.action == "play") {
                m_globalEvent->sfx.Load(cmd.param);
                m_globalEvent->sfx.Play(false);
            } else if (cmd.action == "pause") {
                m_globalEvent->sfx.Pause();
            } else if (cmd.action == "volumn") {
                m_globalEvent->sfx.SetVolume(std::stof(cmd.param));
            }
        }
        return;
    }

    if (cmd.type == "bg") {
        if (cmd.action == "change") {
            ImageHandle img = Assets::LoadImage(cmd.param.c_str());
            if (img)
                SwitchBackground(img);
        } else if (cmd.action == "title") {
            titletrs->SetText(cmd.param);
            titletrs->SetVisible(true);
            std::thread([t = titletrs]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(4000));
                t->SetVisible(false);
            }).detach();
        }
        return;
    }

    if (cmd.type == "character") {
        if (cmd.action == "show") {
            if (auto it = characterContainers.find(cmd.param);
                it != characterContainers.end()) {
                it->second->SetAlpha(255);
                UpdateCharacterHighlight(cmd.param);
            }
        } else if (cmd.action == "hide") {
            if (auto it = characterContainers.find(cmd.param);
                it != characterContainers.end()) {
                it->second->SetAlpha(0);
            }
        }
        return;
    }

    if (cmd.type == "chapter") {
        if (cmd.action == "drop") {
            try {
                LoadScene(std::stoi(cmd.param));
            } catch (...) {
            }
        }
        return;
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

    const CharacterDef *def = gameScript.getCharacterDef(charName);
    if (!def)
        return;

    auto spIt = def->sprites.find(spriteName);
    if (spIt == def->sprites.end())
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

RectF GameWindow::CalculateCharacterPosition(
    const CharacterDef &def, const CharacterInstance &instance) {
    const std::vector<int> &usedOffset =
        instance.offset.empty() ? def.offset : instance.offset;

    float x = 0.f;
    float y = 300.f;

    if (def.base_pos == "left") {
        x = 100.f + usedOffset[0];
        y += usedOffset[1];
    } else if (def.base_pos == "right") {
        x = 800.f + usedOffset[0];
        y += usedOffset[1];
    } else {
        x = 450.f + usedOffset[0];
        y += usedOffset[1];
    }

    return RectF(x, y, 300.f, 500.f);
}

void GameWindow::ReverseDialogVisility() {
    for (auto it = dialogControls.rbegin(); it != dialogControls.rend(); ++it) {
        (*it)->SetVisible(!(*it)->IsVisible());
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

    std::thread([fadeOutBg, fadeInBg]() {
        constexpr int totalFadeTime = 400;
        constexpr int frameInterval = 16;

        auto start = std::chrono::steady_clock::now();
        int frame_count = 0;

        while (true) {
            auto elapsed =
                std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::steady_clock::now() - start)
                    .count();

            float progress = std::clamp(static_cast<float>(elapsed) /
                                            (totalFadeTime * 1000.0f),
                                        0.0f, 1.0f);

            float easeProgress = progress * progress;

            unsigned char alphaIn =
                static_cast<unsigned char>(255.0f * easeProgress);
            unsigned char alphaOut =
                static_cast<unsigned char>(255.0f * (1.0f - easeProgress));

            if (alphaIn != fadeInBg->GetAlpha() ||
                alphaOut != fadeOutBg->GetAlpha()) {
                fadeInBg->SetAlpha(alphaIn);
                fadeOutBg->SetAlpha(alphaOut);
            }

            if (progress >= 1.0f)
                break;

            auto target_time = start + std::chrono::milliseconds(frameInterval *
                                                                 ++frame_count);
            std::this_thread::sleep_until(target_time);
        }
    }).detach();
}

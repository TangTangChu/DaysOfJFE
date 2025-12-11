#pragma once

#include "windows/WindowPanel.h"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "gfx/geom/Geom.h"
#include "script/ScriptModel.h"
#include "script/ScriptRuntime.h"
#include "script/ScriptYamlLoader.h"
#include "ui/Button.h"
#include "ui/ClickPaper.h"
#include "ui/DialogBox.h"
#include "ui/ImageContainer.h"
#include "ui/Layout.h"
#include "ui/TextBlock.h"
#include "ui/TitleTransition.h"
#include "utils/Timer.h"
#include "windows/LoadSceneWindow.h"

class GameWindow : public WindowPanel {
  public:
    std::shared_ptr<DialogBox> dialogBox;
    std::shared_ptr<ClickPaper> cp1;

    std::shared_ptr<StackPanel> buttonStack;
    std::shared_ptr<PrimaryButton> btn_autoplay;

    std::shared_ptr<ImageContainer> Bg1;
    std::shared_ptr<ImageContainer> Bg2;

    std::shared_ptr<TitleTransition> titletrs;
    std::shared_ptr<LoadSceneWindow> lsw;

    std::function<void()> nextDialog;

    // 场景内角色立绘容器：脚本角色名 -> ImageContainer
    std::map<std::string, std::shared_ptr<ImageContainer>> characterContainers;

    // 选项 UI
    std::shared_ptr<StackPanel> choicePanel_;
    std::string currentChoiceId_;

    Timer timer;
    bool isautoplay = false;

    GameWindow();

    void OnWindowResize(int width, int height) override;

    void InitializeUI();

    void SetCharacterName(const std::string &name);
    void ShowDialogue(const std::string &dialogue);
    void ShowDialogueImmediate(const std::string &dialogue);
    void ClearDialogue();
    bool IsDialogueFinished() const;
    void SkipDialogueAnimation();

    void StartNewGame();

    void ReverseDialogVisility();

  private:
    void SetupSceneVisual(const script::Chapter &chapter,
                          const script::Scene &scene);

    void HandleBgCommand(const script::BgCommand &cmd);
    void HandleSoundCommand(const script::SoundCommand &cmd);
    void HandleCharacterCommand(const script::CharacterCommand &cmd);

    void UpdateCharacterHighlight(const std::string &speakingChar);
    void UpdateCharacterSprite(const std::string &charName,
                               const std::string &spriteName);
    void ClearCharacters();

    std::unique_ptr<script::ScriptRuntime> runtime_;
    std::vector<script::Chapter> chapters_;
    const script::Chapter *currentChapter_ = nullptr;

    int windowWidth_ = 1600;
    int windowHeight_ = 900;
    gfx::Rect contentViewport_{0.f, 0.f, 1600.f, 900.f};

    void SwitchBackground(const ImageHandle &bg);
    static constexpr float ViewW = 1600.f; // 设计分辨率宽
    static constexpr float ViewH = 900.f;  // 设计分辨率高

    std::vector<std::shared_ptr<Controls>> dialogControls;
    gfx::Rect
    CalculateCharacterPosition(const script::CharacterDef &def,
                               const script::CharacterInstance &instance);

    // 根据当前 contentViewport_ 和子元素大小，对选项面板进行布局
    void LayoutChoicePanel();
};

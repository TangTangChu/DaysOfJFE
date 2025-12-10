#pragma once
#include "WindowPanel.h"
#include <memory>
#include <queue>
#include "TextBlock.h"
#include "OutlineText.h"
#include "ImageContainer.h"
#include "GameScript.h"
#include "Button.h"
#include "ClickPaper.h"
#include <variant>
#include "TitleTransition.h"
#include "LoadSceneWindow.h"
#include "Timer.h"
#include <map>

class GameWindow : public WindowPanel {
public:
    // UI 组件
    std::shared_ptr<TextBlock> tbk;           // 角色名
    std::shared_ptr<OutlineText> tbk2;        // 对话文本
    std::shared_ptr<ImageContainer> ic1;      // 对话框背景
    std::shared_ptr<ClickPaper> cp1;          // 点击区域
    std::shared_ptr<PrimaryButton> btn_save;
    std::shared_ptr<PrimaryButton> btn_load;
    std::shared_ptr<PrimaryButton> btn_qsave;
    std::shared_ptr<PrimaryButton> btn_qload;
    std::shared_ptr<PrimaryButton> btn_autoplay;
    std::shared_ptr<PrimaryButton> btn_setting;
    std::shared_ptr<ImageContainer> Bg1;
    std::shared_ptr<ImageContainer> Bg2;
    std::shared_ptr<TitleTransition> titletrs;
    std::shared_ptr<LoadSceneWindow> lsw;
    std::shared_ptr<ImageContainer> transitionImage;  // 章节过渡图片

    // 对话系统
    GameScript gameScript;
    std::queue<std::variant<Command, Dialogue>> scriptQueue;
    Dialogue currentDialogue;
    bool isWaitingInput = false;
    std::function<void()> nextDialog;

    // 角色容器
    std::map<std::string, std::shared_ptr<ImageContainer>> characterContainers;

    Timer timer;
    bool isautoplay = false;

    GameWindow();

    void InitializeUI();
    void SetCharacterName(const std::string& name);
    void ShowDialogue(const std::string& dialogue);
    void ShowDialogueImmediate(const std::string& dialogue);
    void ClearDialogue();
    bool IsDialogueFinished() const;
    void SkipDialogueAnimation();

    bool LoadScript(const std::string& filename);
    void LoadScene(int sceneId);
    void ReverseDialogVisibility();
    void ProcessNextItem();
    void ExecuteCommand(const Command& cmd);
    
    // 窗口显示时的回调
    void OnShow() override;

private:
    bool sceneLoaded = false;
    void ShowCharacter(const std::string& nameOrId,
                       const std::string& sprite = "normal",
                       const std::vector<int>* offsetOverride = nullptr);
    void UpdateCharacterHighlight(const std::string& speakingChar);
    void UpdateCharacterSprite(const std::string& charName, const std::string& spriteName);
    void ClearCharacters();
    void SwitchBackground(sk_sp<SkImage> bg);
    
    std::vector<std::shared_ptr<Controls>> dialogControls;
    SkRect CalculateCharacterPosition(const CharacterDef& def, const CharacterInstance& instance);
};

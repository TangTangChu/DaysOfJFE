#pragma once
#include "windows/WindowPanel.h"
#include <functional>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <variant>
#include <vector>

#include "script/GameScript.h"
#include "ui/Button.h"
#include "ui/ClickPaper.h"
#include "ui/ImageContainer.h"
#include "ui/TextBlock.h"
#include "ui/TitleTransition.h"
#include "utils/Timer.h"
#include "windows/LoadSceneWindow.h"

class GameWindow : public WindowPanel {
  public:
    std::shared_ptr<TextBlock> tbk;
    std::shared_ptr<TextBlock> tbk2;
    std::shared_ptr<ImageContainer> ic1;
    std::shared_ptr<ClickPaper> cp1;

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

    GameScript gameScript;
    std::queue<ScriptVariant> scriptQueue;
    Dialogue currentDialogue;
    bool isWaitingInput = false;
    std::function<void()> nextDialog;

    std::map<std::string, std::shared_ptr<ImageContainer>> characterContainers;

    Timer timer;
    bool isautoplay = false;

    GameWindow();

    void InitializeUI();

    void SetCharacterName(const std::string &name);
    void ShowDialogue(const std::string &dialogue);
    void ShowDialogueImmediate(const std::string &dialogue);
    void ClearDialogue();
    bool IsDialogueFinished() const;
    void SkipDialogueAnimation();

    bool LoadScript(const std::string &filename);

    void LoadScene(int sceneId);
    void ReverseDialogVisility();
    void ProcessNextItem();
    void ExecuteCommand(const Command &cmd);

  private:
    void UpdateCharacterHighlight(const std::string &speakingChar);
    void UpdateCharacterSprite(const std::string &charName,
                               const std::string &spriteName);
    void ClearCharacters();
    void SwitchBackground(const ImageHandle &bg);
    static constexpr float ViewW = 1600.f;
    static constexpr float ViewH = 900.f;

    std::vector<std::shared_ptr<Controls>> dialogControls;
    gfx::Rect CalculateCharacterPosition(const CharacterDef &def,
                                         const CharacterInstance &instance);
};

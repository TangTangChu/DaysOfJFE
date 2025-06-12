#pragma once
#include "WindowPanel.h"
#include <memory>
#include <queue>
#include "TextBlock.h"
#include "ImageContainer.h"
#include "GameScript.h"
#include "Button.h"
#include "ClickPaper.h"
#include <variant>
#include "TitleTransition.h"
#include "LoadSceneWindow.h"
#include "Timer.h"
class GameWindow : public WindowPanel {
public:
	// UI组件
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
	// 对话系统

	GameScript gameScript;
	std::queue<std::variant<Command, Dialogue>> scriptQueue;
	Dialogue currentDialogue;
	bool isWaitingInput = false;
	std::function<void()> nextDialog;
	// 角色管理
	std::map<std::wstring, std::shared_ptr<ImageContainer>> characterContainers;


	Timer timer;
	bool isautoplay = false;
	GameWindow();

	void InitializeUI();
	void SetCharacterName(const std::wstring& name);
	void ShowDialogue(const std::wstring& dialogue);
	void ShowDialogueImmediate(const std::wstring& dialogue);
	void ClearDialogue();
	bool IsDialogueFinished() const;
	void SkipDialogueAnimation();

	bool LoadScript(const std::wstring& filename);

	void LoadScene(int sceneId);
	void ReverseDialogVisility();
	void ProcessNextItem();
	void ExecuteCommand(const Command& cmd);
private:

	void UpdateCharacterHighlight(const std::wstring& speakingChar);

	void UpdateCharacterSprite(const std::wstring& charName, const std::wstring& spriteName);

	void ClearCharacters();
	void SwitchBackground(std::unique_ptr<Image> bg);
	std::vector<std::shared_ptr<Controls>> dialogControls;
	RectF CalculateCharacterPosition(const CharacterDef& def, const CharacterInstance& instance);
};

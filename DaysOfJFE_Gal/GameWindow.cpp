#include "stdafx.h"
#include "GameWindow.h"
#include "SettingWindow.h"
#include <chrono>
#include <thread>
#include "Utils.h"
#include "TitleTransition.h"
#include <algorithm>
GameWindow::GameWindow() :
	tbk(std::make_shared<TextBlock>(L"", 24)),
	tbk2(std::make_shared<TextBlock>(L"", 28, Color(255, 0, 0, 0))),
	ic1(std::make_shared<ImageContainer>()),
	Bg1(std::make_shared<ImageContainer>()), Bg2(std::make_shared<ImageContainer>())
{
	InitializeUI();
}

void GameWindow::InitializeUI()
{
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
	OnMouseRBUPHook = [this]() {
		ReverseDialogVisility();
		};
	OnKeyDownHook = [this](ExMessage ex) {
		if (ex.vkcode == VK_ESCAPE) {
			ReverseDialogVisility();
		}
		else if (ex.vkcode == VK_SPACE || ex.vkcode == VK_RETURN) {
			nextDialog();
		}
		};
	Bg1->SetAlpha(0);
	Bg2->SetAlpha(255);
	Bg1->SetRectF(RectF{ 0.F,0.F,1600.F,900.F });
	Bg2->SetRectF(RectF{ 0.F,0.F,1600.F,900.F });
	Bg1->SetMode(3);
	Bg2->SetMode(3);
	cp1 = std::make_shared<ClickPaper>(nextDialog);
	cp1->SetRect(Rect{ 0,0,1600,900 });
	auto bottombg = std::make_unique<Image>(L"src/Image/misc/footer.png");
	RectF rectf2(0, 615, 1335, 285);
	ic1->SetRectF(rectf2);
	ic1->SetMode(3);
	ic1->SetImage(std::move(bottombg));

	tbk->SetPosition(45, 630);
	tbk->SetSize(400, 50);
	auto customFormat = std::make_unique<StringFormat>();
	customFormat->SetAlignment(StringAlignmentCenter);
	customFormat->SetLineAlignment(StringAlignmentCenter);
	tbk->SetFontStyle(1);
	tbk->SetStringFormat(std::move(customFormat));
	tbk2->SetPosition(38, 690);
	tbk2->SetSize(1300, 185);
	tbk2->SetFontStyle(1);
	titletrs = std::make_shared<TitleTransition>();
	titletrs->SetVisible(false);
	lsw = std::make_shared<LoadSceneWindow>([this]() {
		LoadScene(1);
		m_globalEvent->RequestWindowSwitch(1);
		},
		[this]() {
			LoadScene(2);
			m_globalEvent->RequestWindowSwitch(1);
		},
		[this]() {
			LoadScene(3);
			m_globalEvent->RequestWindowSwitch(1);
		},
		[this]() {
			LoadScene(4);
			m_globalEvent->RequestWindowSwitch(1);
		},
		[this]() {
			LoadScene(5);
			m_globalEvent->RequestWindowSwitch(1);
		},
		[this]() {m_globalEvent->RequestWindowSwitch(1); });
	btn_save = std::make_shared<PrimaryButton>(L"保存", [this]() {
		auto sw = std::make_shared<SettingWindow>([this]() {m_globalEvent->RequestWindowSwitch(1); }, L"(ᗜˬᗜ) 很遗憾，我没写这个\n这游戏要什么保存\n 志乃：「你该不会是在想反正之后可以读档吧。这种想法志乃我可不喜欢」 \n		————『想要传达给你的爱恋』");
		m_globalEvent->RequestWindowSwitch(sw);
		});
	btn_load = std::make_shared<PrimaryButton>(L"读取", [this]() {
		
		auto sw = std::make_shared<SettingWindow>([this]() {m_globalEvent->RequestWindowSwitch(1); }, L"(ᗜˬᗜ) 很遗憾，我没写这个\n这么短的剧情你就将就着看吧");
		m_globalEvent->RequestWindowSwitch(sw);
		});
	btn_qsave = std::make_shared<PrimaryButton>(L"快速保存", [this]() {
		auto sw = std::make_shared<SettingWindow>([this]() {m_globalEvent->RequestWindowSwitch(1); }, L"(ᗜˬᗜ) 很遗憾，我没写这个\n这么短的剧情你就将就着看吧");
		m_globalEvent->RequestWindowSwitch(sw);
		});
	btn_qload = std::make_shared<PrimaryButton>(L"快速读取场景", [this]() {
		m_globalEvent->RequestWindowSwitch(lsw);
		});
	btn_autoplay = std::make_shared<PrimaryButton>(L"自动播放", []() {});
	btn_setting = std::make_shared<PrimaryButton>(L"设定",
		[this]() {
			auto sw = std::make_shared<SettingWindow>([this]() {m_globalEvent->RequestWindowSwitch(1); }, L"(ᗜˬᗜ) 很遗憾，我没写这个\n这游戏要什么设置，又没有奇奇怪怪的设置");
			m_globalEvent->RequestWindowSwitch(sw);
		});

	btn_save->SetPosition(1400, 670);
	btn_save->SetFontSize(16);
	btn_save->SetFontStyle(1);
	btn_save->SetSize(200, 30);
	btn_save->SetCornerRadius(CornerRadius{ 35,10,10,35 });

	btn_load->SetPosition(1400, 705);
	btn_load->SetFontSize(16);
	btn_load->SetFontStyle(1);
	btn_load->SetSize(200, 30);
	btn_load->SetCornerRadius(CornerRadius{ 35,10,10,35 });

	btn_qsave->SetPosition(1400, 740);
	btn_qsave->SetFontSize(16);
	btn_qsave->SetFontStyle(1);
	btn_qsave->SetSize(200, 30);
	btn_qsave->SetCornerRadius(CornerRadius{ 35,10,10,35 });

	btn_qload->SetPosition(1400, 775);
	btn_qload->SetFontSize(16);
	btn_qload->SetFontStyle(1);
	btn_qload->SetSize(200, 30);
	btn_qload->SetCornerRadius(CornerRadius{ 35,10,10,35 });

	btn_autoplay->SetPosition(1400, 810);
	btn_autoplay->SetFontSize(16);
	btn_autoplay->SetFontStyle(1);
	btn_autoplay->SetSize(200, 30);
	btn_autoplay->SetCornerRadius(CornerRadius{ 35,10,10,35 });

	btn_setting->SetPosition(1400, 845);
	btn_setting->SetFontSize(16);
	btn_setting->SetFontStyle(1);
	btn_setting->SetSize(200, 30);
	btn_setting->SetCornerRadius(CornerRadius{ 35,10,10,35 });
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

void GameWindow::SetCharacterName(const std::wstring& name) { tbk->SetText(name); }
void GameWindow::ShowDialogue(const std::wstring& dialogue)
{
	tbk2->SetText(dialogue);
	tbk2->startAnimation(false);
}
void GameWindow::ShowDialogueImmediate(const std::wstring& dialogue)
{
	tbk2->SetText(dialogue);
	tbk2->showFullText();
}
void GameWindow::ClearDialogue() { tbk2->clearText(); }
bool GameWindow::IsDialogueFinished() const { return !tbk2->IsAnimating(); }
void GameWindow::SkipDialogueAnimation() { tbk2->showFullText(); }

bool GameWindow::LoadScript(const std::wstring& filename)
{
	return gameScript.loadFromFile(filename);
}

void GameWindow::LoadScene(int sceneId)
{
	scriptQueue = std::queue<std::variant<Command, Dialogue>>(); // 清空队列
	ClearCharacters();

	const Chapter& chapter = gameScript.getChapter();
	std::wcout << L"加载场景ID: " << sceneId << std::endl;

	for (const auto& scene : chapter.scenes)
	{
		if (scene.scene_id == sceneId)
		{
			auto img = std::make_unique<Image>(scene.background.c_str());
			if (!img || !img->GetWidth())
			{
				std::wcout << L"背景加载失败: " << scene.background << std::endl;
			}
			else
			{
				SwitchBackground(std::move(img));
				std::wcout << L"背景加载成功 : " << scene.background << std::endl;
			}

			std::wcout << L"开始读取立绘 (角色数量: " << scene.characters.size() << ")" << std::endl;

			for (const auto& character : scene.characters)
			{
				std::wcout << std::format(L"正在处理角色: {}", character.ref_name) << std::endl;

				const CharacterDef* def = gameScript.getCharacterDef(character.ref_name);
				if (!def)
				{
					std::wcout << L"角色定义未找到: " << character.ref_name << std::endl;
					continue;
				}
				if (!def->hasSprites())

				{
					std::wcout << L"跳过无立绘角色" << std::endl;
					continue;
				}
				try
				{
					std::wstring spritePath = def->sprites.at(character.current_sprite);
					std::wcout << L"读取立绘: " << spritePath << std::endl;

					auto charImage = std::make_unique<Image>(spritePath.c_str());
					if (!charImage || !charImage->GetWidth())
					{
						std::wcout << L"立绘加载失败" << std::endl;
						continue;
					}

					auto container = std::make_shared<ImageContainer>();
					RectF charRect = CalculateCharacterPosition(*def, character);
					std::wcout << L"角色位置: X=" << charRect.X << L" Y=" << charRect.Y
						<< L" W=" << charRect.Width << L" H=" << charRect.Height << std::endl;

					container->SetRectF(charRect);
					container->SetMode(3);
					container->SetImage(std::move(charImage));
					container->SetAlpha(0);

					characterContainers[character.ref_name] = container;
					AddControl(container);
					std::wcout << L"角色加载成功" << std::endl;
				}
				catch (const std::exception& e)
				{
					std::wcout << L"异常: " << e.what() << std::endl;
				}
			}

			// 加载对话和命令
			for (const auto& item : scene.scriptSequence) {
				scriptQueue.push(item);
			}

			ProcessNextItem(); // 改为新的处理方法
			break;
		}
	}
	std::wcout << L"==== 场景加载完成 ====" << std::endl;
}
void GameWindow::ProcessNextItem() {
	if (scriptQueue.empty()) return;

	const auto& item = scriptQueue.front();
	if (std::holds_alternative<Command>(item)) {
		try {
			ExecuteCommand(std::get<Command>(item));
			scriptQueue.pop();
		}
		catch (...) {
			scriptQueue.pop();
			std::cerr << "Command执行失败" << std::endl;
		}
		ProcessNextItem();
	}
	else {
		const Dialogue& dlg = std::get<Dialogue>(item);
		if (dlg.tag == DialogueType) {
			if (dlg.displayname.size()!=0) {
				SetCharacterName(dlg.displayname);
			}
			else {
				SetCharacterName(dlg.character);
			}
			ShowDialogue(std::format(L"「{}」", dlg.text));
		}
		else {
			SetCharacterName(L"");
			ShowDialogue(dlg.text);
		}

		auto it = characterContainers.find(dlg.character);
		if (it != characterContainers.end()) {
			it->second->SetAlpha(255);
		}

		if (!dlg.sprite.empty()) {
			UpdateCharacterSprite(dlg.character, dlg.sprite);
		}
		UpdateCharacterHighlight(dlg.character);
	}
}
void GameWindow::ExecuteCommand(const Command& cmd) {
	if (cmd.type == L"sound") {
		if (cmd.soundtype == L"bgm") {
			//m_globalEvent->bgm.Load(Utils::wideToUtf8(cmd.param));
			if (cmd.action == L"play") {
				m_globalEvent->bgm.Load(cmd.param);
				m_globalEvent->bgm.Play(true);
			}
			else if (cmd.action == L"pause") {
				m_globalEvent->bgm.Pause();
			}
			else if (cmd.action == L"volumn") {
				m_globalEvent->bgm.SetVolume(std::stof(cmd.param));
			}
		}
		else if (cmd.soundtype == L"voice") {
			if (cmd.action == L"play") {
				m_globalEvent->voice.Load(cmd.param);
				m_globalEvent->voice.Play(false);
			}
			else if (cmd.action == L"pause") {
				m_globalEvent->voice.Pause();
			}
			else if (cmd.action == L"volumn") {
				m_globalEvent->voice.SetVolume(std::stof(cmd.param));
			}
		}
		else if (cmd.soundtype == L"sfx") {
			if (cmd.action == L"play") {
				m_globalEvent->sfx.Load(cmd.param);
				m_globalEvent->sfx.Play(false);
			}
			else if (cmd.action == L"pause") {
				m_globalEvent->sfx.Pause();
			}
			else if (cmd.action == L"volumn") {
				m_globalEvent->sfx.SetVolume(std::stof(cmd.param));
			}
		}
		
		
	}
	else if (cmd.type == L"bg") {
		if (cmd.action == L"change") {
			std::cout << cmd.param.size() << std::endl;
			auto img = std::make_unique<Image>(cmd.param.c_str());
			SwitchBackground(std::move(img));
		}
		else if (cmd.action == L"title") {
			titletrs->SetText(cmd.param);
			titletrs->SetVisible(true);
			std::thread([this]() {Sleep(4000);
			titletrs->SetVisible(false); }).detach();
			
		}
	}
	else if (cmd.type == L"character") {
		if (cmd.action == L"show") {
			if (auto it = characterContainers.find(cmd.param); it != characterContainers.end()) {
				it->second->SetAlpha(255); // 强制高亮
				UpdateCharacterHighlight(cmd.param); // 维持高亮
			}
		}
		else if (cmd.action == L"hide") {
			if (auto it = characterContainers.find(cmd.param); it != characterContainers.end()) {
				it->second->SetAlpha(0); // 强制隐藏
			}
		}
	}
	else if (cmd.type == L"chapter") {
		if (cmd.action == L"drop") {
			try {
				LoadScene(std::stoi(cmd.param));
			}
			catch (const std::exception& e) {
				std::cerr << "转换出错: " << e.what() << std::endl;
			}

		}

	}
}

void GameWindow::UpdateCharacterHighlight(const std::wstring& speakingChar) {
	for (auto& pair : characterContainers) {
		if (pair.first == speakingChar) {
			if (pair.second->GetAlpha() == 0) {
				pair.second->SetAlpha(255); // 从隐藏状态首次显示
			}
		}
		else {
			// 非说话角色：如果已显示则变暗，保持隐藏状态的不变
			if (pair.second->GetAlpha() > 0) {
				pair.second->SetAlpha(150);
			}
		}
	}
}

void GameWindow::UpdateCharacterSprite(const std::wstring& charName, const std::wstring& spriteName)
{
	auto it = characterContainers.find(charName);
	if (it == characterContainers.end())
		return;

	const CharacterDef* def = gameScript.getCharacterDef(charName);
	if (!def || def->sprites.find(spriteName) == def->sprites.end())
		return;

	std::wstring spritePath = def->sprites.at(spriteName);
	auto newImage = std::make_unique<Image>(spritePath.c_str());
	it->second->SetImage(std::move(newImage));
}

void GameWindow::ClearCharacters()
{
	for (auto& pair : characterContainers)
	{
		RemoveControl(pair.second);
	}
	characterContainers.clear();
}

RectF GameWindow::CalculateCharacterPosition(const CharacterDef& def, const CharacterInstance& instance)
{
	// 优先使用实例的offset，没有则用定义的offset
	const std::vector<int>& usedOffset = instance.offset.empty() ? def.offset : instance.offset;

	float x = 0, y = 300; 

	if (def.base_pos == L"left")
	{
		x = 100 + usedOffset[0];
		y += usedOffset[1];
	}
	else if (def.base_pos == L"right")
	{
		x = 800 + usedOffset[0];
		y += usedOffset[1];
	}
	else
	{ // center
		x = 450 + usedOffset[0];
		y += usedOffset[1];
	}

	std::cout << "计算位置: (" << x << "," << y << ")" << std::endl;
	return RectF(x, y, 300, 500);
}

void GameWindow::ReverseDialogVisility() {
	for (auto it = dialogControls.rbegin(); it != dialogControls.rend(); ++it) {
		(*it)->SetVisible(!(*it)->IsVisible());
	}
}

void GameWindow::SwitchBackground(std::unique_ptr<Image> bg) {
	auto fadeInBg = std::make_shared<ImageContainer>();
	auto fadeOutBg = std::make_shared<ImageContainer>();
	if (Bg1->GetAlpha() == 0) {
		Bg1->SetImage(std::move(bg));
		fadeInBg = Bg1;
		fadeOutBg = Bg2;
	}
	else {
		Bg2->SetImage(std::move(bg));
		fadeInBg = Bg2;
		fadeOutBg = Bg1;
	}
	std::thread([fadeOutBg, fadeInBg]() {
		constexpr int totalFadeTime = 400;  // 总时间(ms)
		constexpr int frameInterval = 16;      // ~60fps (16ms/帧)
		auto start = std::chrono::steady_clock::now();
		int frame_count = 0;

		while (true) {
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::steady_clock::now() - start).count();
			float progress = std::clamp(
				static_cast<float>(elapsed) / (totalFadeTime * 1000.0f),
				0.0f,
				1.0f
			);

			float easeProgress = progress * progress;

			BYTE alphaIn = static_cast<BYTE>(255.0f * easeProgress);
			BYTE alphaOut = static_cast<BYTE>(255.0f * (1.0f - easeProgress));

			if (alphaIn != fadeInBg->GetAlpha() || alphaOut != fadeOutBg->GetAlpha()) {
				fadeInBg->SetAlpha(alphaIn);
				fadeOutBg->SetAlpha(alphaOut);
			}

			if (progress >= 1.0f) break;

			auto target_time = start + std::chrono::milliseconds(frameInterval * ++frame_count);
			std::this_thread::sleep_until(target_time);
		}
		}).detach();
}
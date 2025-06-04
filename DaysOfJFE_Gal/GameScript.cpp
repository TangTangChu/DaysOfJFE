#include "stdafx.h"
#include "GameScript.h"
#include "Utils.h"

Dialogue GameScript::ParseDialogueNode(const YAML::Node& node,
	const std::map<std::wstring, std::wstring>& id_to_name) {
	Dialogue dialogue;
	
	if (node["type"].as<std::string>() == "dialogue") {
		// ID转换
		std::wstring speakerId = Utils::utf8ToWide(node["speaker"].as<std::string>());
		auto it = id_to_name.find(speakerId);
		dialogue.character = (it != id_to_name.end()) ? it->second : speakerId;
		if (node["sprite"]) {
			dialogue.sprite = Utils::utf8ToWide(node["sprite"].as<std::string>());
		}

		dialogue.tag = DialogueType;
	}
	else {
		dialogue.tag = NarrationType;
	}
	if (node["displayname"]) {
		dialogue.displayname = Utils::utf8ToWide(node["displayname"].as<std::string>());
	}

	dialogue.text = Utils::utf8ToWide(node["text"].as<std::string>());

	return dialogue;
}

Command GameScript::ParseCommandNode(const YAML::Node& node) {
	Command cmd;
	cmd.type = Utils::utf8ToWide(node["cmd"].as<std::string>());
	cmd.action = Utils::utf8ToWide(node["action"].as<std::string>());
	if (node["param"]) {
		cmd.param = Utils::utf8ToWide(node["param"].as<std::string>());
	}
	if (node["soundtype"]) {
		cmd.soundtype= Utils::utf8ToWide(node["soundtype"].as<std::string>());
	}
	return cmd;
}

bool GameScript::loadFromFile(const std::wstring& filename) {
	try
	{
		std::string narrowFilename = Utils::wideToUtf8(filename);
		YAML::Node config = YAML::LoadFile(narrowFilename);

		// 解析角色定义库
		if (config["characters"])
		{
			id_to_name_.clear();
			for (const auto& it : config["characters"]) {
				std::wstring name = Utils::utf8ToWide(it.first.as<std::string>());
				auto& def = chapter_.character_defs[name]; // 直接引用
				def = parseCharacterDef(it.second);

				if (!def.id.empty()) {
					id_to_name_.emplace(def.id, name);
				}
			}
		}

		if (config["chapter"]["title"])
		{
			chapter_.title = Utils::utf8ToWide(config["chapter"]["title"].as<std::string>());
		}

		// 解析场景
		if (config["chapter"]["scenes"])
		{
			for (const auto& sceneNode : config["chapter"]["scenes"])
			{
				Scene scene;
				scene.scene_id = sceneNode["scene_id"].as<int>(0);
				scene.background = Utils::utf8ToWide(sceneNode["background"].as<std::string>(""));

				// 解析角色实例
				if (sceneNode["characters"])
				{
					for (const auto& charInstNode : sceneNode["characters"])
					{
						scene.characters.push_back(parseCharacterInstance(charInstNode));
					}
				}
				// 解析对话和命令
				if (sceneNode["dialogues"]) {
					for (const auto& item : sceneNode["dialogues"]) {
						if (item["type"].as<std::string>() == "dialogue" || item["type"].as<std::string>() == "narration") {

							scene.scriptSequence.push_back(ParseDialogueNode(item, id_to_name_));
						}
						else if (item["type"].as<std::string>() == "command") {
							scene.scriptSequence.push_back(ParseCommandNode(item));
						}

						/*if (item["speaker"]) {
							scene.scriptSequence.push_back(ParseDialogueNode(item, id_to_name_));
						}
						else if (item["cmd"]) {
							scene.scriptSequence.push_back(ParseCommandNode(item));
						}*/
					}
				}

				chapter_.scenes.push_back(scene);
			}
		}
		return true;
	}
	catch (const YAML::Exception& e)
	{
		std::cerr << "YAML解析错误: " << e.what() << std::endl;
		return false;
	}
	catch (...)
	{
		std::cerr << "未知错误" << std::endl;
		return false;
	}
}

const CharacterDef* GameScript::getCharacterDefById(const std::wstring& id) const
{
	auto it = id_to_name_.find(id);
	return it != id_to_name_.end() ? getCharacterDef(it->second) : nullptr;
}

const Chapter& GameScript::getChapter() const { return chapter_; }

const CharacterDef* GameScript::getCharacterDef(const std::wstring& name) const
{
	auto it = chapter_.character_defs.find(name);
	return it != chapter_.character_defs.end() ? &it->second : nullptr;
}

// 解析辅助函数
CharacterDef GameScript::parseCharacterDef(const YAML::Node& node) {
	CharacterDef def;

	if (node["id"]) def.id = Utils::utf8ToWide(node["id"].as<std::string>());
	if (node["base_pos"]) def.base_pos = Utils::utf8ToWide(node["base_pos"].as<std::string>());
	if (node["offset"]) def.offset = node["offset"].as<std::vector<int>>();

	if (node["sprites"]) {
		for (const auto& sprite : node["sprites"]) {
			def.sprites.emplace(
				Utils::utf8ToWide(sprite.first.as<std::string>()),
				Utils::utf8ToWide(sprite.second.as<std::string>())
			);
		}
	}
	return def;
}

CharacterInstance GameScript::parseCharacterInstance(const YAML::Node& node)
{
	CharacterInstance instance;

	instance.ref_name = Utils::utf8ToWide(node["name"].as<std::string>());

	if (node["sprite"])
	{
		instance.current_sprite = Utils::utf8ToWide(node["sprite"].as<std::string>());
	}
	if (node["offset"])
	{
		instance.offset = node["offset"].as<std::vector<int>>();
	}

	return instance;
}
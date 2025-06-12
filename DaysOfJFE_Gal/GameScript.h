#pragma once
#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <Windows.h>
#include <variant>
enum DialogueType
{
	DialogueType,
	NarrationType
};
struct Command;
struct Dialogue;

// 角色定义结构体
struct CharacterDef {
	std::wstring id;
	std::wstring base_pos = L"left";
	std::vector<int> offset = { 0, 0 };
	std::map<std::wstring, std::wstring> sprites; // 表情映射
	bool hasSprites() const { return !sprites.empty(); }
};

// 角色实例结构体
struct CharacterInstance {
	std::wstring ref_name;      // 引用的角色名
	std::wstring current_sprite = L"normal";
	std::vector<int> offset;    // 可覆盖的偏移
};

// 对话结构体
struct Dialogue {
	std::wstring character;     // 角色名或引用
	std::wstring displayname;
	std::wstring text;
	std::wstring sprite;       // 表情
	int tag;
};

// 命令结构体
struct Command {
	std::wstring type;
	std::wstring action;
	std::wstring param;
	std::wstring soundtype;
};
using ScriptVariant = std::variant<Command, Dialogue>;

// 场景结构体
struct Scene {
	int scene_id;
	std::wstring background;
	std::vector<CharacterInstance> characters;

	std::vector<ScriptVariant> scriptSequence;
};

// 章节结构体
struct Chapter {
	std::wstring title;
	std::map<std::wstring, CharacterDef> character_defs; // 角色定义库
	std::vector<Scene> scenes;
};

class GameScript {
public:
	GameScript() = default;
	~GameScript() = default;

	bool loadFromFile(const std::wstring& filename);
	const CharacterDef* getCharacterDefById(const std::wstring& id) const;

	const Chapter& getChapter() const;

	const CharacterDef* getCharacterDef(const std::wstring& name) const;
	Dialogue ParseDialogueNode(const YAML::Node& node,
		const std::map<std::wstring, std::wstring>& id_to_name);
	Command ParseCommandNode(const YAML::Node& node);
private:
	Chapter chapter_;

	std::map<std::wstring, std::wstring> id_to_name_;

	CharacterDef parseCharacterDef(const YAML::Node& node);
	CharacterInstance parseCharacterInstance(const YAML::Node& node);
};

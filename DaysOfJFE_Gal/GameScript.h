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

// ��ɫ����ṹ��
struct CharacterDef {
	std::wstring id;
	std::wstring base_pos = L"left";
	std::vector<int> offset = { 0, 0 };
	std::map<std::wstring, std::wstring> sprites; // ����ӳ��
	bool hasSprites() const { return !sprites.empty(); }
};

// ��ɫʵ���ṹ��
struct CharacterInstance {
	std::wstring ref_name;      // ���õĽ�ɫ��
	std::wstring current_sprite = L"normal";
	std::vector<int> offset;    // �ɸ��ǵ�ƫ��
};

// �Ի��ṹ��
struct Dialogue {
	std::wstring character;     // ��ɫ��������
	std::wstring displayname;
	std::wstring text;
	std::wstring sprite;       // ����
	int tag;
};

// ����ṹ��
struct Command {
	std::wstring type;
	std::wstring action;
	std::wstring param;
	std::wstring soundtype;
};
using ScriptVariant = std::variant<Command, Dialogue>;

// �����ṹ��
struct Scene {
	int scene_id;
	std::wstring background;
	std::vector<CharacterInstance> characters;

	std::vector<ScriptVariant> scriptSequence;
};

// �½ڽṹ��
struct Chapter {
	std::wstring title;
	std::map<std::wstring, CharacterDef> character_defs; // ��ɫ�����
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

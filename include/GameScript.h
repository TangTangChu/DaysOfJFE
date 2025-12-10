#pragma once
#include <yaml-cpp/yaml.h>
#include <vector>
#include <string>
#include <map>
#include <variant>

/**
 * @file GameScript.h
 * @brief 游戏脚本解析器 - 支持YAML格式的对话和命令
 */

enum DialogueType {
    DialogueType,
    NarrationType
};

struct Command;
struct Dialogue;

// 角色定义结构体
struct CharacterDef {
    std::string id;
    std::string base_pos = "left";
    std::vector<int> offset = { 0, 0 };
    std::map<std::string, std::string> sprites; // 表情映射
    bool hasSprites() const { return !sprites.empty(); }
};

// 角色实例结构体
struct CharacterInstance {
    std::string ref_name;      // 引用的角色名
    std::string current_sprite = "normal";
    std::vector<int> offset;    // 可覆盖的偏移
};

// 对话结构体
struct Dialogue {
    std::string character;     // 角色名（解析后）
    std::string displayname;
    std::string text;
    std::string sprite;       // 表情
    int tag;
};

// 命令结构体
struct Command {
    std::string type;
    std::string action;
    std::string param;
    std::string soundtype;
};

using ScriptVariant = std::variant<Command, Dialogue>;

// 场景结构体
struct Scene {
    int scene_id;
    std::string background;
    std::vector<CharacterInstance> characters;
    std::vector<ScriptVariant> scriptSequence;
};

// 章节结构体
struct Chapter {
    std::string title;
    std::map<std::string, CharacterDef> character_defs; // 角色定义表
    std::vector<Scene> scenes;
};

class GameScript {
public:
    GameScript() = default;
    ~GameScript() = default;

    bool LoadFromFile(const std::string& filename);
    const CharacterDef* GetCharacterDefById(const std::string& id) const;
    const Chapter& GetChapter() const;
    const CharacterDef* GetCharacterDef(const std::string& name) const;
    
    Dialogue ParseDialogueNode(const YAML::Node& node,
        const std::map<std::string, std::string>& id_to_name);
    Command ParseCommandNode(const YAML::Node& node);

private:
    Chapter chapter_;
    std::map<std::string, std::string> id_to_name_;

    CharacterDef ParseCharacterDefNode(const YAML::Node& node);
    CharacterInstance ParseCharacterInstanceNode(const YAML::Node& node);
};

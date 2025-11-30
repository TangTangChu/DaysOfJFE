#pragma once
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <yaml-cpp/yaml.h>

enum DialogueType { DialogueType, NarrationType };

struct Command;
struct Dialogue;

// 角色定义结构体
struct CharacterDef {
    std::string id;
    std::string base_pos = "left";
    std::vector<int> offset = {0, 0};
    std::map<std::string, std::string> sprites;
    bool hasSprites() const { return !sprites.empty(); }
};

// 角色实例结构体
struct CharacterInstance {
    std::string ref_name;
    std::string current_sprite = "normal";
    std::vector<int> offset;
};

// 对话结构体
struct Dialogue {
    std::string character;
    std::string displayname;
    std::string text;
    std::string sprite;
    int tag = NarrationType;
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
    int scene_id = 0;
    std::string background;
    std::vector<CharacterInstance> characters;
    std::vector<ScriptVariant> scriptSequence;
};

// 章节结构体
struct Chapter {
    std::string title;
    std::map<std::string, CharacterDef> character_defs;
    std::vector<Scene> scenes;
};

class GameScript {
  public:
    GameScript() = default;
    ~GameScript() = default;

    bool loadFromFile(const std::string &filename);

    const CharacterDef *getCharacterDefById(const std::string &id) const;
    const Chapter &getChapter() const;
    const CharacterDef *getCharacterDef(const std::string &name) const;

    Dialogue
    ParseDialogueNode(const YAML::Node &node,
                      const std::map<std::string, std::string> &id_to_name);
    Command ParseCommandNode(const YAML::Node &node);

  private:
    Chapter chapter_;
    std::map<std::string, std::string> id_to_name_;

    CharacterDef parseCharacterDef(const YAML::Node &node);
    CharacterInstance parseCharacterInstance(const YAML::Node &node);
};

#include "GameScript.h"
#include <iostream>

Dialogue GameScript::ParseDialogueNode(const YAML::Node& node,
    const std::map<std::string, std::string>& id_to_name) {
    Dialogue dialogue;

    if (node["type"].as<std::string>() == "dialogue") {
        // ID转换
        std::string speakerId = node["speaker"].as<std::string>();
        auto it = id_to_name.find(speakerId);
        dialogue.character = (it != id_to_name.end()) ? it->second : speakerId;
        
        if (node["sprite"]) {
            dialogue.sprite = node["sprite"].as<std::string>();
        }

        dialogue.tag = DialogueType;
    } else {
        dialogue.tag = NarrationType;
    }
    
    if (node["displayname"]) {
        dialogue.displayname = node["displayname"].as<std::string>();
    }

    dialogue.text = node["text"].as<std::string>();

    return dialogue;
}

Command GameScript::ParseCommandNode(const YAML::Node& node) {
    Command cmd;
    cmd.type = node["cmd"].as<std::string>();
    cmd.action = node["action"].as<std::string>();
    
    if (node["param"]) {
        cmd.param = node["param"].as<std::string>();
    }
    if (node["soundtype"]) {
        cmd.soundtype = node["soundtype"].as<std::string>();
    }
    
    return cmd;
}

bool GameScript::LoadFromFile(const std::string& filename) {
    try {
        YAML::Node config = YAML::LoadFile(filename);

        // 解析角色定义表
        if (config["characters"]) {
            id_to_name_.clear();
            for (const auto& it : config["characters"]) {
                std::string name = it.first.as<std::string>();
                auto& def = chapter_.character_defs[name];
                def = ParseCharacterDefNode(it.second);

                if (!def.id.empty()) {
                    id_to_name_.emplace(def.id, name);
                }
            }
        }

        if (config["chapter"]["title"]) {
            chapter_.title = config["chapter"]["title"].as<std::string>();
        }

        // 解析场景
        if (config["chapter"]["scenes"]) {
            for (const auto& sceneNode : config["chapter"]["scenes"]) {
                Scene scene;
                scene.scene_id = sceneNode["scene_id"].as<int>(0);
                scene.background = sceneNode["background"].as<std::string>("");

                // 解析角色实例
                if (sceneNode["characters"]) {
                    for (const auto& charInstNode : sceneNode["characters"]) {
                        scene.characters.push_back(ParseCharacterInstanceNode(charInstNode));
                    }
                }
                
                // 解析对话和命令
                if (sceneNode["dialogues"]) {
                    for (const auto& item : sceneNode["dialogues"]) {
                        std::string itemType = item["type"].as<std::string>();
                        
                        if (itemType == "dialogue" || itemType == "narration") {
                            scene.scriptSequence.push_back(ParseDialogueNode(item, id_to_name_));
                        } else if (itemType == "command") {
                            scene.scriptSequence.push_back(ParseCommandNode(item));
                        }
                    }
                }

                chapter_.scenes.push_back(scene);
            }
        }
        
        std::cout << "脚本加载成功: " << filename << std::endl;
        return true;
    }
    catch (const YAML::Exception& e) {
        std::cerr << "YAML 解析错误: " << e.what() << std::endl;
        return false;
    }
    catch (...) {
        std::cerr << "未知错误" << std::endl;
        return false;
    }
}

const CharacterDef* GameScript::GetCharacterDefById(const std::string& id) const {
    auto it = id_to_name_.find(id);
    return it != id_to_name_.end() ? GetCharacterDef(it->second) : nullptr;
}

const Chapter& GameScript::GetChapter() const { 
    return chapter_; 
}

const CharacterDef* GameScript::GetCharacterDef(const std::string& name) const {
    auto it = chapter_.character_defs.find(name);
    return it != chapter_.character_defs.end() ? &it->second : nullptr;
}

CharacterDef GameScript::ParseCharacterDefNode(const YAML::Node& node) {
    CharacterDef def;

    if (node["id"]) {
        def.id = node["id"].as<std::string>();
    }
    if (node["base_pos"]) {
        def.base_pos = node["base_pos"].as<std::string>();
    }
    if (node["offset"]) {
        def.offset = node["offset"].as<std::vector<int>>();
    }

    if (node["sprites"]) {
        for (const auto& sprite : node["sprites"]) {
            def.sprites.emplace(
                sprite.first.as<std::string>(),
                sprite.second.as<std::string>()
            );
        }
    }
    
    return def;
}

CharacterInstance GameScript::ParseCharacterInstanceNode(const YAML::Node& node) {
    CharacterInstance instance;

    instance.ref_name = node["name"].as<std::string>();

    if (node["sprite"]) {
        instance.current_sprite = node["sprite"].as<std::string>();
    }
    if (node["offset"]) {
        instance.offset = node["offset"].as<std::vector<int>>();
    }

    return instance;
}

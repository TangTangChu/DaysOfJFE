#pragma once

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <windows.h>
#include "yaml-cpp/yaml.h"
struct Dialogue {
    std::wstring character;
    std::wstring text;
    std::wstring effect;
};

struct Character {
    std::wstring name;
    std::wstring sprite;
    std::wstring base_position;
    std::vector<int> offset;
};

struct Scene {
    int scene_id;
    std::wstring background;
    std::vector<Character> characters;
    std::vector<Dialogue> dialogues;
};

struct Chapter {
    std::wstring title;
    std::vector<Scene> scenes;
};
class GameScriptParser {
public:
    bool LoadFromFile(const std::wstring& filename) {
        try {
            int size_needed = WideCharToMultiByte(
                CP_UTF8, 0,
                filename.c_str(), (int)filename.size(),
                nullptr, 0, nullptr, nullptr);

            std::string str(size_needed, 0);
            WideCharToMultiByte(
                CP_UTF8, 0,
                filename.c_str(), (int)filename.size(),
                &str[0], size_needed, nullptr, nullptr);

            YAML::Node config = YAML::LoadFile(str);

            chapter_.title = config["chapter"]["title"].as<std::wstring>();

            const YAML::Node& scenes = config["chapter"]["scenes"];
            for (const auto& sceneNode : scenes) {
                Scene scene;
                scene.scene_id = sceneNode["scene_id"].as<int>();
                scene.background = sceneNode["background"].as<std::wstring>();

                for (const auto& charNode : sceneNode["characters"]) {
                    Character character;
                    character.name = charNode["name"].as<std::wstring>();

                    if (charNode["sprite"]) {
                        character.sprite = charNode["sprite"].as<std::wstring>();
                    }

                    character.base_position = charNode["base_position"].as<std::wstring>();
                    character.offset = charNode["offset"].as<std::vector<int>>();
                    scene.characters.push_back(character);
                }

                for (const auto& dialogueNode : sceneNode["dialogues"]) {
                    Dialogue dialogue;
                    dialogue.character = dialogueNode["character"].as<std::wstring>();
                    dialogue.text = dialogueNode["text"].as<std::wstring>();

                    if (dialogueNode["effect"]) {
                        dialogue.effect = dialogueNode["effect"].as<std::wstring>();
                    }

                    scene.dialogues.push_back(dialogue);
                }

                chapter_.scenes.push_back(scene);
            }

            return true;
        }
        catch (const YAML::Exception& e) {
            std::cerr << "YAML parsing error: " << e.what() << std::endl;
            return false;
        }
    }

    const Chapter& GetChapter() const {
        return chapter_;
    }

private:
    Chapter chapter_;
};
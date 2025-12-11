#include "script/ScriptYamlLoader.h"

#include <filesystem>
#include <iostream>
#include <yaml-cpp/yaml.h>


namespace fs = std::filesystem;

namespace script {

namespace {

CharacterDef ParseCharacterDef(const YAML::Node &node) {
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
        for (const auto &it : node["sprites"]) {
            def.sprites.emplace(it.first.as<std::string>(),
                                it.second.as<std::string>());
        }
    }
    return def;
}

CharacterInstance ParseCharacterInstance(const YAML::Node &node) {
    CharacterInstance inst;
    inst.ref_name = node["name"].as<std::string>();
    if (node["sprite"]) {
        inst.sprite = node["sprite"].as<std::string>();
    }
    if (node["offset"]) {
        inst.offset = node["offset"].as<std::vector<int>>();
    }
    return inst;
}

Dialogue ParseDialogueNode(const YAML::Node &node) {
    Dialogue dlg;
    if (node["id"]) {
        dlg.id = node["id"].as<std::string>();
    }
    if (node["speaker"]) {
        dlg.speakerId = node["speaker"].as<std::string>();
    }
    if (node["displayname"]) {
        dlg.displayName = node["displayname"].as<std::string>();
    }
    if (node["sprite"]) {
        dlg.sprite = node["sprite"].as<std::string>();
    }
    dlg.text = node["text"].as<std::string>("");

    const std::string type = node["type"].as<std::string>("dialogue");
    dlg.tag =
        (type == "narration") ? DialogueTag::Narration : DialogueTag::Dialogue;
    return dlg;
}

BgCommand ParseBgCommand(const YAML::Node &node) {
    BgCommand cmd;
    cmd.action = node["action"].as<std::string>("change");
    if (node["image"]) {
        cmd.image = node["image"].as<std::string>();
    }
    if (node["text"]) {
        cmd.text = node["text"].as<std::string>();
    }
    return cmd;
}

SoundCommand ParseSoundCommand(const YAML::Node &node) {
    SoundCommand cmd;
    cmd.channel = node["channel"].as<std::string>("bgm");
    cmd.action = node["action"].as<std::string>("play");
    if (node["file"]) {
        cmd.file = node["file"].as<std::string>();
    }
    if (node["volume"]) {
        cmd.volume = node["volume"].as<float>();
    }
    return cmd;
}

CharacterCommand ParseCharacterCommand(const YAML::Node &node) {
    CharacterCommand cmd;
    cmd.action = node["action"].as<std::string>("show");
    if (node["target"]) {
        cmd.targetId = node["target"].as<std::string>();
    }
    if (node["sprite"]) {
        cmd.sprite = node["sprite"].as<std::string>();
    }
    return cmd;
}

FlowTarget ParseFlowTargetFromNode(const YAML::Node &node) {
    FlowTarget target;
    if (node["chapter"]) {
        target.chapter = node["chapter"].as<std::string>();
    }
    if (node["scene"]) {
        target.scene = node["scene"].as<std::string>();
    }
    if (node["label"]) {
        target.label = node["label"].as<std::string>();
    }
    return target;
}

FlowCommand ParseFlowCommand(const YAML::Node &node) {
    FlowCommand cmd;
    cmd.action = node["action"].as<std::string>("goto");
    if (node["goto"]) {
        cmd.target = ParseFlowTargetFromNode(node["goto"]);
    } else {
        cmd.target = ParseFlowTargetFromNode(node);
    }
    return cmd;
}

SetCommand ParseSetCommand(const YAML::Node &node) {
    SetCommand cmd;
    cmd.varName = node["var"].as<std::string>();
    const YAML::Node &val = node["value"];
    if (val.IsScalar()) {
        const std::string s = val.as<std::string>();
        if (s == "true" || s == "false") {
            cmd.value = (s == "true");
        } else {
            try {
                int i = std::stoi(s);
                cmd.value = i;
            } catch (...) {
                cmd.value = s;
            }
        }
    } else if (val.IsSequence() || val.IsMap()) {
        cmd.value = val.as<std::string>();
    }
    return cmd;
}

IfCommand ParseIfCommand(const YAML::Node &node) {
    IfCommand cmd;
    cmd.varName = node["cond"].as<std::string>();

    if (node["then"]) {
        cmd.thenTarget = ParseFlowTargetFromNode(node["then"]);
    }
    if (node["else"]) {
        cmd.elseTarget = ParseFlowTargetFromNode(node["else"]);
    }
    return cmd;
}

LabelCommand ParseLabelCommand(const YAML::Node &node) {
    LabelCommand cmd;
    cmd.id = node["id"].as<std::string>();
    return cmd;
}

WaitCommand ParseWaitCommand(const YAML::Node &node) {
    WaitCommand cmd;
    if (node["ms"]) {
        cmd.milliseconds = node["ms"].as<int>();
    } else if (node["milliseconds"]) {
        cmd.milliseconds = node["milliseconds"].as<int>();
    }
    return cmd;
}

Choice ParseChoice(const YAML::Node &node) {
    Choice choice;
    if (node["id"]) {
        choice.id = node["id"].as<std::string>();
    }
    if (node["text"]) {
        choice.text = node["text"].as<std::string>();
    }

    const YAML::Node &opts = node["options"];
    if (opts && opts.IsSequence()) {
        for (const auto &optNode : opts) {
            ChoiceOption opt;
            opt.id = optNode["id"].as<std::string>();
            opt.text = optNode["text"].as<std::string>("");

            if (optNode["goto"]) {
                opt.target = ParseFlowTargetFromNode(optNode["goto"]);
            } else {
                opt.target = ParseFlowTargetFromNode(optNode);
            }

            choice.options.push_back(std::move(opt));
        }
    }

    return choice;
}

} // namespace

ScriptIndex ScriptYamlLoader::LoadIndex(const std::string &path) {
    ScriptIndex index;

    try {
        YAML::Node root = YAML::LoadFile(path);

        // 解析 chapters 列表
        if (root["chapters"] && root["chapters"].IsSequence()) {
            const auto &chs = root["chapters"];
            fs::path baseDir = fs::path(path).parent_path();

            for (const auto &chNode : chs) {
                ChapterId id = chNode["id"].as<std::string>();
                std::string file = chNode["file"].as<std::string>();
                fs::path filePath = baseDir / file;

                index.order.push_back(id);
                index.chapterFiles.emplace(id, filePath.string());
            }
        }
    } catch (const YAML::Exception &e) {
        std::cerr << "加载 index.yaml 失败: " << e.what() << std::endl;
    }

    return index;
}

Chapter ScriptYamlLoader::LoadChapterFromFile(const std::string &path) {
    Chapter chapter;

    try {
        YAML::Node root = YAML::LoadFile(path);

        // meta 段
        if (root["meta"]) {
            const auto &meta = root["meta"];
            if (meta["id"]) {
                chapter.id = meta["id"].as<std::string>();
            }
            if (meta["title"]) {
                chapter.title = meta["title"].as<std::string>();
            }
        }

        // 顶层 characters
        if (root["characters"]) {
            for (const auto &it : root["characters"]) {
                const std::string name = it.first.as<std::string>();
                chapter.characters.emplace(name, ParseCharacterDef(it.second));
            }
        }

        // scenes
        if (root["scenes"] && root["scenes"].IsSequence()) {
            const auto &scenesNode = root["scenes"];
            for (const auto &sceneNode : scenesNode) {
                Scene scene;
                scene.id = sceneNode["id"].as<std::string>();
                if (sceneNode["title"]) {
                    scene.title = sceneNode["title"].as<std::string>();
                }
                if (sceneNode["background"]) {
                    scene.background =
                        sceneNode["background"].as<std::string>();
                }

                // 场景内角色初始布局
                if (sceneNode["characters"]) {
                    for (const auto &charNode : sceneNode["characters"]) {
                        scene.characters.push_back(
                            ParseCharacterInstance(charNode));
                    }
                }

                // sequence
                const YAML::Node &seqNode = sceneNode["sequence"];
                if (seqNode && seqNode.IsSequence()) {
                    size_t opIndex = 0;
                    for (const auto &item : seqNode) {
                        const std::string type = item["type"].as<std::string>();

                        if (type == "dialogue" || type == "narration") {
                            Dialogue dlg = ParseDialogueNode(item);
                            scene.sequence.emplace_back(std::move(dlg));
                        } else if (type == "bg") {
                            BgCommand bg = ParseBgCommand(item);
                            scene.sequence.emplace_back(std::move(bg));
                        } else if (type == "sound") {
                            SoundCommand snd = ParseSoundCommand(item);
                            scene.sequence.emplace_back(std::move(snd));
                        } else if (type == "character") {
                            CharacterCommand cc = ParseCharacterCommand(item);
                            scene.sequence.emplace_back(std::move(cc));
                        } else if (type == "flow") {
                            FlowCommand fc = ParseFlowCommand(item);
                            scene.sequence.emplace_back(std::move(fc));
                        } else if (type == "set") {
                            SetCommand sc = ParseSetCommand(item);
                            scene.sequence.emplace_back(std::move(sc));
                        } else if (type == "if") {
                            IfCommand ic = ParseIfCommand(item);
                            scene.sequence.emplace_back(std::move(ic));
                        } else if (type == "label") {
                            LabelCommand lc = ParseLabelCommand(item);
                            scene.labels.emplace(lc.id, opIndex);
                            scene.sequence.emplace_back(std::move(lc));
                        } else if (type == "wait") {
                            WaitCommand wc = ParseWaitCommand(item);
                            scene.sequence.emplace_back(std::move(wc));
                        } else if (type == "choice") {
                            Choice c = ParseChoice(item);
                            scene.sequence.emplace_back(std::move(c));
                        } else {
                            // 未知 type
                            // 下次一定
                            continue;
                        }

                        ++opIndex;
                    }
                }

                chapter.scenes.emplace(scene.id, std::move(scene));
            }
        }

    } catch (const YAML::Exception &e) {
        std::cerr << "加载章节 YAML 失败: " << path << ", error: " << e.what()
                  << std::endl;
    }

    return chapter;
}

} // namespace script

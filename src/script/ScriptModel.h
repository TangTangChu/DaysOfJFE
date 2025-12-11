#pragma once

#include <map>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace script {

using ScriptId = std::string; // id
using SceneId = std::string;
using ChapterId = std::string;

// 文本类型：对白 / 旁白
enum class DialogueTag { Dialogue, Narration };

// 角色定义
struct CharacterDef {
    std::string id;                // 剧本内部 id
    std::string base_pos = "left"; // 立绘基准位置：left/right/center
    std::vector<int> offset{0, 0}; // 默认偏移 [x, y]
    std::map<std::string, std::string> sprites; // 表情 key -> 图片路径
};

// 场景内角色实例
struct CharacterInstance {
    std::string ref_name;          // 角色名
    std::string sprite = "normal"; // sprite key
    std::vector<int> offset;       // 场景内局部偏移
};

struct Dialogue {
    std::string id;          // 可选：唯一 ID 用于已读记录等
    std::string speakerId;   // 角色 id
    std::string displayName; // UI 上显示名
    std::string text;        // 文本内容
    std::string sprite;      // 可选：本句对白使用的 sprite key
    DialogueTag tag = DialogueTag::Dialogue;
};

// 背景相关命令
struct BgCommand {
    std::string action; // "change" / "title" / ...
    std::string image;  // 背景图片路径
    std::string text;   // 标题文字
};

// 音频相关命令
struct SoundCommand {
    std::string channel; // bgm / voice / sfx
    std::string action;  // play / pause / stop / volume
    std::string file;    // 播放文件
    float volume = 1.0f; // 0~1
};

// 角色立绘操作
struct CharacterCommand {
    std::string action;   // show / hide / sprite
    std::string targetId; // 角色 id
    std::string sprite;   // sprite key
};

// 跳转目标
struct FlowTarget {
    ChapterId chapter; // 目标章节，空表示当前章节
    SceneId scene;     // 目标场景
    ScriptId label;    // 目标 label，可为空
};

// 流程控制命令
struct FlowCommand {
    std::string action;
    FlowTarget target; // 目标
};

// 变量值类型
using ScriptValue = std::variant<int, bool, std::string>;

// 设置变量
struct SetCommand {
    std::string varName;
    ScriptValue value;
};

// 条件跳转
struct IfCommand {
    std::string varName;   // 检查的变量名
    FlowTarget thenTarget; // 为真时跳转
    FlowTarget elseTarget; // 为假时跳转
};

// label 占位，用于 Scene 内部跳转锚点
struct LabelCommand {
    ScriptId id;
};

// 等待命令
struct WaitCommand {
    int milliseconds = 0; // >0 表示在此处停顿
};

// 选项
struct ChoiceOption {
    std::string id;    // option id
    std::string text;  // 选项显示文本
    FlowTarget target; // 选中后跳转目标
};

struct Choice {
    std::string id;   // choice id
    std::string text; // 问题文案
    std::vector<ChoiceOption> options;
};

using ScriptOp = std::variant<Dialogue, BgCommand, SoundCommand,
                              CharacterCommand, FlowCommand, SetCommand,
                              IfCommand, LabelCommand, WaitCommand, Choice>;

// 场景
struct Scene {
    SceneId id;             // 场景 id
    std::string title;      // 可选标题
    std::string background; // 初始背景

    std::vector<CharacterInstance> characters; // 初始立绘
    std::vector<ScriptOp> sequence;            // 主事件序列

    // 预先解析的 label 表：label id -> sequence 下标
    std::unordered_map<ScriptId, size_t> labels;
};

// 章节
struct Chapter {
    ChapterId id;      // 章节 id
    std::string title; // 标题
    std::map<std::string, CharacterDef> characters;
    std::unordered_map<SceneId, Scene> scenes;
};

struct ScriptIndex {
    std::vector<ChapterId> order;                            // 章节顺序
    std::unordered_map<ChapterId, std::string> chapterFiles; // id -> 文件路径
};

} // namespace script

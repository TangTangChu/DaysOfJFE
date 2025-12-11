#pragma once

#include "script/ScriptModel.h"
#include <functional>

namespace script {

struct RuntimeCallbacks {
    std::function<void(const Chapter &, const Scene &)> onSceneEnter;
    std::function<void(const Dialogue &)> onDialogue;
    std::function<void(const BgCommand &)> onBg;
    std::function<void(const SoundCommand &)> onSound;
    std::function<void(const CharacterCommand &)> onCharacter;
    std::function<void(const Choice &)> onChoice;
};

struct RuntimePosition {
    ChapterId chapterId;
    SceneId sceneId;
    size_t index = 0; // 当前 sequence 索引
};

// 存档状态
struct SaveState {
    RuntimePosition pos;
    std::unordered_map<std::string, ScriptValue> vars;
};

class ScriptRuntime {
  public:
    ScriptRuntime(std::vector<Chapter> chapters, RuntimeCallbacks callbacks);
    void Start(const ChapterId &chapterId, const SceneId &sceneId,
               const ScriptId &label = "");

    void Next();

    void Choose(const std::string &choiceId, const std::string &optionId);

    bool IsFinished() const;      
    bool IsWaitingChoice() const;
    SaveState CaptureState() const;
    void RestoreState(const SaveState &state);
    void SetVar(const std::string &name, const ScriptValue &value);
    const ScriptValue *GetVar(const std::string &name) const;

    RuntimePosition GetPosition() const;

  private:
    std::unordered_map<ChapterId, Chapter> chapters_;
    RuntimeCallbacks cbs_;

    const Chapter *curChapter_ = nullptr;
    const Scene *curScene_ = nullptr;
    size_t index_ = 0;

    std::unordered_map<std::string, ScriptValue> vars_;

    const Choice *pendingChoice_ = nullptr;
    void JumpTo(const FlowTarget &target);
    bool JumpToLabel(const ScriptId &labelId);
    bool IsTruthy(const std::string &varName) const;
};

} // namespace script

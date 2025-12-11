#include "script/ScriptRuntime.h"

#include <utility>

namespace script {

ScriptRuntime::ScriptRuntime(std::vector<Chapter> chapters,
                             RuntimeCallbacks callbacks)
    : cbs_(std::move(callbacks)) {
    for (auto &ch : chapters) {
        chapters_.emplace(ch.id, std::move(ch));
    }
}

void ScriptRuntime::Start(const ChapterId &chapterId, const SceneId &sceneId,
                          const ScriptId &label) {
    auto chit = chapters_.find(chapterId);
    if (chit == chapters_.end()) {
        curChapter_ = nullptr;
        curScene_ = nullptr;
        index_ = 0;
        pendingChoice_ = nullptr;
        return;
    }
    curChapter_ = &chit->second;

    // 查找场景
    auto sit = curChapter_->scenes.find(sceneId);
    if (sit == curChapter_->scenes.end()) {
        curScene_ = nullptr;
        index_ = 0;
        pendingChoice_ = nullptr;
        return;
    }
    curScene_ = &sit->second;

    // 定位 label
    if (!label.empty()) {
        auto lit = curScene_->labels.find(label);
        index_ = (lit != curScene_->labels.end()) ? lit->second : 0;
    } else {
        index_ = 0;
    }

    pendingChoice_ = nullptr;

    // 通知宿主进入新场景
    if (cbs_.onSceneEnter) {
        cbs_.onSceneEnter(*curChapter_, *curScene_);
    }
}

void ScriptRuntime::Next() {
    // 若当前没有有效场景或正在等待选项，暂不推进
    if (!curScene_ || pendingChoice_) {
        return;
    }

    while (curScene_) {
        auto &seq = curScene_->sequence;
        const size_t n = seq.size();

        while (index_ < n) {
            const ScriptOp &op = seq[index_++];

            if (auto dlg = std::get_if<Dialogue>(&op)) {
                if (cbs_.onDialogue) {
                    cbs_.onDialogue(*dlg);
                }
                return;
            }

            if (auto bg = std::get_if<BgCommand>(&op)) {
                if (cbs_.onBg) {
                    cbs_.onBg(*bg);
                }
                continue;
            }

            if (auto snd = std::get_if<SoundCommand>(&op)) {
                if (cbs_.onSound) {
                    cbs_.onSound(*snd);
                }
                continue;
            }

            if (auto chCmd = std::get_if<CharacterCommand>(&op)) {
                if (cbs_.onCharacter) {
                    cbs_.onCharacter(*chCmd);
                }
                continue;
            }

            if (auto setCmd = std::get_if<SetCommand>(&op)) {
                vars_[setCmd->varName] = setCmd->value;
                continue;
            }

            if (auto ifCmd = std::get_if<IfCommand>(&op)) {
                const bool cond = IsTruthy(ifCmd->varName);
                const FlowTarget &target =
                    cond ? ifCmd->thenTarget : ifCmd->elseTarget;
                JumpTo(target);
                break;
            }

            if (auto flow = std::get_if<FlowCommand>(&op)) {
                JumpTo(flow->target);
                break;
            }

            if (auto label = std::get_if<LabelCommand>(&op)) {
                (void)label;
                continue;
            }

            if (auto wait = std::get_if<WaitCommand>(&op)) {
                if (wait->milliseconds > 0) {
                    return;
                }
                continue;
            }

            if (auto choice = std::get_if<Choice>(&op)) {
                pendingChoice_ = choice;
                if (cbs_.onChoice) {
                    cbs_.onChoice(*choice);
                }
                return; // 等待玩家选择
            }
        }
        return;
    }
}

void ScriptRuntime::Choose(const std::string &choiceId,
                           const std::string &optionId) {
    if (!pendingChoice_)
        return;

    const Choice &choice = *pendingChoice_;
    if (!choice.id.empty() && choice.id != choiceId) {
        return;
    }

    for (const auto &opt : choice.options) {
        if (opt.id == optionId) {
            pendingChoice_ = nullptr;
            JumpTo(opt.target);
            return;
        }
    }
}

bool ScriptRuntime::IsFinished() const {
    if (!curScene_)
        return true;
    const auto &seq = curScene_->sequence;
    return index_ >= seq.size() && !pendingChoice_;
}

bool ScriptRuntime::IsWaitingChoice() const {
    return pendingChoice_ != nullptr;
}

SaveState ScriptRuntime::CaptureState() const {
    SaveState s;
    s.pos = GetPosition();
    s.vars = vars_;
    return s;
}

void ScriptRuntime::RestoreState(const SaveState &state) {
    vars_ = state.vars;

    // 定位章节和场景
    auto chit = chapters_.find(state.pos.chapterId);
    if (chit == chapters_.end()) {
        curChapter_ = nullptr;
        curScene_ = nullptr;
        index_ = 0;
        pendingChoice_ = nullptr;
        return;
    }
    curChapter_ = &chit->second;

    auto sit = curChapter_->scenes.find(state.pos.sceneId);
    if (sit == curChapter_->scenes.end()) {
        curScene_ = nullptr;
        index_ = 0;
        pendingChoice_ = nullptr;
        return;
    }
    curScene_ = &sit->second;
    index_ = state.pos.index;
    pendingChoice_ = nullptr;

    if (curChapter_ && curScene_ && cbs_.onSceneEnter) {
        cbs_.onSceneEnter(*curChapter_, *curScene_);
    }
}

void ScriptRuntime::SetVar(const std::string &name, const ScriptValue &value) {
    vars_[name] = value;
}

const ScriptValue *ScriptRuntime::GetVar(const std::string &name) const {
    auto it = vars_.find(name);
    return it == vars_.end() ? nullptr : &it->second;
}

RuntimePosition ScriptRuntime::GetPosition() const {
    RuntimePosition pos;
    if (curChapter_)
        pos.chapterId = curChapter_->id;
    if (curScene_)
        pos.sceneId = curScene_->id;
    pos.index = index_;
    return pos;
}

void ScriptRuntime::JumpTo(const FlowTarget &target) {
    ChapterId chapterId = target.chapter;
    if (chapterId.empty() && curChapter_) {
        chapterId = curChapter_->id;
    }

    auto chit = chapters_.find(chapterId);
    if (chit == chapters_.end()) {
        curChapter_ = nullptr;
        curScene_ = nullptr;
        index_ = 0;
        pendingChoice_ = nullptr;
        return;
    }
    curChapter_ = &chit->second;

    auto sit = curChapter_->scenes.find(target.scene);
    if (sit == curChapter_->scenes.end()) {
        curScene_ = nullptr;
        index_ = 0;
        pendingChoice_ = nullptr;
        return;
    }
    curScene_ = &sit->second;

    if (!target.label.empty()) {
        auto lit = curScene_->labels.find(target.label);
        index_ = (lit != curScene_->labels.end()) ? lit->second : 0;
    } else {
        index_ = 0;
    }

    pendingChoice_ = nullptr;

    if (curChapter_ && curScene_ && cbs_.onSceneEnter) {
        cbs_.onSceneEnter(*curChapter_, *curScene_);
    }
}

bool ScriptRuntime::JumpToLabel(const ScriptId &labelId) {
    if (!curScene_)
        return false;
    auto it = curScene_->labels.find(labelId);
    if (it == curScene_->labels.end())
        return false;
    index_ = it->second;
    return true;
}

bool ScriptRuntime::IsTruthy(const std::string &varName) const {
    auto it = vars_.find(varName);
    if (it == vars_.end())
        return false;

    const ScriptValue &v = it->second;
    if (const auto *b = std::get_if<bool>(&v)) {
        return *b;
    }
    if (const auto *i = std::get_if<int>(&v)) {
        return *i != 0;
    }
    if (const auto *s = std::get_if<std::string>(&v)) {
        return !s->empty();
    }
    return false;
}

} // namespace script

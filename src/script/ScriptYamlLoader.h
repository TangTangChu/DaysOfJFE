#pragma once

#include "script/ScriptModel.h"
#include <string>
#include <vector>

namespace script {

struct ScriptIndex;

class ScriptYamlLoader {
  public:
    static ScriptIndex LoadIndex(const std::string &path);
    static Chapter LoadChapterFromFile(const std::string &path);

  private:
};

} // namespace script

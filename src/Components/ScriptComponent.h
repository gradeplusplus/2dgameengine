#ifndef SCRIPTCOMPONENT_H
#define SCRIPTCOMPONENT_H
#include <string>

struct ScriptComponent {
    std::string scriptPath;
    ScriptComponent(std::string scriptPath = "") {
        this->scriptPath = scriptPath;
    }
};

#endif

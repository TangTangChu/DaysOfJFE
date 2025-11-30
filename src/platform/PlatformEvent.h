#pragma once
#include <variant>

struct MouseButtonEvent {
    enum Button { Left, Right, Middle };
    enum Action { Down, Up };
    int x = 0, y = 0;
    Button button = Left;
    Action action = Down;
};

struct MouseMoveEvent {
    int x = 0, y = 0;
};

struct KeyEvent {
    enum Action { Down, Up };
    int key = 0;  // GLFW key code
    int mods = 0; // shift/ctrl/alt
    Action action = Down;
};

using PlatformEvent = std::variant<MouseButtonEvent, MouseMoveEvent, KeyEvent>;

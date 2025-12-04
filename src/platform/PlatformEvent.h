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
    int dx = 0, dy = 0; // 鼠标移动增量
};

struct MouseWheelEvent {
    int x = 0, y = 0;
    float deltaX = 0.0f;
    float deltaY = 0.0f;
};

struct KeyEvent {
    enum Action { Down, Up, Repeat };
    int key = 0; // GLFW key code
    int scancode = 0;
    int mods = 0; // shift/ctrl/alt
    Action action = Down;
};

struct TextInputEvent {
    unsigned int codepoint = 0;
};

struct WindowResizeEvent {
    int width = 0;
    int height = 0;
};

struct FocusEvent {
    bool focused = false;
};

struct TimerEvent {
    int timerId = 0;
};

struct GamepadEvent {
    int gamepadId = 0;
    int button = 0;
    float axisValue = 0.0f;
    enum Type { Button, Axis };
    Type type = Button;
    enum Action { Pressed, Released, Moved };
    Action action = Pressed;
};

using PlatformEvent =
    std::variant<MouseButtonEvent, MouseMoveEvent, MouseWheelEvent, KeyEvent,
                 TextInputEvent, WindowResizeEvent, FocusEvent, TimerEvent,
                 GamepadEvent>;

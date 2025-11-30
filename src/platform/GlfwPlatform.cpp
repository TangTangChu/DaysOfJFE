#include "GlfwPlatform.h"
#include "ui/WindowManager.h"

GlfwPlatform *GlfwPlatform::s_self = nullptr;

GlfwPlatform::GlfwPlatform(GLFWwindow *win, WindowManager *wm)
    : m_win(win), m_wm(wm) {
    s_self = this;
    glfwSetMouseButtonCallback(win, MouseButtonCb);
    glfwSetCursorPosCallback(win, CursorPosCb);
    glfwSetKeyCallback(win, KeyCb);
}

void GlfwPlatform::dispatch(const PlatformEvent &e) {
    if (m_wm)
        m_wm->Dispatch(e);
}

void GlfwPlatform::MouseButtonCb(GLFWwindow *win, int button, int action, int) {
    double x, y;
    glfwGetCursorPos(win, &x, &y);

    MouseButtonEvent e;
    e.x = (int)x;
    e.y = (int)y;
    e.button = (button == GLFW_MOUSE_BUTTON_LEFT)    ? MouseButtonEvent::Left
               : (button == GLFW_MOUSE_BUTTON_RIGHT) ? MouseButtonEvent::Right
                                                     : MouseButtonEvent::Middle;
    e.action =
        (action == GLFW_PRESS) ? MouseButtonEvent::Down : MouseButtonEvent::Up;

    s_self->dispatch(e);
}

void GlfwPlatform::CursorPosCb(GLFWwindow *, double x, double y) {
    MouseMoveEvent e{(int)x, (int)y};
    s_self->dispatch(e);
}

void GlfwPlatform::KeyCb(GLFWwindow *, int key, int, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_RELEASE)
        return;

    KeyEvent e;
    e.key = key;
    e.mods = mods;
    e.action = (action == GLFW_PRESS) ? KeyEvent::Down : KeyEvent::Up;

    s_self->dispatch(e);
}

#include "GlfwPlatform.h"
#include "windows/WindowManager.h"
#include "app/EventManager.h"

GlfwPlatform *GlfwPlatform::s_self = nullptr;

GlfwPlatform::GlfwPlatform(GLFWwindow *win, EventManager *eventMgr)
    : m_win(win), m_eventMgr(eventMgr) {
    s_self = this;
    glfwSetMouseButtonCallback(win, MouseButtonCb);
    glfwSetCursorPosCallback(win, CursorPosCb);
    glfwSetKeyCallback(win, KeyCb);
    glfwSetScrollCallback(win, ScrollCb);
    glfwSetCharCallback(win, CharCb);
    glfwSetFramebufferSizeCallback(win, FramebufferSizeCb);
    glfwSetWindowFocusCallback(win, WindowFocusCb);
}

void GlfwPlatform::dispatch(const PlatformEvent &e) {
    if (m_eventMgr)
        m_eventMgr->ProcessEvent(e);
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
    static int lastX = 0, lastY = 0;
    int currentX = (int)x;
    int currentY = (int)y;
    MouseMoveEvent e;
    e.x = currentX;
    e.y = currentY;
    e.dx = currentX - lastX;
    e.dy = currentY - lastY;

    lastX = currentX;
    lastY = currentY;

    s_self->dispatch(e);
}
void GlfwPlatform::KeyCb(GLFWwindow *, int key, int scancode, int action,
                         int mods) {
    if (action != GLFW_PRESS && action != GLFW_RELEASE && action != GLFW_REPEAT)
        return;

    KeyEvent e;
    e.key = key;
    e.scancode = scancode;
    e.mods = mods;

    switch (action) {
    case GLFW_PRESS:
        e.action = KeyEvent::Down;
        break;
    case GLFW_RELEASE:
        e.action = KeyEvent::Up;
        break;
    case GLFW_REPEAT:
        e.action = KeyEvent::Repeat;
        break;
    }

    s_self->dispatch(e);
}

void GlfwPlatform::ScrollCb(GLFWwindow *, double xoffset, double yoffset) {
    double x, y;
    glfwGetCursorPos(s_self->m_win, &x, &y);

    MouseWheelEvent e;
    e.x = (int)x;
    e.y = (int)y;
    e.deltaX = (float)xoffset;
    e.deltaY = (float)yoffset;

    s_self->dispatch(e);
}

void GlfwPlatform::CharCb(GLFWwindow *, unsigned int codepoint) {
    TextInputEvent e;
    e.codepoint = codepoint;

    s_self->dispatch(e);
}

void GlfwPlatform::FramebufferSizeCb(GLFWwindow *, int width, int height) {
    WindowResizeEvent e;
    e.width = width;
    e.height = height;

    s_self->dispatch(e);
}

void GlfwPlatform::WindowFocusCb(GLFWwindow *, int focused) {
    FocusEvent e;
    e.focused = (focused == GLFW_TRUE);

    s_self->dispatch(e);
}

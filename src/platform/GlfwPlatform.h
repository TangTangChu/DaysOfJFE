#pragma once
#include "PlatformEvent.h"
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

class WindowManager;
class EventManager;

class GlfwPlatform {
  public:
    GlfwPlatform(GLFWwindow *win, EventManager *eventMgr);

  private:
    GLFWwindow *m_win;
    EventManager *m_eventMgr;
    float m_contentScaleX = 1.0f;
    float m_contentScaleY = 1.0f;
    static GlfwPlatform *s_self;

    static void MouseButtonCb(GLFWwindow *, int button, int action, int mods);
    static void CursorPosCb(GLFWwindow *, double x, double y);
    static void KeyCb(GLFWwindow *, int key, int scancode, int action,
                      int mods);
    static void ScrollCb(GLFWwindow *, double xoffset, double yoffset);
    static void CharCb(GLFWwindow *, unsigned int codepoint);
    static void FramebufferSizeCb(GLFWwindow *, int width, int height);
    static void WindowFocusCb(GLFWwindow *, int focused);
    static void ContentScaleCb(GLFWwindow *, float xscale, float yscale);

    void dispatch(const PlatformEvent &e);
};

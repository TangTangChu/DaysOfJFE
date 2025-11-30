#pragma once
#include "PlatformEvent.h"
#include <GLFW/glfw3.h>

class WindowManager;

class GlfwPlatform {
  public:
    GlfwPlatform(GLFWwindow *win, WindowManager *wm);

  private:
    GLFWwindow *m_win;
    WindowManager *m_wm;
    static GlfwPlatform *s_self;

    static void MouseButtonCb(GLFWwindow *, int button, int action, int mods);
    static void CursorPosCb(GLFWwindow *, double x, double y);
    static void KeyCb(GLFWwindow *, int key, int scancode, int action,
                      int mods);

    void dispatch(const PlatformEvent &e);
};

#include "app/ApplicationContext.h"
#include "gfx/FontManager.h"
#include "gfx/SkiaRenderer.h"
#include "platform/GlfwPlatform.h"
#include "windows/GameWindow.h"
#include "windows/HomeWindow.h"
#include "windows/LoadSceneWindow.h"
#include "windows/LoadingWindow.h"
#include "windows/SettingWindow.h"
#include "windows/WindowManager.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

int main() {
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow *win =
        glfwCreateWindow(1600, 900, "DaysOfJFE", nullptr, nullptr);
    if (!win) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(win);
        glfwTerminate();
        return -1;
    }
    int fbw, fbh;
    glfwGetFramebufferSize(win, &fbw, &fbh);

    SkiaRenderer renderer;
    renderer.initGL(fbw, fbh);
    auto &fontManager = FontManager::Instance();
    fontManager.Initialize();
    std::string fontPath = "./assets/fonts/";
    fontManager.RegisterFont("SarasaUISC",
                             fontPath + "SarasaUiSC-SemiBold.ttf");
    fontManager.RegisterFont("SourceHans",
                             fontPath + "SourceHanSerifSC-Heavy.otf");
    fontManager.SetDefaultFont("SarasaUISC");
    renderer.setDefaultFont("SarasaUISC", 16.0f);

    WindowManager wm;

    ApplicationContext applicationContext(&wm);
    wm.SetGlobalEvent(&applicationContext);

    auto hw = std::make_shared<HomeWindow>();
    auto gw = std::make_shared<GameWindow>();
    auto lw = std::make_shared<LoadingWindow>();
    auto lsw = std::make_shared<LoadSceneWindow>();
    auto sw = std::make_shared<SettingWindow>();

    wm.AddWindow(hw);
    wm.AddWindow(gw);
    wm.AddWindow(lw);
    wm.AddWindow(lsw);
    wm.AddWindow(sw);
    wm.SwitchWindow(0);

    GlfwPlatform platform(win, &applicationContext.eventManager);

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();

        int w, h;
        glfwGetFramebufferSize(win, &w, &h);
        renderer.resizeIfNeeded(w, h);

        renderer.beginFrame();
        wm.Redraw(renderer);
        renderer.endFrame();

        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}

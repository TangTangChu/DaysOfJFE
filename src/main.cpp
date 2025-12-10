/**
 * @file main.cpp
 * @brief Days of JFE - Linux版主程序入口
 * 
 * 项目信息:
 * - 原版: Windows (EasyX + GDI+ + BASS)
 * - 新版: Linux (SDL2 + Skia + SDL_mixer)
 * - 窗口尺寸: 1600x900 (固定，不可调整)
 * - 帧率: 60 FPS
 * - 渲染后端: OpenGL 2.1 (GPU加速)
 * 
 * 渲染流程:
 * 1. SDL初始化 + OpenGL上下文
 * 2. Skia初始化 + GPU后端
 * 3. 创建窗口管理器和游戏窗口
 * 4. 主循环: 事件处理 -> 绘制 -> 交换缓冲区
 */

#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include <vector>

// --- Skia核心头文件 ---
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkSurface.h"
#include "include/core/SkImage.h"

// --- GPU后端头文件 ---
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"

// --- 游戏窗口类 ---
#include "WindowManager.h"
#include "HomeWindow.h"
#include "GameWindow.h"
#include "LoadingWindow.h"

// 窗口配置
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;
const char* WINDOW_TITLE = "Days of JFE - Linux Edition";

/**
 * @brief 程序主入口
 * 
 * 初始化流程:
 * 1. SDL2: 视频、音频子系统
 * 2. OpenGL: 2.1上下文，双缓冲
 * 3. Skia: GPU后端（优先）或CPU后端（回退）
 * 4. 窗口管理器: 加载动画、主菜单、游戏窗口
 * 5. 主循环: 60 FPS固定帧率
 * 
 * @return 0: 正常退出, 1: 初始化失败
 */
int main(int argc, char* argv[]) {
    // ========================================================
    // 1. SDL初始化 (替代initgraph)
    // ========================================================
    std::cout << "=== Days of JFE Linux Edition ===" << std::endl;
    std::cout << "1. 初始化 SDL..." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL 初始化失败: " << SDL_GetError() << std::endl;
        return 1;
    }
    std::cout << "   ✓ SDL 初始化成功" << std::endl;

    // 设置 OpenGL 属性
    std::cout << "2. 配置 OpenGL..." << std::endl;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // 创建窗口（固定尺寸，不可调整）
    std::cout << "3. 创建窗口..." << std::endl;
    SDL_Window* window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, 
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        std::cerr << "窗口创建失败: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    std::cout << "   ✓ 窗口创建成功" << std::endl;

    std::cout << "4. 创建 OpenGL 上下文..." << std::endl;
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "OpenGL 上下文创建失败: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    std::cout << "   ✓ OpenGL 上下文创建成功" << std::endl;
    
    SDL_GL_MakeCurrent(window, glContext);
    SDL_GL_SetSwapInterval(1); // VSync

    // ========================================================
    // 2. Skia初始化 (替代GDI+)
    // ========================================================
    std::cout << "5. 初始化 Skia..." << std::endl;
    SkGraphics::Init();
    
    auto interface = GrGLMakeNativeInterface();
    sk_sp<GrDirectContext> context;
    sk_sp<SkSurface> surface;
    
    if (!interface) {
        std::cout << "   ! 无法创建原生 GL 接口，尝试默认接口" << std::endl;
        // 尝试使用默认的 GL 接口
        interface = GrGLMakeNativeInterface();
    }
    
    if (interface) {
        context = GrDirectContexts::MakeGL(interface);
        if (context) {
            std::cout << "   ✓ GPU 上下文创建成功" << std::endl;
        }
    }
    
    if (context) {
        // GPU 渲染
        std::cout << "   使用 GPU 渲染" << std::endl;
        GrGLFramebufferInfo fbInfo;
        fbInfo.fFBOID = 0;
        fbInfo.fFormat = 0x8058;  // GL_RGBA8

        GrBackendRenderTarget backendRT = GrBackendRenderTargets::MakeGL(
            WINDOW_WIDTH, WINDOW_HEIGHT, 0, 8, fbInfo
        );

        surface = SkSurfaces::WrapBackendRenderTarget(
            context.get(),
            backendRT,
            kBottomLeft_GrSurfaceOrigin,
            kRGBA_8888_SkColorType,
            nullptr,
            nullptr
        );
        
        if (!surface) {
            std::cout << "   ! GPU Surface 创建失败，回退到 CPU" << std::endl;
        }
    } else {
        std::cout << "   ! GPU 不可用" << std::endl;
    }
    
    if (!surface) {
        // 回退到 CPU 渲染
        std::cout << "   使用 CPU 渲染（性能较低）" << std::endl;
        surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(WINDOW_WIDTH, WINDOW_HEIGHT));
    }

    if (!surface) {
        std::cerr << "创建 SkSurface 失败" << std::endl;
        return 1;
    }
    std::cout << "   ✓ Skia 渲染表面创建成功" << std::endl;

    SkCanvas* canvas = surface->getCanvas();

    // ========================================================
    // 3. 初始化窗口管理器
    // ========================================================
    WindowManager wm(canvas);
    
    // 创建加载动画窗口
    auto lw = std::make_shared<LoadingWindow>(
        std::vector<std::string>{
            "assets/image/misc/mainflash.png",
            "assets/image/misc/attention.png"
        },
        700,  // 淡入时间
        700   // 淡出时间
    );
    wm.AddWindow(lw);
    
    // 创建主页面
    auto hw = std::make_shared<HomeWindow>();
    wm.AddWindow(hw);
    
    // 创建游戏窗口
    auto gw = std::make_shared<GameWindow>();
    wm.AddWindow(gw);
    
    // 预加载游戏脚本（但不加载场景，等用户点击"开始游戏"后才加载场景0）
    gw->LoadScript("assets/script/script121.yaml");
    
    // 设置加载动画完成后跳转到主界面
    lw->SetFinalHook([&wm]() {
        wm.SwitchWindow(1);  // 跳转到主界面（HomeWindow）
    });
    
    // 启动时显示加载动画
    wm.SwitchWindow(0);
    lw->StartAnimation();

    // ========================================================
    // 4. 主循环 (固定60 FPS)
    // ========================================================
    bool running = true;
    SDL_Event event;
    
    std::cout << "程序启动成功！" << std::endl;
    
    // 如果使用 CPU 渲染，需要额外的纹理来传输到 GPU
    sk_sp<SkImage> cpuImage;
    bool usingCPURendering = !context;
    
    // 帧率控制
    const int TARGET_FPS = 60;
    const int FRAME_DELAY = 1000 / TARGET_FPS;  // ~16ms
    Uint32 frameStart;

    while (running) {
        frameStart = SDL_GetTicks();
        // 事件处理 (替代 peekmessage)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                break;
            }
            
            // 跳过 MOUSEMOTION 提高性能 (如原版)
            if (event.type == SDL_MOUSEMOTION) {
                continue;
            }
            
            // 分发事件到窗口管理器
            wm.DispatchEvent(event);
        }

        // ========================================================
        // 5. 渲染
        // ========================================================
        
        // 处理待切换的窗口（在渲染前）
        wm.ProcessPendingWindowSwitch();
        
        // 清空画布并重置所有状态
        canvas->save();
        canvas->clear(SK_ColorWHITE);
        
        // 绘制所有窗口和控件
        wm.Redraw();
        
        canvas->restore();
        
        // 刷新到屏幕
        if (context) {
            // GPU渲染 - flush + swap
            context->flushAndSubmit();
            SDL_GL_SwapWindow(window);
        } else {
            // CPU渲染 - 需要手动传输到窗口
            cpuImage = surface->makeImageSnapshot();
            if (cpuImage) {
                SkPixmap pixmap;
                if (cpuImage->peekPixels(&pixmap)) {
                    SDL_Surface* sdlSurface = SDL_CreateRGBSurfaceFrom(
                        (void*)pixmap.addr(),
                        WINDOW_WIDTH, WINDOW_HEIGHT,
                        32, pixmap.rowBytes(),
                        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
                    );
                    
                    if (sdlSurface) {
                        // CPU渲染模式下窗口显示受限
                        static bool warned = false;
                        if (!warned) {
                            std::cout << "⚠️  CPU 渲染模式下窗口显示受限，建议在支持 OpenGL 的环境运行" << std::endl;
                            warned = true;
                        }
                        SDL_FreeSurface(sdlSurface);
                    }
                }
            }
            SDL_GL_SwapWindow(window);
        }
        
        // 帧率控制 - 限制到60 FPS
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    // ========================================================
    // 6. 清理资源
    // ========================================================
    std::cout << "程序退出中..." << std::endl;
    
    surface.reset();
    context.reset();
    
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
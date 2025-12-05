# DaysOfJFE

最新**垃圾代码**力作.里面的一些功能可能是用了一些奇奇怪怪的方法实现的，大佬要喷轻点
同时可能有部分我不再使用的代码没删掉，无视即可

## 这一步计划

用`Skia` + `GLFW`重写该项目

## 简述

本分支为技术验证+重构工作分支，旨在使用更现代的技术实现我的理想

## 运行流程

### 初始化阶段
- **GLFW窗口初始化**: 创建1600×900的窗口，设置OpenGL上下文（未来应该会添加自适应，不过锁定16:9）
- **Skia渲染器初始化**: 初始化Skia的Ganesh后端，支持硬件加速渲染
- **字体管理器初始化**: 目前来说会加载Sarasa UI SC和Source Han Serif SC字体
- **窗口管理器创建**: 管理多个窗口的切换和渲染

### 应用架构
- **ApplicationContext**: 全局应用上下文，包含：
  - 音频系统（BGM、语音、音效播放器，三个通道）
  - 事件管理器
  - 窗口管理器引用
- **WindowManager**: 窗口管理系统，支持：
  - 多窗口切换（首页、游戏、加载、场景选择、设置）
  - 分层控件管理（背景层、中景层、前景层）
  - 事件分发

### 窗口系统
- **WindowPanel基类**: 所有窗口的基类，提供：
  - 分层控件管理
  - 背景图像设置
  - 事件处理钩子
  - 自定义背景绘制
- **具体窗口实现**:
  - `HomeWindow`: 首页窗口
  - `GameWindow`: 游戏主窗口，处理脚本执行和场景渲染
  - `LoadingWindow`: 加载窗口
  - `LoadSceneWindow`: 场景选择窗口
  - `SettingWindow`: 设置窗口

### 渲染系统
- **IRenderer接口**: 定义统一的2D渲染接口
- **SkiaRenderer实现**: 使用Skia库实现所有渲染操作：
  - 绘制矩形、圆角矩形、圆形、线条
  - 文本渲染
  - 图像绘制
  - 字体管理
- **渲染循环**:
  1. 处理GLFW事件
  2. 检查帧缓冲区大小变化
  3. 开始帧渲染
  4. 窗口管理器重绘当前窗口
  5. 结束帧渲染，交换缓冲区

### 事件处理
- **GlfwPlatform**: GLFW事件到平台事件的转换
- **PlatformEvent**: 统一的事件数据结构
- **事件分发流程**:
  GLFW事件 → GlfwPlatform → EventManager → WindowManager → 当前WindowPanel → 控件处理

### 脚本系统
- **YAML脚本格式**: 使用YAML定义游戏内容
- **GameScript类**: 脚本解析和执行
- **脚本结构**:
  - 角色定义（ID、位置、立绘图）
  - 场景（背景、角色实例、脚本序列）
  - 对话（角色、显示名、文本、立绘状态）
  - 命令（类型、动作、参数）
- **支持的命令类型**:
  - `bg change`: 切换背景
  - `bg title`: 显示标题
  - `character show/hide`: 显示/隐藏角色
  - `sprite change`: 切换角色立绘
  - `chapter drop`: 跳转到指定章节

### 资源管理
- **Assets模块**: 统一管理游戏资源
- **资源目录结构**:
  - `assets/fonts/`: 字体文件
  - `assets/image/`: 图像资源
  - `assets/scripts/`: YAML脚本文件
  - `assets/sound/`: 音频资源

## 构建
- **CMake构建**: 支持跨平台构建
- **vcpkg依赖管理**:
  - GLFW: 窗口和输入管理
  - Skia: 2D图形渲染
  - yaml-cpp: YAML脚本解析
  - OpenAL: 音频播放
  - glad: OpenGL加载器
- **编译选项**: C++20标准
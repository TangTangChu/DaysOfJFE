# Days of JFE - Linux Edition

基于原版Windows游戏的Linux移植版本。

## 构建指南

### 1. 安装依赖

```bash
sudo pacman -S cmake gcc sdl2 sdl2_mixer yaml-cpp mesa
```

### 2. 获取Skia库

本项目需要自行编译Skia库：

```bash
# 1. Clone Skia项目
git clone https://skia.googlesource.com/skia.git
cd skia
python3 tools/git-sync-deps

# 2. 编译Skia（Release模式）
bin/gn gen out/Release --args='is_official_build=true'
ninja -C out/Release

# 3. 拷贝到项目目录
mkdir -p /path/to/hajimi/libs/skia/lib
mkdir -p /path/to/hajimi/libs/skia/include
mkdir -p /path/to/hajimi/libs/skia/modules

cp out/Release/libskia.a /path/to/hajimi/libs/skia/lib/
cp -r include/* /path/to/hajimi/libs/skia/include/
cp -r modules/* /path/to/hajimi/libs/skia/modules/
```

### 3. 编译项目

```bash
# 进入项目根目录
cd /path/to/hajimi

# 创建构建目录
mkdir -p build
cd build

# 配置CMake
cmake ..

# 编译（使用所有CPU核心）
make -j$(nproc)
```

### 4. 运行游戏

```bash
cd build
./DaysOfJFE
```


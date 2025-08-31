@echo off
echo Qt Software Manager - 构建脚本
echo =============================

REM 检查是否安装了CMake
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo 错误: 未找到CMake。请先安装CMake。
    pause
    exit /b 1
)

REM 检查是否安装了Qt
qmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo 警告: 未找到Qt。请确保已安装Qt 6.9或更高版本。
)

REM 创建构建目录
if not exist build mkdir build
cd build

echo.
echo 正在配置项目...
cmake .. -G "MinGW Makefiles"

if %errorlevel% neq 0 (
    echo 错误: CMake配置失败。
    cd ..
    pause
    exit /b 1
)

echo.
echo 正在编译项目...
cmake --build .

if %errorlevel% neq 0 (
    echo 错误: 编译失败。
    cd ..
    pause
    exit /b 1
)

echo.
echo 构建完成！可执行文件位于 build/bin/ 目录中。
cd ..

pause
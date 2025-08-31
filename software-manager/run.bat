@echo off
echo Qt Software Manager - 运行脚本
echo =============================

REM 检查构建目录是否存在
if not exist build (
    echo 错误: 未找到构建目录。请先运行 build.bat 构建项目。
    pause
    exit /b 1
)

cd build

REM 检查可执行文件是否存在
if not exist bin\QtSoftwareManager.exe (
    echo 错误: 未找到可执行文件。请先运行 build.bat 构建项目。
    cd ..
    pause
    exit /b 1
)

echo 正在启动 Qt Software Manager...
echo.

bin\QtSoftwareManager.exe

cd ..

pause
# Qt 软件管家 (Qt Software Manager)

Qt 软件管家是一款基于 Qt 6.9 框架开发的桌面应用程序，旨在为用户提供高效管理、分类和快速启动计算机软件的解决方案。

## 功能特性

- **自动扫描**: 自动扫描系统中的快捷方式和可执行文件
- **手动添加**: 支持手动添加软件到管理器中
- **智能分类**: 提供分类管理功能，可创建、编辑、删除分类
- **直观展示**: 网格视图和列表视图两种方式展示软件
- **快速启动**: 一键启动软件，提升使用效率
- **拖拽操作**: 支持拖拽方式分配软件到不同分类
- **右键菜单**: 提供丰富的右键菜单功能
- **全局快捷键**: 支持全局快捷键快速操作
- **系统托盘**: 支持最小化到系统托盘后台运行
- **数据持久化**: 使用 SQLite 数据库存储软件信息和分类关系

## 技术栈

- **编程语言**: C++17
- **GUI框架**: Qt 6.9
- **构建系统**: CMake 3.16+
- **数据持久化**: SQLite 数据库
- **图标处理**: Qt QIcon/QFileIconProvider
- **全局快捷键**: QHotkey 库

## 项目结构

```
QtSoftwareManager/
├── CMakeLists.txt
├── README.md
├── LICENSE
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── SoftwareScanner.hpp/.cpp
│   │   ├── CategoryManager.hpp/.cpp
│   │   ├── SettingsManager.hpp/.cpp
│   │   ├── SystemTrayManager.hpp/.cpp
│   │   ├── GlobalHotkeyManager.hpp/.cpp
│   │   └── DatabaseManager.hpp/.cpp
│   ├── model/
│   │   ├── SoftwareItem.hpp/.cpp
│   ├── ui/
│   │   ├── MainWindow.hpp/.cpp
│   │   ├── SidebarWidget.hpp/.cpp
│   │   ├── SoftwareGridView.hpp/.cpp
│   │   ├── SoftwareListView.hpp/.cpp
│   │   ├── SearchDialog.hpp/.cpp
│   │   └── SoftwareItemWidget.hpp/.cpp
│   └── utils/
│       ├── IconExtractor.hpp/.cpp
├── resources/
│   ├── Resources.qrc
│   └── icons/
├── data/
│   └── software.db
└── tests/
    ├── TestSoftwareItem.cpp
    ├── TestCategoryManager.cpp
    └── TestSoftwareScanner.cpp
```

## 构建说明

### 环境要求

- CMake 3.16 或更高版本
- Qt 6.9 或更高版本
- C++17 编译器支持

### 构建步骤

1. 克隆项目仓库
```bash
git clone <repository-url>
cd QtSoftwareManager
```

2. 创建构建目录
```bash
mkdir build
cd build
```

3. 配置项目
```bash
cmake ..
```

4. 编译项目
```bash
cmake --build .
```

5. 运行应用程序
```bash
./QtSoftwareManager
```

## 使用说明

1. **首次启动**: 应用程序会自动扫描系统中的软件快捷方式
2. **分类管理**: 在左侧边栏可以创建、重命名和删除分类
3. **软件操作**: 
   - 单击软件图标选中软件
   - 双击软件图标启动软件
   - 右键点击软件图标显示操作菜单
4. **视图切换**: 使用工具栏按钮在网格视图和列表视图之间切换
5. **搜索功能**: 使用 Ctrl+Shift+F 快捷键或工具栏搜索按钮查找软件

## 快捷键

- **Ctrl+W**: 显示/隐藏主窗口
- **Ctrl+Shift+F**: 快速搜索软件
- **Ctrl+Q**: 退出应用程序
- **Ctrl+R**: 重新扫描系统软件
- **Ctrl+N**: 新建分类

## 开发计划

### 第一阶段: 基础框架搭建
- [x] 创建项目结构和CMakeLists.txt配置
- [x] 实现基础的MainWindow和UI组件
- [x] 实现SoftwareItem数据模型

### 第二阶段: 核心功能开发
- [x] 实现软件扫描功能(SoftwareScanner)
- [x] 实现分类管理功能(CategoryManager)
- [x] 实现数据库存储功能(DatabaseManager)

### 第三阶段: 高级功能开发
- [x] 实现系统托盘功能(SystemTrayManager)
- [x] 实现全局快捷键功能(GlobalHotkeyManager)
- [x] 实现图标提取功能(IconExtractor)

### 第四阶段: 完善与测试
- [ ] 完善UI交互和用户体验
- [ ] 实现完整的测试套件
- [ ] 性能优化和bug修复

## 贡献指南

欢迎提交 Issue 和 Pull Request 来帮助改进这个项目。

## 许可证

本项目采用 [MIT License](LICENSE) 开源许可证。
cmake_minimum_required(VERSION 3.16)
project(QtSoftwareManager VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置模块路径
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql Test Gui)

# 启用自动MOC、RCC和UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/core)
include_directories(${CMAKE_SOURCE_DIR}/src/model)
include_directories(${CMAKE_SOURCE_DIR}/src/ui)
include_directories(${CMAKE_SOURCE_DIR}/src/utils)
include_directories(${CMAKE_SOURCE_DIR}/src/qhotkey)

# 定义源文件
set(SOURCES
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/SidebarWidget.cpp
    src/ui/SoftwareGridView.cpp
    src/ui/SoftwareListView.cpp
    src/ui/SoftwareItemWidget.cpp
    src/ui/SearchDialog.cpp
    src/ui/SettingsDialog.cpp
    src/core/SoftwareScanner.cpp
    src/core/CategoryManager.cpp
    src/core/SettingsManager.cpp
    src/core/SystemTrayManager.cpp
    src/core/GlobalHotkeyManager.cpp
    src/core/DatabaseManager.cpp
    src/model/SoftwareItem.cpp
    src/utils/IconExtractor.cpp
    src/utils/Logging.cpp
    src/qhotkey/qhotkey.cpp
    src/qhotkey/qhotkey_win.cpp
)

# 定义头文件
set(HEADERS
    src/ui/MainWindow.hpp
    src/ui/SidebarWidget.hpp
    src/ui/SoftwareGridView.hpp
    src/ui/SoftwareListView.hpp
    src/ui/SoftwareItemWidget.hpp
    src/ui/SearchDialog.hpp
    src/ui/SettingsDialog.hpp
    src/core/SoftwareScanner.hpp
    src/core/CategoryManager.hpp
    src/core/SettingsManager.hpp
    src/core/SystemTrayManager.hpp
    src/core/GlobalHotkeyManager.hpp
    src/core/DatabaseManager.hpp
    src/model/SoftwareItem.hpp
    src/utils/IconExtractor.hpp
    src/utils/Logging.hpp
    src/qhotkey/qhotkey.h
    src/qhotkey/qhotkey_p.h
)

# 添加资源文件
set(RESOURCES
    resources/Resources.qrc
)

# 添加可执行文件
add_executable(QtSoftwareManager
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# 链接Qt库
target_link_libraries(QtSoftwareManager 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Sql
)

# 设置输出目录
set_target_properties(QtSoftwareManager PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 创建测试可执行文件
add_executable(TestSoftwareItem tests/TestSoftwareItem.cpp src/model/SoftwareItem.cpp src/utils/Logging.cpp)
target_link_libraries(TestSoftwareItem Qt6::Core Qt6::Test Qt6::Gui Qt6::Widgets)

add_executable(TestCategoryManager tests/TestCategoryManager.cpp src/core/CategoryManager.cpp src/utils/Logging.cpp)
target_link_libraries(TestCategoryManager Qt6::Core Qt6::Test Qt6::Gui Qt6::Widgets)

add_executable(TestSoftwareScanner tests/TestSoftwareScanner.cpp src/core/SoftwareScanner.cpp src/model/SoftwareItem.cpp src/utils/Logging.cpp)
target_link_libraries(TestSoftwareScanner Qt6::Core Qt6::Test Qt6::Gui Qt6::Widgets)

add_executable(TestDatabaseManager tests/TestDatabaseManager.cpp src/core/DatabaseManager.cpp src/model/SoftwareItem.cpp src/utils/Logging.cpp)
target_link_libraries(TestDatabaseManager Qt6::Core Qt6::Sql Qt6::Test Qt6::Gui Qt6::Widgets)

# 启用测试
enable_testing()

# 添加测试
add_test(NAME TestSoftwareItem COMMAND TestSoftwareItem)
add_test(NAME TestCategoryManager COMMAND TestCategoryManager)
add_test(NAME TestSoftwareScanner COMMAND TestSoftwareScanner)
add_test(NAME TestDatabaseManager COMMAND TestDatabaseManager)

# 安装规则
install(TARGETS QtSoftwareManager
    RUNTIME DESTINATION bin
)

# CPack配置（用于打包）
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "QtSoftwareManager")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Qt Software Manager")
include(CPack)
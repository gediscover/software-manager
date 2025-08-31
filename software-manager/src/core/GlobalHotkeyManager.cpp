#include "GlobalHotkeyManager.hpp"
#include "ui/MainWindow.hpp"
#include <QSettings>
#include "../utils/Logging.hpp"

// 注意：这里需要QHotkey库支持
// 由于QHotkey是一个第三方库，我们需要在项目中集成它
// 暂时使用前向声明，实际使用时需要包含QHotkey头文件
class QHotkey;

GlobalHotkeyManager::GlobalHotkeyManager(MainWindow* parent)
    : QObject(parent)
    , m_mainWindow(parent)
{
    // 初始化默认快捷键
    initializeDefaultHotkeys();
}

GlobalHotkeyManager::~GlobalHotkeyManager()
{
    unregisterAllHotkeys();
}

bool GlobalHotkeyManager::registerHotkey(const QKeySequence& sequence, const QString& name)
{
    // 这里需要QHotkey库的实际实现
    // 暂时返回true表示成功
    Q_UNUSED(sequence)
    Q_UNUSED(name)
    
    emit hotkeyRegistered(name);
    return true;
}

void GlobalHotkeyManager::unregisterHotkey(const QString& name)
{
    // 这里需要QHotkey库的实际实现
    Q_UNUSED(name)
    
    emit hotkeyUnregistered(name);
}

void GlobalHotkeyManager::setHotkeyEnabled(const QString& name, bool enabled)
{
    // 这里需要QHotkey库的实际实现
    Q_UNUSED(name)
    Q_UNUSED(enabled)
}

bool GlobalHotkeyManager::isHotkeyRegistered(const QString& name) const
{
    return m_hotkeys.contains(name);
}

QMap<QString, QKeySequence> GlobalHotkeyManager::getRegisteredHotkeys() const
{
    QMap<QString, QKeySequence> hotkeys;
    // 这里需要返回实际注册的快捷键
    return hotkeys;
}

void GlobalHotkeyManager::initializeDefaultHotkeys()
{
    setupDefaultHotkeys();
}

void GlobalHotkeyManager::unregisterAllHotkeys()
{
    // 注销所有快捷键
    for (const QString& name : m_hotkeys.keys()) {
        unregisterHotkey(name);
    }
    m_hotkeys.clear();
}

void GlobalHotkeyManager::loadHotkeySettings()
{
    QSettings settings;
    // 加载快捷键设置
    // 实际实现需要从配置文件读取快捷键设置
}

void GlobalHotkeyManager::saveHotkeySettings()
{
    QSettings settings;
    // 保存快捷键设置
    // 实际实现需要将快捷键设置保存到配置文件
}

void GlobalHotkeyManager::onShowWindowHotkeyPressed()
{
    emit hotkeyPressed("show_window");
    
    if (m_mainWindow) {
        // 显示/隐藏主窗口
        // m_mainWindow->toggleVisibility();
    }
}

void GlobalHotkeyManager::onSearchHotkeyPressed()
{
    emit hotkeyPressed("search");
    
    if (m_mainWindow) {
        // 激活搜索功能
        // m_mainWindow->showSearchDialog();
    }
}

void GlobalHotkeyManager::onQuitHotkeyPressed()
{
    emit hotkeyPressed("quit");
    
    // 退出应用程序
    // qApp->quit();
}

void GlobalHotkeyManager::onScanHotkeyPressed()
{
    emit hotkeyPressed("scan");
    
    if (m_mainWindow) {
        // 重新扫描系统软件
        // m_mainWindow->scanSystemSoftware();
    }
}

void GlobalHotkeyManager::setupDefaultHotkeys()
{
    // 注册默认快捷键
    // Ctrl+W: 显示/隐藏主窗口
    // registerHotkey(QKeySequence("Ctrl+W"), "show_window");
    
    // Ctrl+Shift+F: 搜索
    // registerHotkey(QKeySequence("Ctrl+Shift+F"), "search");
    
    // Ctrl+Q: 退出
    // registerHotkey(QKeySequence("Ctrl+Q"), "quit");
    
    // Ctrl+R: 重新扫描
    // registerHotkey(QKeySequence("Ctrl+R"), "scan");
    
    qCInfo(softwareManager) << "默认快捷键设置完成";
}

QString GlobalHotkeyManager::getHotkeyDescription(const QString& name) const
{
    if (name == "show_window") {
        return "显示/隐藏主窗口";
    } else if (name == "search") {
        return "搜索软件";
    } else if (name == "quit") {
        return "退出应用程序";
    } else if (name == "scan") {
        return "重新扫描";
    }
    
    return name;
}
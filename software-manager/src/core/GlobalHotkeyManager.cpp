#include "GlobalHotkeyManager.hpp"
#include "ui/MainWindow.hpp"
#include <QSettings>
#include <QApplication>
#include "../utils/Logging.hpp"
#include "../qhotkey/qhotkey.h"

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
    // 检查是否已注册同名快捷键
    if (m_hotkeys.contains(name)) {
        unregisterHotkey(name);
    }
    
    // 创建QHotkey对象
    QHotkey* hotkey = new QHotkey(sequence, true, this);
    
    if (hotkey->isRegistered()) {
        // 连接信号槽
        if (name == "show_window") {
            connect(hotkey, &QHotkey::activated, this, &GlobalHotkeyManager::onShowWindowHotkeyPressed);
        } else if (name == "search") {
            connect(hotkey, &QHotkey::activated, this, &GlobalHotkeyManager::onSearchHotkeyPressed);
        } else if (name == "quit") {
            connect(hotkey, &QHotkey::activated, this, &GlobalHotkeyManager::onQuitHotkeyPressed);
        } else if (name == "scan") {
            connect(hotkey, &QHotkey::activated, this, &GlobalHotkeyManager::onScanHotkeyPressed);
        }
        
        // 存储快捷键对象
        m_hotkeys[name] = hotkey;
        
        emit hotkeyRegistered(name);
        qCInfo(softwareManager) << "成功注册快捷键:" << name << "序列:" << sequence.toString();
        return true;
    } else {
        delete hotkey;
        qCWarning(softwareManager) << "注册快捷键失败:" << name << "序列:" << sequence.toString();
        return false;
    }
}

void GlobalHotkeyManager::unregisterHotkey(const QString& name)
{
    if (m_hotkeys.contains(name)) {
        QHotkey* hotkey = m_hotkeys[name];
        hotkey->deleteLater();
        m_hotkeys.remove(name);
        emit hotkeyUnregistered(name);
        qCInfo(softwareManager) << "注销快捷键:" << name;
    }
}

void GlobalHotkeyManager::setHotkeyEnabled(const QString& name, bool enabled)
{
    if (m_hotkeys.contains(name)) {
        m_hotkeys[name]->setRegistered(enabled);
    }
}

bool GlobalHotkeyManager::isHotkeyRegistered(const QString& name) const
{
    return m_hotkeys.contains(name) && m_hotkeys[name]->isRegistered();
}

QMap<QString, QKeySequence> GlobalHotkeyManager::getRegisteredHotkeys() const
{
    QMap<QString, QKeySequence> hotkeys;
    for (auto it = m_hotkeys.constBegin(); it != m_hotkeys.constEnd(); ++it) {
        hotkeys[it.key()] = it.value()->shortcut();
    }
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
    settings.beginGroup("Hotkeys");
    
    // 加载快捷键设置
    QStringList keys = settings.childKeys();
    for (const QString& key : keys) {
        QString sequenceStr = settings.value(key).toString();
        if (!sequenceStr.isEmpty()) {
            QKeySequence sequence(sequenceStr);
            registerHotkey(sequence, key);
        }
    }
    
    settings.endGroup();
}

void GlobalHotkeyManager::saveHotkeySettings()
{
    QSettings settings;
    settings.beginGroup("Hotkeys");
    
    // 保存快捷键设置
    for (auto it = m_hotkeys.constBegin(); it != m_hotkeys.constEnd(); ++it) {
        settings.setValue(it.key(), it.value()->shortcut().toString());
    }
    
    settings.endGroup();
}

void GlobalHotkeyManager::onShowWindowHotkeyPressed()
{
    emit hotkeyPressed("show_window");
    
    if (m_mainWindow) {
        // 显示/隐藏主窗口
        if (m_mainWindow->isVisible()) {
            m_mainWindow->hide();
        } else {
            m_mainWindow->show();
            m_mainWindow->raise();
            m_mainWindow->activateWindow();
        }
    }
}

void GlobalHotkeyManager::onSearchHotkeyPressed()
{
    emit hotkeyPressed("search");
    
    if (m_mainWindow) {
        // 激活搜索功能
        // 这里需要MainWindow提供相应的公共方法
        // m_mainWindow->showSearchDialog();
    }
}

void GlobalHotkeyManager::onQuitHotkeyPressed()
{
    emit hotkeyPressed("quit");
    
    // 退出应用程序
    qApp->quit();
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
    registerHotkey(QKeySequence("Ctrl+W"), "show_window");
    
    // Ctrl+Shift+F: 搜索
    registerHotkey(QKeySequence("Ctrl+Shift+F"), "search");
    
    // Ctrl+Q: 退出
    registerHotkey(QKeySequence("Ctrl+Q"), "quit");
    
    // Ctrl+R: 重新扫描
    registerHotkey(QKeySequence("Ctrl+R"), "scan");
    
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
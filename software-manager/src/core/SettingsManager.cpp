#include "SettingsManager.hpp"
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(softwareManager, "softwaremanager")

SettingsManager::SettingsManager(QObject* parent)
    : QObject(parent)
    , m_windowSize(1000, 700)
    , m_windowPosition(100, 100)
    , m_windowMaximized(false)
    , m_autoScanEnabled(true)
    , m_viewMode("grid")
    , m_iconSize(64)
    , m_defaultCategory("未分类")
    , m_minimizeToTray(true)
    , m_closeToTray(true)
{
    loadSettings();
}

void SettingsManager::setWindowSize(const QSize& size)
{
    m_windowSize = size;
}

QSize SettingsManager::windowSize() const
{
    return m_windowSize;
}

void SettingsManager::setWindowPosition(const QPoint& position)
{
    m_windowPosition = position;
}

QPoint SettingsManager::windowPosition() const
{
    return m_windowPosition;
}

void SettingsManager::setWindowMaximized(bool maximized)
{
    m_windowMaximized = maximized;
}

bool SettingsManager::windowMaximized() const
{
    return m_windowMaximized;
}

void SettingsManager::setScanPaths(const QStringList& paths)
{
    m_scanPaths = paths;
}

QStringList SettingsManager::scanPaths() const
{
    return m_scanPaths;
}

void SettingsManager::setAutoScanEnabled(bool enabled)
{
    m_autoScanEnabled = enabled;
}

bool SettingsManager::autoScanEnabled() const
{
    return m_autoScanEnabled;
}

void SettingsManager::setViewMode(const QString& mode)
{
    m_viewMode = mode;
}

QString SettingsManager::viewMode() const
{
    return m_viewMode;
}

void SettingsManager::setIconSize(int size)
{
    m_iconSize = size;
}

int SettingsManager::iconSize() const
{
    return m_iconSize;
}

void SettingsManager::setHotkey(const QString& action, const QString& sequence)
{
    m_hotkeys[action] = sequence;
}

QString SettingsManager::hotkey(const QString& action) const
{
    return m_hotkeys.value(action, QString());
}

QMap<QString, QString> SettingsManager::allHotkeys() const
{
    return m_hotkeys;
}

void SettingsManager::setDefaultCategory(const QString& category)
{
    m_defaultCategory = category;
}

QString SettingsManager::defaultCategory() const
{
    return m_defaultCategory;
}

void SettingsManager::setMinimizeToTray(bool enabled)
{
    m_minimizeToTray = enabled;
}

bool SettingsManager::minimizeToTray() const
{
    return m_minimizeToTray;
}

void SettingsManager::setCloseToTray(bool enabled)
{
    m_closeToTray = enabled;
}

bool SettingsManager::closeToTray() const
{
    return m_closeToTray;
}

void SettingsManager::saveSettings()
{
    QSettings settings;
    
    // 保存窗口设置
    settings.setValue("Window/Size", m_windowSize);
    settings.setValue("Window/Position", m_windowPosition);
    settings.setValue("Window/Maximized", m_windowMaximized);
    
    // 保存扫描设置
    settings.setValue("Scan/Paths", m_scanPaths);
    settings.setValue("Scan/AutoScan", m_autoScanEnabled);
    
    // 保存视图设置
    settings.setValue("View/Mode", m_viewMode);
    settings.setValue("View/IconSize", m_iconSize);
    
    // 保存快捷键设置
    settings.beginGroup("Hotkeys");
    for (auto it = m_hotkeys.constBegin(); it != m_hotkeys.constEnd(); ++it) {
        settings.setValue(it.key(), it.value());
    }
    settings.endGroup();
    
    // 保存分类设置
    settings.setValue("Category/Default", m_defaultCategory);
    
    // 保存系统托盘设置
    settings.setValue("Tray/MinimizeToTray", m_minimizeToTray);
    settings.setValue("Tray/CloseToTray", m_closeToTray);
    
    qCInfo(softwareManager) << "设置已保存";
}

void SettingsManager::loadSettings()
{
    QSettings settings;
    
    // 加载窗口设置
    m_windowSize = settings.value("Window/Size", m_windowSize).toSize();
    m_windowPosition = settings.value("Window/Position", m_windowPosition).toPoint();
    m_windowMaximized = settings.value("Window/Maximized", m_windowMaximized).toBool();
    
    // 加载扫描设置
    m_scanPaths = settings.value("Scan/Paths", m_scanPaths).toStringList();
    m_autoScanEnabled = settings.value("Scan/AutoScan", m_autoScanEnabled).toBool();
    
    // 加载视图设置
    m_viewMode = settings.value("View/Mode", m_viewMode).toString();
    m_iconSize = settings.value("View/IconSize", m_iconSize).toInt();
    
    // 加载快捷键设置
    settings.beginGroup("Hotkeys");
    QStringList keys = settings.childKeys();
    for (const QString& key : keys) {
        m_hotkeys[key] = settings.value(key).toString();
    }
    settings.endGroup();
    
    // 加载分类设置
    m_defaultCategory = settings.value("Category/Default", m_defaultCategory).toString();
    
    // 加载系统托盘设置
    m_minimizeToTray = settings.value("Tray/MinimizeToTray", m_minimizeToTray).toBool();
    m_closeToTray = settings.value("Tray/CloseToTray", m_closeToTray).toBool();
    
    qCInfo(softwareManager) << "设置已加载";
}
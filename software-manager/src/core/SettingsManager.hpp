#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QStringList>
#include <QSize>
#include <QPoint>

class SettingsManager : public QObject {
    Q_OBJECT

public:
    explicit SettingsManager(QObject* parent = nullptr);
    
    // 窗口设置
    void setWindowSize(const QSize& size);
    QSize windowSize() const;
    
    void setWindowPosition(const QPoint& position);
    QPoint windowPosition() const;
    
    void setWindowMaximized(bool maximized);
    bool windowMaximized() const;
    
    // 扫描设置
    void setScanPaths(const QStringList& paths);
    QStringList scanPaths() const;
    
    void setAutoScanEnabled(bool enabled);
    bool autoScanEnabled() const;
    
    // 视图设置
    void setViewMode(const QString& mode);  // "grid" 或 "list"
    QString viewMode() const;
    
    void setIconSize(int size);
    int iconSize() const;
    
    // 快捷键设置
    void setHotkey(const QString& action, const QString& sequence);
    QString hotkey(const QString& action) const;
    QMap<QString, QString> allHotkeys() const;
    
    // 分类设置
    void setDefaultCategory(const QString& category);
    QString defaultCategory() const;
    
    // 系统托盘设置
    void setMinimizeToTray(bool enabled);
    bool minimizeToTray() const;
    
    void setCloseToTray(bool enabled);
    bool closeToTray() const;
    
    // 保存和加载设置
    void saveSettings();
    void loadSettings();
    
private:
    // 窗口设置
    QSize m_windowSize;
    QPoint m_windowPosition;
    bool m_windowMaximized;
    
    // 扫描设置
    QStringList m_scanPaths;
    bool m_autoScanEnabled;
    
    // 视图设置
    QString m_viewMode;
    int m_iconSize;
    
    // 快捷键设置
    QMap<QString, QString> m_hotkeys;
    
    // 分类设置
    QString m_defaultCategory;
    
    // 系统托盘设置
    bool m_minimizeToTray;
    bool m_closeToTray;
};

#endif // SETTINGSMANAGER_H
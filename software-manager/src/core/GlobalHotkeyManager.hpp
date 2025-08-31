#ifndef GLOBALHOTKEYMANAGER_H
#define GLOBALHOTKEYMANAGER_H

#include <QObject>
#include <QKeySequence>
#include <QMap>

class QHotkey;
class MainWindow;

class GlobalHotkeyManager : public QObject {
    Q_OBJECT

public:
    explicit GlobalHotkeyManager(MainWindow* parent = nullptr);
    ~GlobalHotkeyManager();
    
    // 快捷键管理
    bool registerHotkey(const QKeySequence& sequence, const QString& name);
    void unregisterHotkey(const QString& name);
    void setHotkeyEnabled(const QString& name, bool enabled);
    bool isHotkeyRegistered(const QString& name) const;
    
    // 批量操作
    QMap<QString, QKeySequence> getRegisteredHotkeys() const;
    void initializeDefaultHotkeys();
    void unregisterAllHotkeys();
    
    // 配置管理
    void loadHotkeySettings();
    void saveHotkeySettings();
    
signals:
    void hotkeyPressed(const QString& name);
    void hotkeyRegistered(const QString& name);
    void hotkeyUnregistered(const QString& name);
    
private slots:
    void onShowWindowHotkeyPressed();
    void onSearchHotkeyPressed();
    void onQuitHotkeyPressed();
    void onScanHotkeyPressed();
    
private:
    QMap<QString, QHotkey*> m_hotkeys;
    MainWindow* m_mainWindow;
    
    // 私有方法
    void setupDefaultHotkeys();
    QString getHotkeyDescription(const QString& name) const;
};

#endif // GLOBALHOTKEYMANAGER_H
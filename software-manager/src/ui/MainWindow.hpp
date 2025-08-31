#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QStackedWidget>
#include <QToolBar>
#include <QStatusBar>

// 前置声明
class SidebarWidget;
class SoftwareGridView;
class SoftwareListView;
class SoftwareScanner;
class CategoryManager;
class SystemTrayManager;
class GlobalHotkeyManager;
class SearchDialog;
class SettingsDialog;
class DatabaseManager;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
    // 公共方法供其他组件调用
    void showSearchDialog();
    void scanSystemSoftware();
    
    // 添加获取数据库管理器的方法
    DatabaseManager* databaseManager() const;
    
protected:
    void closeEvent(QCloseEvent* event) override;
    
private slots:
    // UI事件处理
    void onScanButtonClicked();
    void onAddButtonClicked();
    void onSettingsButtonClicked();
    void onCategorySelected(const QString& category);
    void onSearchTriggered();
    void onViewModeChanged(bool isGridMode);
    
    // 系统托盘事件
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    
    // 快捷键事件
    void onShowWindowHotkeyPressed();
    void onSearchHotkeyPressed();
    
    // 扫描完成事件
    void onScanFinished(const QList<class SoftwareItem>& items);
    
    // 软件项事件
    void onSoftwareItemLaunched(const QString& softwareId);
    void onSoftwareItemRemoved(const QString& softwareId);
    
private:
    // UI组件
    SidebarWidget* m_sidebar;
    QStackedWidget* m_viewStack;
    SoftwareGridView* m_gridView;
    SoftwareListView* m_listView;
    QToolBar* m_toolbar;
    QStatusBar* m_statusbar;
    
    // 核心管理器
    SoftwareScanner* m_scanner;
    CategoryManager* m_categoryManager;
    SystemTrayManager* m_trayManager;
    GlobalHotkeyManager* m_hotkeyManager;
    DatabaseManager* m_databaseManager;
    
    // 对话框
    SearchDialog* m_searchDialog;
    SettingsDialog* m_settingsDialog;
    
    // 私有方法
    void setupUI();
    void setupConnections();
    void setupTrayIcon();
    void setupHotkeys();
    void loadSettings();
    void saveSettings();
    void updateSoftwareList(const QString& category = QString());
    
    // 软件管理方法
    void addSoftwareManually();
    void launchSoftware(const QString& softwareId);
    void removeSoftware(const QString& softwareId);
};

#endif // MAINWINDOW_H
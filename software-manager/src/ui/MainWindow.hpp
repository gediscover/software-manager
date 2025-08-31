#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

// 前置声明
class SidebarWidget;
class SoftwareGridView;
class SoftwareListView;
class SoftwareScanner;
class CategoryManager;
class SystemTrayManager;
class GlobalHotkeyManager;
class SearchDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    
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
    
    // 对话框
    SearchDialog* m_searchDialog;
    
    // 私有方法
    void setupUI();
    void setupConnections();
    void setupTrayIcon();
    void setupHotkeys();
    void loadSettings();
    void saveSettings();
    void updateSoftwareList(const QString& category = QString());
};

#endif // MAINWINDOW_H
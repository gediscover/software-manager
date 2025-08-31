#include "MainWindow.hpp"
#include "SidebarWidget.hpp"
#include "SoftwareGridView.hpp"
#include "SoftwareListView.hpp"
#include "SearchDialog.hpp"
#include "../core/SoftwareScanner.hpp"
#include "../core/CategoryManager.hpp"
#include "../core/SystemTrayManager.hpp"
#include "../core/GlobalHotkeyManager.hpp"
#include "../model/SoftwareItem.hpp"
#include <QToolBar>
#include <QStatusBar>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QSettings>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(softwareManager, "softwaremanager")

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_sidebar(nullptr)
    , m_viewStack(nullptr)
    , m_gridView(nullptr)
    , m_listView(nullptr)
    , m_toolbar(nullptr)
    , m_statusbar(nullptr)
    , m_scanner(nullptr)
    , m_categoryManager(nullptr)
    , m_trayManager(nullptr)
    , m_hotkeyManager(nullptr)
    , m_searchDialog(nullptr)
{
    setupUI();
    setupConnections();
    setupTrayIcon();
    setupHotkeys();
    loadSettings();
    
    // 设置窗口标题和大小
    setWindowTitle("Qt 软件管家");
    resize(1000, 700);
    
    // 设置状态栏
    m_statusbar->showMessage("就绪");
    
    qCInfo(softwareManager) << "主窗口初始化完成";
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // 最小化到系统托盘而不是关闭
    if (m_trayManager && m_trayManager->isTrayAvailable()) {
        hide();
        m_trayManager->showNotification("Qt 软件管家", "程序已在后台运行");
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::onScanButtonClicked()
{
    if (m_scanner) {
        m_scanner->scanSystemSoftware();
        m_statusbar->showMessage("正在扫描系统软件...");
    }
}

void MainWindow::onAddButtonClicked()
{
    m_statusbar->showMessage("添加软件功能待实现");
    // 实现手动添加软件功能
}

void MainWindow::onSettingsButtonClicked()
{
    m_statusbar->showMessage("设置功能待实现");
    // 实现设置对话框
}

void MainWindow::onCategorySelected(const QString& category)
{
    updateSoftwareList(category);
    m_statusbar->showMessage(QString("显示分类: %1").arg(category));
}

void MainWindow::onSearchTriggered()
{
    if (!m_searchDialog) {
        m_searchDialog = new SearchDialog(this);
    }
    
    m_searchDialog->show();
    m_searchDialog->raise();
    m_searchDialog->activateWindow();
}

void MainWindow::onViewModeChanged(bool isGridMode)
{
    if (m_viewStack) {
        m_viewStack->setCurrentIndex(isGridMode ? 0 : 1);
        m_statusbar->showMessage(isGridMode ? "网格视图" : "列表视图");
    }
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    Q_UNUSED(reason)
    // 托盘图标激活处理在SystemTrayManager中完成
}

void MainWindow::onShowWindowHotkeyPressed()
{
    // 显示/隐藏窗口
    if (isVisible()) {
        hide();
    } else {
        show();
        raise();
        activateWindow();
    }
}

void MainWindow::onSearchHotkeyPressed()
{
    onSearchTriggered();
}

void MainWindow::onScanFinished(const QList<SoftwareItem>& items)
{
    Q_UNUSED(items)
    m_statusbar->showMessage(QString("扫描完成，发现 %1 个软件").arg(items.size()));
    
    // 更新软件列表显示
    updateSoftwareList();
}

void MainWindow::setupUI()
{
    // 创建中心部件
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // 创建分割器
    QSplitter* splitter = new QSplitter(centralWidget);
    mainLayout->addWidget(splitter);
    
    // 创建侧边栏
    m_sidebar = new SidebarWidget(this);
    splitter->addWidget(m_sidebar);
    
    // 创建视图堆栈
    m_viewStack = new QStackedWidget(this);
    splitter->addWidget(m_viewStack);
    
    // 创建网格视图
    m_gridView = new SoftwareGridView(this);
    m_viewStack->addWidget(m_gridView);
    
    // 创建列表视图
    m_listView = new SoftwareListView(this);
    m_viewStack->addWidget(m_listView);
    
    // 设置分割器比例
    splitter->setSizes(QList<int>() << 200 << 800);
    
    // 创建工具栏
    m_toolbar = addToolBar("主工具栏");
    m_toolbar->setObjectName("mainToolbar");
    
    // 添加工具栏按钮
    QAction* scanAction = m_toolbar->addAction("扫描");
    connect(scanAction, &QAction::triggered, this, &MainWindow::onScanButtonClicked);
    
    QAction* addAction = m_toolbar->addAction("添加");
    connect(addAction, &QAction::triggered, this, &MainWindow::onAddButtonClicked);
    
    QAction* settingsAction = m_toolbar->addAction("设置");
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onSettingsButtonClicked);
    
    // 添加视图切换按钮
    m_toolbar->addSeparator();
    QAction* gridViewAction = m_toolbar->addAction("网格视图");
    QAction* listViewAction = m_toolbar->addAction("列表视图");
    
    // 连接视图切换信号
    connect(gridViewAction, &QAction::triggered, this, [this]() { onViewModeChanged(true); });
    connect(listViewAction, &QAction::triggered, this, [this]() { onViewModeChanged(false); });
    
    // 创建状态栏
    m_statusbar = statusBar();
    
    // 创建核心管理器
    m_scanner = new SoftwareScanner(this);
    m_categoryManager = new CategoryManager(this);
    m_trayManager = new SystemTrayManager(this);
    m_hotkeyManager = new GlobalHotkeyManager(this);
}

void MainWindow::setupConnections()
{
    // 连接侧边栏信号
    connect(m_sidebar, &SidebarWidget::categorySelected, 
            this, &MainWindow::onCategorySelected);
    
    // 连接扫描器信号
    connect(m_scanner, &SoftwareScanner::scanFinished, 
            this, &MainWindow::onScanFinished);
    connect(m_scanner, &SoftwareScanner::scanProgress, 
            this, [this](int progress) {
                m_statusbar->showMessage(QString("正在扫描... %1%").arg(progress));
            });
    connect(m_scanner, &SoftwareScanner::scanError, 
            this, [this](const QString& error) {
                m_statusbar->showMessage(QString("扫描错误: %1").arg(error));
            });
    
    // 连接系统托盘信号
    connect(m_trayManager, &SystemTrayManager::trayIconActivated, 
            this, &MainWindow::onTrayIconActivated);
}

void MainWindow::setupTrayIcon()
{
    if (m_trayManager) {
        m_trayManager->setupTrayIcon();
    }
}

void MainWindow::setupHotkeys()
{
    if (m_hotkeyManager) {
        m_hotkeyManager->initializeDefaultHotkeys();
    }
}

void MainWindow::loadSettings()
{
    QSettings settings;
    
    // 加载窗口几何信息
    QByteArray geometry = settings.value("MainWindow/Geometry").toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
    
    // 加载窗口状态
    QByteArray state = settings.value("MainWindow/State").toByteArray();
    if (!state.isEmpty()) {
        restoreState(state);
    }
}

void MainWindow::saveSettings()
{
    QSettings settings;
    
    // 保存窗口几何信息
    settings.setValue("MainWindow/Geometry", saveGeometry());
    
    // 保存窗口状态
    settings.setValue("MainWindow/State", saveState());
}

void MainWindow::updateSoftwareList(const QString& category)
{
    Q_UNUSED(category)
    // 更新软件列表显示
    // 根据选择的分类过滤软件列表
    m_statusbar->showMessage("软件列表已更新");
}
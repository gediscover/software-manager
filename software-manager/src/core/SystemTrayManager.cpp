#include "SystemTrayManager.hpp"
#include "ui/MainWindow.hpp"
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QStyle>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(softwareManager, "softwaremanager")

SystemTrayManager::SystemTrayManager(MainWindow* parent)
    : QObject(parent)
    , m_trayIcon(nullptr)
    , m_trayMenu(nullptr)
    , m_mainWindow(parent)
    , m_showAction(nullptr)
    , m_hideAction(nullptr)
    , m_quitAction(nullptr)
{
}

SystemTrayManager::~SystemTrayManager()
{
    if (m_trayIcon) {
        delete m_trayIcon;
    }
}

void SystemTrayManager::setupTrayIcon()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qCWarning(softwareManager) << "系统托盘不可用";
        return;
    }
    
    if (!m_trayIcon) {
        m_trayIcon = new QSystemTrayIcon(this);
        
        // 设置托盘图标
        QIcon icon = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
        m_trayIcon->setIcon(icon);
        
        // 连接信号槽
        connect(m_trayIcon, &QSystemTrayIcon::activated, 
                this, &SystemTrayManager::onTrayIconActivated);
        
        // 创建托盘菜单
        createTrayMenu();
        
        // 显示托盘图标
        m_trayIcon->show();
        
        qCInfo(softwareManager) << "托盘图标设置完成";
    }
}

void SystemTrayManager::setTrayIcon(const QIcon& icon)
{
    if (m_trayIcon) {
        m_trayIcon->setIcon(icon);
    }
}

bool SystemTrayManager::isTrayAvailable() const
{
    return QSystemTrayIcon::isSystemTrayAvailable() && m_trayIcon;
}

void SystemTrayManager::showNotification(const QString& title, 
                                       const QString& message,
                                       QSystemTrayIcon::MessageIcon icon,
                                       int millisecondsTimeoutHint)
{
    if (m_trayIcon && m_trayIcon->isVisible()) {
        m_trayIcon->showMessage(title, message, icon, millisecondsTimeoutHint);
        qCInfo(softwareManager) << "显示托盘通知:" << title << "-" << message;
    }
}

void SystemTrayManager::toggleMainWindow()
{
    if (m_mainWindow) {
        if (m_mainWindow->isVisible()) {
            m_mainWindow->hide();
        } else {
            m_mainWindow->show();
            m_mainWindow->raise();
            m_mainWindow->activateWindow();
        }
    }
}

void SystemTrayManager::setMainWindow(MainWindow* window)
{
    m_mainWindow = window;
}

void SystemTrayManager::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    emit trayIconActivated(reason);
    
    // 双击托盘图标显示/隐藏主窗口
    if (reason == QSystemTrayIcon::DoubleClick) {
        toggleMainWindow();
    }
}

void SystemTrayManager::onShowActionTriggered()
{
    if (m_mainWindow) {
        m_mainWindow->show();
        m_mainWindow->raise();
        m_mainWindow->activateWindow();
    }
}

void SystemTrayManager::onHideActionTriggered()
{
    if (m_mainWindow) {
        m_mainWindow->hide();
    }
}

void SystemTrayManager::onQuitActionTriggered()
{
    qApp->quit();
}

void SystemTrayManager::createTrayMenu()
{
    if (!m_trayIcon) {
        return;
    }
    
    if (m_trayMenu) {
        delete m_trayMenu;
    }
    
    m_trayMenu = new QMenu();
    
    // 创建菜单项
    m_showAction = m_trayMenu->addAction("显示主窗口");
    connect(m_showAction, &QAction::triggered, this, &SystemTrayManager::onShowActionTriggered);
    
    m_hideAction = m_trayMenu->addAction("隐藏主窗口");
    connect(m_hideAction, &QAction::triggered, this, &SystemTrayManager::onHideActionTriggered);
    
    m_trayMenu->addSeparator();
    
    m_quitAction = m_trayMenu->addAction("退出");
    connect(m_quitAction, &QAction::triggered, this, &SystemTrayManager::onQuitActionTriggered);
    
    // 设置托盘菜单
    m_trayIcon->setContextMenu(m_trayMenu);
}
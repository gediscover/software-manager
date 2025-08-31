#ifndef SYSTEMTRAYMANAGER_H
#define SYSTEMTRAYMANAGER_H

#include <QObject>
#include <QSystemTrayIcon>

class QMenu;
class MainWindow;

class SystemTrayManager : public QObject {
    Q_OBJECT

public:
    explicit SystemTrayManager(MainWindow* parent = nullptr);
    ~SystemTrayManager();
    
    // 托盘管理
    void setupTrayIcon();
    void setTrayIcon(const QIcon& icon);
    bool isTrayAvailable() const;
    
    // 通知管理
    void showNotification(const QString& title, 
                         const QString& message,
                         QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information,
                         int millisecondsTimeoutHint = 10000);
    
    // 窗口控制
    void toggleMainWindow();
    void setMainWindow(MainWindow* window);
    
signals:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    
private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onShowActionTriggered();
    void onHideActionTriggered();
    void onQuitActionTriggered();
    
private:
    void createTrayMenu();
    
    QSystemTrayIcon* m_trayIcon;
    QMenu* m_trayMenu;
    MainWindow* m_mainWindow;
    QAction* m_showAction;
    QAction* m_hideAction;
    QAction* m_quitAction;
};

#endif // SYSTEMTRAYMANAGER_H
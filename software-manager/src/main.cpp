#include <QApplication>
#include <QLoggingCategory>
#include "ui/MainWindow.hpp"

// 定义日志类别
Q_LOGGING_CATEGORY(softwareManager, "softwaremanager")

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    app.setApplicationName("Qt Software Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Qt Software Manager");
    app.setOrganizationDomain("qtsoftwaremanager.org");
    
    // 启用日志
    QLoggingCategory::setFilterRules("softwaremanager.debug=true");
    
    // 创建主窗口
    MainWindow window;
    window.show();
    
    // 记录启动日志
    qCInfo(softwareManager) << "Qt Software Manager 启动";
    
    return app.exec();
}
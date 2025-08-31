#include <QApplication>
#include <QLoggingCategory>
#include "utils/Logging.hpp"
#include "ui/MainWindow.hpp"

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
#include "SoftwareScanner.hpp"
#include "model/SoftwareItem.hpp"
#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QApplication>
#include <QFileIconProvider>
#include <QMimeDatabase>
#include <QMimeType>
#include "../utils/Logging.hpp"

SoftwareScanner::SoftwareScanner(QObject* parent)
    : QObject(parent)
    , m_isScanning(false)
    , m_workerThread(nullptr)
    , m_worker(nullptr)
{
    setupDefaultPaths();
}

SoftwareScanner::~SoftwareScanner()
{
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
        delete m_workerThread;
    }
}

void SoftwareScanner::scanSystemSoftware()
{
    if (m_isScanning) {
        qCWarning(softwareManager) << "扫描已在进行中";
        return;
    }
    
    m_isScanning = true;
    emit scanStarted();
    
    // 创建工作线程
    m_workerThread = new QThread(this);
    m_worker = new ScanWorker(m_scanPaths);
    
    // 移动到工作线程
    m_worker->moveToThread(m_workerThread);
    
    // 连接信号槽
    connect(m_workerThread, &QThread::started, m_worker, &ScanWorker::process);
    connect(m_worker, &ScanWorker::progress, this, &SoftwareScanner::scanProgress);
    connect(m_worker, &ScanWorker::finished, this, &SoftwareScanner::scanFinished);
    connect(m_worker, &ScanWorker::cancelled, this, &SoftwareScanner::scanCancelled);
    connect(m_worker, &ScanWorker::error, this, &SoftwareScanner::scanError);
    connect(m_worker, &ScanWorker::finished, m_workerThread, &QThread::quit);
    connect(m_worker, &ScanWorker::cancelled, m_workerThread, &QThread::quit);
    connect(m_workerThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(m_workerThread, &QThread::finished, m_workerThread, &QObject::deleteLater);
    
    // 启动线程
    m_workerThread->start();
    
    qCInfo(softwareManager) << "开始扫描系统软件，路径数量:" << m_scanPaths.size();
}

void SoftwareScanner::cancelScan()
{
    if (m_worker) {
        m_worker->cancel();
    }
}

bool SoftwareScanner::isCurrentlyScanning() const
{
    return m_isScanning;
}

void SoftwareScanner::addCustomPath(const QString& path)
{
    if (!m_scanPaths.contains(path)) {
        m_scanPaths.append(path);
        qCInfo(softwareManager) << "添加自定义扫描路径:" << path;
    }
}

void SoftwareScanner::removeCustomPath(const QString& path)
{
    m_scanPaths.removeAll(path);
    qCInfo(softwareManager) << "移除自定义扫描路径:" << path;
}

QStringList SoftwareScanner::getScanPaths() const
{
    return m_scanPaths;
}

void SoftwareScanner::setScanPaths(const QStringList& paths)
{
    m_scanPaths = paths;
    qCInfo(softwareManager) << "设置扫描路径，共" << paths.size() << "个路径";
}

SoftwareItem SoftwareScanner::createSoftwareItem(const QString& filePath)
{
    return SoftwareItem(filePath);
}

void SoftwareScanner::setupDefaultPaths()
{
#ifdef Q_OS_WIN
    // Windows系统默认路径
    m_scanPaths << "C:/ProgramData/Microsoft/Windows/Start Menu/Programs";
    m_scanPaths << QDir::homePath() + "/Desktop";
    m_scanPaths << QDir::homePath() + "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs";
#elif defined(Q_OS_MAC)
    // macOS系统默认路径
    m_scanPaths << "/Applications";
    m_scanPaths << QDir::homePath() + "/Applications";
    m_scanPaths << QDir::homePath() + "/Desktop";
#else
    // Linux系统默认路径
    m_scanPaths << "/usr/share/applications";
    m_scanPaths << QDir::homePath() + "/.local/share/applications";
    m_scanPaths << QDir::homePath() + "/Desktop";
#endif
    
    qCInfo(softwareManager) << "设置默认扫描路径";
}

QStringList SoftwareScanner::getDefaultScanPaths() const
{
    QStringList defaultPaths;
    
#ifdef Q_OS_WIN
    defaultPaths << "C:/ProgramData/Microsoft/Windows/Start Menu/Programs";
    defaultPaths << QDir::homePath() + "/Desktop";
#elif defined(Q_OS_MAC)
    defaultPaths << "/Applications";
    defaultPaths << QDir::homePath() + "/Applications";
    defaultPaths << QDir::homePath() + "/Desktop";
#else
    defaultPaths << "/usr/share/applications";
    defaultPaths << QDir::homePath() + "/.local/share/applications";
    defaultPaths << QDir::homePath() + "/Desktop";
#endif
    
    return defaultPaths;
}

bool SoftwareScanner::isValidExecutablePath(const QString& path) const
{
    QFileInfo fileInfo(path);
    
    // 检查文件是否存在
    if (!fileInfo.exists()) {
        return false;
    }
    
    // 检查是否为文件
    if (!fileInfo.isFile()) {
        return false;
    }
    
    // 检查文件扩展名
    QString suffix = fileInfo.suffix().toLower();
    
#ifdef Q_OS_WIN
    return suffix == "exe" || suffix == "lnk" || suffix == "bat" || suffix == "com";
#elif defined(Q_OS_MAC)
    return suffix == "app" || suffix == "command";
#else
    // Linux系统检查是否有执行权限
    return fileInfo.isExecutable() || suffix == "desktop";
#endif
}

// ScanWorker实现
ScanWorker::ScanWorker(const QStringList& paths, QObject* parent)
    : QObject(parent)
    , m_paths(paths)
    , m_cancelled(false)
{
}

void ScanWorker::process()
{
    QList<SoftwareItem> items;
    
    int totalPaths = m_paths.size();
    int processedPaths = 0;
    
    for (const QString& path : m_paths) {
        if (m_cancelled) {
            emit cancelled();
            return;
        }
        
        QList<SoftwareItem> pathItems = scanDirectory(path);
        items.append(pathItems);
        
        processedPaths++;
        int progressValue = (processedPaths * 100) / totalPaths;
        emit progress(progressValue);
    }
    
    emit finished(items);
}

void ScanWorker::cancel()
{
    m_cancelled = true;
}

QList<SoftwareItem> ScanWorker::scanDirectory(const QString& path)
{
    QList<SoftwareItem> items;
    
    QDir dir(path);
    if (!dir.exists()) {
        qCWarning(softwareManager) << "扫描路径不存在:" << path;
        return items;
    }
    
    // 获取目录中的所有文件
    QFileInfoList fileInfos = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    
    for (const QFileInfo& fileInfo : fileInfos) {
        if (m_cancelled) {
            return items;
        }
        
        QString filePath = fileInfo.absoluteFilePath();
        
        // 如果是目录，递归扫描
        if (fileInfo.isDir()) {
            QList<SoftwareItem> subItems = scanDirectory(filePath);
            items.append(subItems);
        }
        // 如果是有效的可执行文件或快捷方式
        else if (fileInfo.isFile()) {
            // 检查是否为有效的可执行文件路径
            // 这里简化处理，实际应该根据文件扩展名和系统类型判断
            if (filePath.endsWith(".exe", Qt::CaseInsensitive) || 
                filePath.endsWith(".lnk", Qt::CaseInsensitive) ||
                filePath.endsWith(".app", Qt::CaseInsensitive) ||
                (fileInfo.isExecutable() && !filePath.endsWith(".desktop"))) {
                
                SoftwareItem item(filePath);
                if (item.isValid()) {
                    items.append(item);
                }
            }
        }
    }
    
    return items;
}

SoftwareItem ScanWorker::parseShortcutFile(const QString& filePath)
{
    // 这里应该解析快捷方式文件(.lnk, .desktop等)获取目标路径
    // 简化处理，直接返回基于文件路径的软件项
    return SoftwareItem(filePath);
}

QIcon ScanWorker::extractIcon(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QFileIconProvider iconProvider;
    return iconProvider.icon(fileInfo);
}
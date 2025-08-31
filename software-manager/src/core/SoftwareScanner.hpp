#ifndef SOFTWARESCANNER_H
#define SOFTWARESCANNER_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QList>
#include <QFileInfo>

class SoftwareItem;
class ScanWorker;

class SoftwareScanner : public QObject {
    Q_OBJECT

public:
    explicit SoftwareScanner(QObject* parent = nullptr);
    ~SoftwareScanner();
    
    // 扫描控制
    void scanSystemSoftware();
    void cancelScan();
    bool isCurrentlyScanning() const;
    
    // 路径管理
    void addCustomPath(const QString& path);
    void removeCustomPath(const QString& path);
    QStringList getScanPaths() const;
    void setScanPaths(const QStringList& paths);
    
    // 手动添加
    SoftwareItem createSoftwareItem(const QString& filePath);
    
signals:
    void scanStarted();
    void scanProgress(int progress);
    void scanFinished(const QList<SoftwareItem>& items);
    void scanCancelled();
    void scanError(const QString& error);
    
private:
    QStringList m_scanPaths;
    bool m_isScanning;
    QThread* m_workerThread;
    ScanWorker* m_worker;
    
    // 私有方法
    void setupDefaultPaths();
    QStringList getDefaultScanPaths() const;
    bool isValidExecutablePath(const QString& path) const;
};

// 工作线程类
class ScanWorker : public QObject {
    Q_OBJECT

public:
    explicit ScanWorker(const QStringList& paths, QObject* parent = nullptr);
    
public slots:
    void process();
    void cancel();
    
signals:
    void progress(int progress);
    void finished(const QList<SoftwareItem>& items);
    void cancelled();
    void error(const QString& error);
    
private:
    QStringList m_paths;
    bool m_cancelled;
    
    QList<SoftwareItem> scanDirectory(const QString& path);
    SoftwareItem parseShortcutFile(const QString& filePath);
    QIcon extractIcon(const QString& filePath);
};

#endif // SOFTWARESCANNER_H
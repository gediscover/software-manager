#ifndef ICONEXTRACTOR_H
#define ICONEXTRACTOR_H

#include <QObject>
#include <QIcon>
#include <QCache>
#include <QThreadPool>

class IconLoadTask;

class IconExtractor : public QObject {
    Q_OBJECT

public:
    explicit IconExtractor(QObject* parent = nullptr);
    
    // 图标提取方法
    QIcon extractIcon(const QString& filePath);
    void extractIconAsync(const QString& filePath, QObject* receiver, const char* member);
    
    // 缓存管理
    void setCacheSize(int size);
    void clearCache();
    
private:
    QCache<QString, QIcon> m_iconCache;
    QThreadPool* m_threadPool;
    int m_maxCacheSize;
    
    QIcon loadIconFromFile(const QString& filePath);
};

#endif // ICONEXTRACTOR_H
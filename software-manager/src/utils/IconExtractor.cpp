#include "IconExtractor.hpp"
#include <QFileIconProvider>
#include <QFileInfo>
#include <QApplication>
#include <QStyle>
#include "Logging.hpp"

IconExtractor::IconExtractor(QObject* parent)
    : QObject(parent)
    , m_threadPool(QThreadPool::globalInstance())
    , m_maxCacheSize(100)
{
    m_iconCache.setMaxCost(m_maxCacheSize);
}

QIcon IconExtractor::extractIcon(const QString& filePath)
{
    // 检查缓存
    if (m_iconCache.contains(filePath)) {
        return *m_iconCache.object(filePath);
    }
    
    // 加载图标
    QIcon icon = loadIconFromFile(filePath);
    
    // 添加到缓存
    if (!icon.isNull()) {
        m_iconCache.insert(filePath, new QIcon(icon));
    }
    
    return icon;
}

void IconExtractor::extractIconAsync(const QString& filePath, QObject* receiver, const char* member)
{
    Q_UNUSED(filePath)
    Q_UNUSED(receiver)
    Q_UNUSED(member)
    
    // 异步图标加载实现
    // 需要创建IconLoadTask类来处理异步加载
}

void IconExtractor::setCacheSize(int size)
{
    m_maxCacheSize = size;
    m_iconCache.setMaxCost(size);
}

void IconExtractor::clearCache()
{
    m_iconCache.clear();
}

QIcon IconExtractor::loadIconFromFile(const QString& filePath)
{
    if (filePath.isEmpty() || !QFile::exists(filePath)) {
        return QIcon();
    }
    
    QFileInfo fileInfo(filePath);
    QFileIconProvider iconProvider;
    QIcon icon = iconProvider.icon(fileInfo);
    
    // 如果图标为空，使用默认图标
    if (icon.isNull()) {
        icon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    }
    
    return icon;
}
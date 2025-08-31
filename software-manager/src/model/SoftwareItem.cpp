#include "SoftwareItem.hpp"
#include <QFileInfo>
#include <QMimeDatabase>
#include <QMimeType>
#include <QDir>
#include <QStandardPaths>
#include <QApplication>
#include <QStyle>
#include <QPixmap>
#include <QFileIconProvider>
#include <QVariantMap>
#include "../utils/Logging.hpp"

SoftwareItem::SoftwareItem()
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_createdAt(QDateTime::currentDateTime())
    , m_updatedAt(QDateTime::currentDateTime())
{
}

SoftwareItem::SoftwareItem(const QString& filePath)
    : m_id(QUuid::createUuid().toString(QUuid::WithoutBraces))
    , m_filePath(filePath)
    , m_createdAt(QDateTime::currentDateTime())
    , m_updatedAt(QDateTime::currentDateTime())
{
    initializeFromFilePath(filePath);
}

SoftwareItem::SoftwareItem(const QString& id, const QString& name, const QString& filePath, 
                           const QString& category, const QString& description, 
                           const QString& version, const QDateTime& createdAt, 
                           const QDateTime& updatedAt)
    : m_id(id)
    , m_name(name)
    , m_filePath(filePath)
    , m_category(category)
    , m_description(description)
    , m_version(version)
    , m_createdAt(createdAt)
    , m_updatedAt(updatedAt)
{
    // 初始化图标
    if (!m_filePath.isEmpty() && QFile::exists(m_filePath)) {
        QFileInfo fileInfo(m_filePath);
        QFileIconProvider iconProvider;
        m_icon = iconProvider.icon(fileInfo);
        
        // 如果图标为空，使用默认图标
        if (m_icon.isNull()) {
            m_icon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
        }
    }
}

QString SoftwareItem::getId() const
{
    return m_id;
}

QString SoftwareItem::getName() const
{
    return m_name;
}

QString SoftwareItem::getFilePath() const
{
    return m_filePath;
}

QString SoftwareItem::getCategory() const
{
    return m_category;
}

QIcon SoftwareItem::getIcon() const
{
    return m_icon;
}

QString SoftwareItem::getDescription() const
{
    return m_description;
}

QString SoftwareItem::getVersion() const
{
    return m_version;
}

QDateTime SoftwareItem::getCreatedAt() const
{
    return m_createdAt;
}

QDateTime SoftwareItem::getUpdatedAt() const
{
    return m_updatedAt;
}

void SoftwareItem::setName(const QString& name)
{
    m_name = name;
    updateTimestamp();
}

void SoftwareItem::setCategory(const QString& category)
{
    m_category = category;
    updateTimestamp();
}

void SoftwareItem::setDescription(const QString& description)
{
    m_description = description;
    updateTimestamp();
}

void SoftwareItem::setVersion(const QString& version)
{
    m_version = version;
    updateTimestamp();
}

void SoftwareItem::setIcon(const QIcon& icon)
{
    m_icon = icon;
}

bool SoftwareItem::isValid() const
{
    return !m_id.isEmpty() && !m_name.isEmpty() && !m_filePath.isEmpty() && QFile::exists(m_filePath);
}

void SoftwareItem::updateTimestamp()
{
    m_updatedAt = QDateTime::currentDateTime();
}

void SoftwareItem::initializeFromFilePath(const QString& filePath)
{
    if (filePath.isEmpty() || !QFile::exists(filePath)) {
        qCWarning(softwareManager) << "文件路径无效:" << filePath;
        return;
    }
    
    QFileInfo fileInfo(filePath);
    
    // 提取名称
    m_name = extractNameFromPath(filePath);
    
    // 提取图标
    QFileIconProvider iconProvider;
    m_icon = iconProvider.icon(fileInfo);
    
    // 如果图标为空，使用默认图标
    if (m_icon.isNull()) {
        m_icon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    }
    
    qCInfo(softwareManager) << "成功初始化软件项:" << m_name << "路径:" << filePath;
}

QString SoftwareItem::extractNameFromPath(const QString& filePath) const
{
    QFileInfo fileInfo(filePath);
    QString baseName = fileInfo.baseName();
    
    // 如果是快捷方式文件，移除.lnk扩展名
    if (fileInfo.suffix().toLower() == "lnk") {
        return baseName;
    }
    
    // 如果是.desktop文件，移除.desktop扩展名
    if (fileInfo.suffix().toLower() == "desktop") {
        return baseName;
    }
    
    return fileInfo.fileName();
}

QVariantMap SoftwareItem::toVariantMap() const
{
    QVariantMap map;
    map["id"] = m_id;
    map["name"] = m_name;
    map["filePath"] = m_filePath;
    map["category"] = m_category;
    map["description"] = m_description;
    map["version"] = m_version;
    map["createdAt"] = m_createdAt;
    map["updatedAt"] = m_updatedAt;
    return map;
}

SoftwareItem SoftwareItem::fromVariantMap(const QVariantMap& map)
{
    SoftwareItem item(
        map["id"].toString(),
        map["name"].toString(),
        map["filePath"].toString(),
        map["category"].toString(),
        map["description"].toString(),
        map["version"].toString(),
        map["createdAt"].toDateTime(),
        map["updatedAt"].toDateTime()
    );
    return item;
}
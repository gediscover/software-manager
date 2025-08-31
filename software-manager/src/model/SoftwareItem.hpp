#ifndef SOFTWAREITEM_H
#define SOFTWAREITEM_H

#include <QString>
#include <QIcon>
#include <QDateTime>
#include <QFileInfo>
#include <QUuid>

class SoftwareItem {
public:
    // 构造函数
    SoftwareItem();
    explicit SoftwareItem(const QString& filePath);
    
    // 从数据库记录创建对象的构造函数
    SoftwareItem(const QString& id, const QString& name, const QString& filePath, 
                 const QString& category, const QString& description, 
                 const QString& version, const QDateTime& createdAt, 
                 const QDateTime& updatedAt);
    
    // Getter方法
    QString getId() const;
    QString getName() const;
    QString getFilePath() const;
    QString getCategory() const;
    QIcon getIcon() const;
    QString getDescription() const;
    QString getVersion() const;
    QDateTime getCreatedAt() const;
    QDateTime getUpdatedAt() const;
    
    // Setter方法
    void setName(const QString& name);
    void setCategory(const QString& category);
    void setDescription(const QString& description);
    void setVersion(const QString& version);
    void setIcon(const QIcon& icon);
    
    // 功能方法
    bool isValid() const;
    void updateTimestamp();
    
    // 数据库序列化方法
    QVariantMap toVariantMap() const;
    static SoftwareItem fromVariantMap(const QVariantMap& map);
    
private:
    QString m_id;
    QString m_name;
    QString m_filePath;
    QString m_category;
    QIcon m_icon;
    QString m_description;
    QString m_version;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    
    // 私有辅助方法
    void initializeFromFilePath(const QString& filePath);
    QString extractNameFromPath(const QString& filePath) const;
};

#endif // SOFTWAREITEM_H
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QList>
#include <QStringList>

class SoftwareItem;

class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject* parent = nullptr);
    ~DatabaseManager();
    
    // 数据库初始化
    bool initializeDatabase();
    bool isDatabaseValid() const;
    
    // 软件项管理
    bool addSoftwareItem(const SoftwareItem& item);
    bool updateSoftwareItem(const SoftwareItem& item);
    bool removeSoftwareItem(const QString& id);
    QList<SoftwareItem> getAllSoftwareItems();
    QList<SoftwareItem> getSoftwareItemsByCategory(const QString& category);
    SoftwareItem getSoftwareItemById(const QString& id);
    bool softwareItemExists(const QString& id);
    
    // 分类管理
    bool addCategory(const QString& name);
    bool removeCategory(const QString& name);
    QStringList getAllCategories();
    bool categoryExists(const QString& name);
    
    // 软件分类关联
    bool moveSoftwareToCategory(const QString& softwareId, const QString& categoryName);
    int getCategoryCount(const QString& category);
    
    // 批量操作
    bool batchInsertSoftwareItems(const QList<SoftwareItem>& items);
    bool batchUpdateSoftwareItems(const QList<SoftwareItem>& items);
    
    // 数据库维护
    bool backupDatabase(const QString& backupPath);
    bool restoreDatabase(const QString& backupPath);
    qint64 getDatabaseSize() const;
    
private:
    QString m_dbPath;
    QSqlDatabase m_database;
    
    // 私有方法
    bool createTables();
    bool executeQuery(const QString& sql);
    QString getDatabasePath() const;
    bool openDatabase();
    void closeDatabase();
    
    // 工具方法
    QString escapeString(const QString& str) const;
};

#endif // DATABASEMANAGER_H
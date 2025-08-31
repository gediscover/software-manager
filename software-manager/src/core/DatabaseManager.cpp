#include "DatabaseManager.hpp"
#include "../model/SoftwareItem.hpp"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QLoggingCategory>
#include <QFile>
#include <QSaveFile>
#include <QUuid>
#include "../utils/Logging.hpp"

DatabaseManager::DatabaseManager(QObject* parent)
    : QObject(parent)
{
    // 设置数据库路径
    m_dbPath = getDatabasePath();
    
    // 打开数据库
    openDatabase();
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

bool DatabaseManager::initializeDatabase()
{
    if (!openDatabase()) {
        qCWarning(softwareManager) << "无法打开数据库";
        return false;
    }
    
    if (!createTables()) {
        qCWarning(softwareManager) << "无法创建数据库表";
        return false;
    }
    
    qCInfo(softwareManager) << "数据库初始化成功";
    return true;
}

bool DatabaseManager::isDatabaseValid() const
{
    return m_database.isValid() && m_database.isOpen();
}

bool DatabaseManager::addSoftwareItem(const SoftwareItem& item)
{
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return false;
    }
    
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO software_items (id, name, file_path, category, description, version, created_at, updated_at) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    
    query.addBindValue(item.getId());
    query.addBindValue(item.getName());
    query.addBindValue(item.getFilePath());
    query.addBindValue(item.getCategory());
    query.addBindValue(item.getDescription());
    query.addBindValue(item.getVersion());
    query.addBindValue(item.getCreatedAt().toString(Qt::ISODate));
    query.addBindValue(item.getUpdatedAt().toString(Qt::ISODate));
    
    if (!query.exec()) {
        qCWarning(softwareManager) << "插入软件项失败:" << query.lastError().text();
        return false;
    }
    
    qCInfo(softwareManager) << "成功添加软件项:" << item.getName();
    return true;
}

bool DatabaseManager::updateSoftwareItem(const SoftwareItem& item)
{
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return false;
    }
    
    QSqlQuery query(m_database);
    query.prepare("UPDATE software_items SET name = ?, file_path = ?, category = ?, "
                  "description = ?, version = ?, updated_at = ? WHERE id = ?");
    
    query.addBindValue(item.getName());
    query.addBindValue(item.getFilePath());
    query.addBindValue(item.getCategory());
    query.addBindValue(item.getDescription());
    query.addBindValue(item.getVersion());
    query.addBindValue(item.getUpdatedAt().toString(Qt::ISODate));
    query.addBindValue(item.getId());
    
    if (!query.exec()) {
        qCWarning(softwareManager) << "更新软件项失败:" << query.lastError().text();
        return false;
    }
    
    qCInfo(softwareManager) << "成功更新软件项:" << item.getName();
    return true;
}

bool DatabaseManager::removeSoftwareItem(const QString& id)
{
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return false;
    }
    
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM software_items WHERE id = ?");
    query.addBindValue(id);
    
    if (!query.exec()) {
        qCWarning(softwareManager) << "删除软件项失败:" << query.lastError().text();
        return false;
    }
    
    // 同时删除分类关联
    QSqlQuery relationQuery(m_database);
    relationQuery.prepare("DELETE FROM software_category_relations WHERE software_id = ?");
    relationQuery.addBindValue(id);
    relationQuery.exec();
    
    qCInfo(softwareManager) << "成功删除软件项:" << id;
    return true;
}

QList<SoftwareItem> DatabaseManager::getAllSoftwareItems()
{
    QList<SoftwareItem> items;
    
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return items;
    }
    
    QSqlQuery query(m_database);
    query.prepare("SELECT id, name, file_path, category, description, version, created_at, updated_at "
                  "FROM software_items ORDER BY name");
    
    if (!query.exec()) {
        qCWarning(softwareManager) << "查询所有软件项失败:" << query.lastError().text();
        return items;
    }
    
    while (query.next()) {
        // 从查询结果创建软件项对象
        QString id = query.value(0).toString();
        QString name = query.value(1).toString();
        QString filePath = query.value(2).toString();
        QString category = query.value(3).toString();
        QString description = query.value(4).toString();
        QString version = query.value(5).toString();
        QDateTime createdAt = QDateTime::fromString(query.value(6).toString(), Qt::ISODate);
        QDateTime updatedAt = QDateTime::fromString(query.value(7).toString(), Qt::ISODate);
        
        SoftwareItem item(id, name, filePath, category, description, version, createdAt, updatedAt);
        items.append(item);
    }
    
    qCInfo(softwareManager) << "查询到" << items.size() << "个软件项";
    return items;
}

QList<SoftwareItem> DatabaseManager::getSoftwareItemsByCategory(const QString& category)
{
    QList<SoftwareItem> items;
    
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return items;
    }
    
    QSqlQuery query(m_database);
    query.prepare("SELECT id, name, file_path, category, description, version, created_at, updated_at "
                  "FROM software_items WHERE category = ? ORDER BY name");
    query.addBindValue(category);
    
    if (!query.exec()) {
        qCWarning(softwareManager) << "按分类查询软件项失败:" << query.lastError().text();
        return items;
    }
    
    while (query.next()) {
        // 从查询结果创建软件项对象
        QString id = query.value(0).toString();
        QString name = query.value(1).toString();
        QString filePath = query.value(2).toString();
        QString itemCategory = query.value(3).toString();
        QString description = query.value(4).toString();
        QString version = query.value(5).toString();
        QDateTime createdAt = QDateTime::fromString(query.value(6).toString(), Qt::ISODate);
        QDateTime updatedAt = QDateTime::fromString(query.value(7).toString(), Qt::ISODate);
        
        SoftwareItem item(id, name, filePath, itemCategory, description, version, createdAt, updatedAt);
        items.append(item);
    }
    
    qCInfo(softwareManager) << "分类" << category << "下查询到" << items.size() << "个软件项";
    return items;
}

SoftwareItem DatabaseManager::getSoftwareItemById(const QString& id)
{
    SoftwareItem item;
    
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return item;
    }
    
    QSqlQuery query(m_database);
    query.prepare("SELECT id, name, file_path, category, description, version, created_at, updated_at "
                  "FROM software_items WHERE id = ?");
    query.addBindValue(id);
    
    if (!query.exec()) {
        qCWarning(softwareManager) << "按ID查询软件项失败:" << query.lastError().text();
        return item;
    }
    
    if (query.next()) {
        // 从查询结果创建软件项对象
        QString itemId = query.value(0).toString();
        QString name = query.value(1).toString();
        QString filePath = query.value(2).toString();
        QString category = query.value(3).toString();
        QString description = query.value(4).toString();
        QString version = query.value(5).toString();
        QDateTime createdAt = QDateTime::fromString(query.value(6).toString(), Qt::ISODate);
        QDateTime updatedAt = QDateTime::fromString(query.value(7).toString(), Qt::ISODate);
        
        item = SoftwareItem(itemId, name, filePath, category, description, version, createdAt, updatedAt);
    }
    
    return item;
}

bool DatabaseManager::softwareItemExists(const QString& id)
{
    if (!isDatabaseValid()) {
        return false;
    }
    
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(*) FROM software_items WHERE id = ?");
    query.addBindValue(id);
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    return query.value(0).toInt() > 0;
}

bool DatabaseManager::addCategory(const QString& name)
{
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return false;
    }
    
    // 检查分类是否已存在
    if (categoryExists(name)) {
        return true;  // 分类已存在，视为成功
    }
    
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO categories (name, created_at, updated_at) VALUES (?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    
    if (!query.exec()) {
        qCWarning(softwareManager) << "添加分类失败:" << query.lastError().text();
        return false;
    }
    
    qCInfo(softwareManager) << "成功添加分类:" << name;
    return true;
}

bool DatabaseManager::removeCategory(const QString& name)
{
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return false;
    }
    
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM categories WHERE name = ?");
    query.addBindValue(name);
    
    if (!query.exec()) {
        qCWarning(softwareManager) << "删除分类失败:" << query.lastError().text();
        return false;
    }
    
    qCInfo(softwareManager) << "成功删除分类:" << name;
    return true;
}

QStringList DatabaseManager::getAllCategories()
{
    QStringList categories;
    
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return categories;
    }
    
    QSqlQuery query(m_database);
    query.prepare("SELECT name FROM categories ORDER BY name");
    
    if (!query.exec()) {
        qCWarning(softwareManager) << "查询所有分类失败:" << query.lastError().text();
        return categories;
    }
    
    while (query.next()) {
        categories.append(query.value(0).toString());
    }
    
    return categories;
}

bool DatabaseManager::categoryExists(const QString& name)
{
    if (!isDatabaseValid()) {
        return false;
    }
    
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(*) FROM categories WHERE name = ?");
    query.addBindValue(name);
    
    if (!query.exec() || !query.next()) {
        return false;
    }
    
    return query.value(0).toInt() > 0;
}

bool DatabaseManager::moveSoftwareToCategory(const QString& softwareId, const QString& categoryName)
{
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return false;
    }
    
    // 更新软件项的分类
    QSqlQuery query(m_database);
    query.prepare("UPDATE software_items SET category = ?, updated_at = ? WHERE id = ?");
    query.addBindValue(categoryName);
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(softwareId);
    
    if (!query.exec()) {
        qCWarning(softwareManager) << "移动软件到分类失败:" << query.lastError().text();
        return false;
    }
    
    qCInfo(softwareManager) << "成功移动软件" << softwareId << "到分类" << categoryName;
    return true;
}

int DatabaseManager::getCategoryCount(const QString& category)
{
    if (!isDatabaseValid()) {
        return 0;
    }
    
    QSqlQuery query(m_database);
    query.prepare("SELECT COUNT(*) FROM software_items WHERE category = ?");
    query.addBindValue(category);
    
    if (!query.exec() || !query.next()) {
        return 0;
    }
    
    return query.value(0).toInt();
}

bool DatabaseManager::batchInsertSoftwareItems(const QList<SoftwareItem>& items)
{
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return false;
    }
    
    // 开始事务
    if (!m_database.transaction()) {
        qCWarning(softwareManager) << "无法开始数据库事务";
        return false;
    }
    
    bool success = true;
    
    for (const SoftwareItem& item : items) {
        if (!addSoftwareItem(item)) {
            success = false;
            break;
        }
    }
    
    // 提交或回滚事务
    if (success) {
        if (!m_database.commit()) {
            qCWarning(softwareManager) << "无法提交数据库事务";
            success = false;
        }
    } else {
        m_database.rollback();
    }
    
    if (success) {
        qCInfo(softwareManager) << "批量插入" << items.size() << "个软件项成功";
    } else {
        qCWarning(softwareManager) << "批量插入软件项失败";
    }
    
    return success;
}

bool DatabaseManager::batchUpdateSoftwareItems(const QList<SoftwareItem>& items)
{
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return false;
    }
    
    // 开始事务
    if (!m_database.transaction()) {
        qCWarning(softwareManager) << "无法开始数据库事务";
        return false;
    }
    
    bool success = true;
    
    for (const SoftwareItem& item : items) {
        if (!updateSoftwareItem(item)) {
            success = false;
            break;
        }
    }
    
    // 提交或回滚事务
    if (success) {
        if (!m_database.commit()) {
            qCWarning(softwareManager) << "无法提交数据库事务";
            success = false;
        }
    } else {
        m_database.rollback();
    }
    
    if (success) {
        qCInfo(softwareManager) << "批量更新" << items.size() << "个软件项成功";
    } else {
        qCWarning(softwareManager) << "批量更新软件项失败";
    }
    
    return success;
}

bool DatabaseManager::backupDatabase(const QString& backupPath)
{
    if (!isDatabaseValid()) {
        qCWarning(softwareManager) << "数据库未初始化";
        return false;
    }
    
    // 关闭数据库
    closeDatabase();
    
    // 复制数据库文件
    QFile sourceFile(m_dbPath);
    bool success = sourceFile.copy(backupPath);
    
    // 重新打开数据库
    openDatabase();
    
    if (success) {
        qCInfo(softwareManager) << "数据库备份成功:" << backupPath;
    } else {
        qCWarning(softwareManager) << "数据库备份失败:" << backupPath;
    }
    
    return success;
}

bool DatabaseManager::restoreDatabase(const QString& backupPath)
{
    if (!QFile::exists(backupPath)) {
        qCWarning(softwareManager) << "备份文件不存在:" << backupPath;
        return false;
    }
    
    // 关闭数据库
    closeDatabase();
    
    // 复制备份文件到数据库位置
    QFile::remove(m_dbPath);
    bool success = QFile::copy(backupPath, m_dbPath);
    
    // 重新打开数据库
    openDatabase();
    
    if (success) {
        qCInfo(softwareManager) << "数据库恢复成功:" << backupPath;
    } else {
        qCWarning(softwareManager) << "数据库恢复失败:" << backupPath;
    }
    
    return success;
}

qint64 DatabaseManager::getDatabaseSize() const
{
    QFile file(m_dbPath);
    if (file.exists()) {
        return file.size();
    }
    return 0;
}

bool DatabaseManager::createTables()
{
    if (!isDatabaseValid()) {
        return false;
    }
    
    // 创建software_items表
    QString createSoftwareItemsTable = 
        "CREATE TABLE IF NOT EXISTS software_items ("
        "id TEXT PRIMARY KEY, "
        "name TEXT NOT NULL, "
        "file_path TEXT NOT NULL, "
        "category TEXT, "
        "description TEXT, "
        "version TEXT, "
        "created_at DATETIME NOT NULL, "
        "updated_at DATETIME NOT NULL"
        ")";
    
    if (!executeQuery(createSoftwareItemsTable)) {
        return false;
    }
    
    // 创建categories表
    QString createCategoriesTable = 
        "CREATE TABLE IF NOT EXISTS categories ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL UNIQUE, "
        "created_at DATETIME NOT NULL, "
        "updated_at DATETIME NOT NULL"
        ")";
    
    if (!executeQuery(createCategoriesTable)) {
        return false;
    }
    
    // 创建software_category_relations表
    QString createRelationsTable = 
        "CREATE TABLE IF NOT EXISTS software_category_relations ("
        "software_id TEXT, "
        "category_id INTEGER, "
        "created_at DATETIME NOT NULL, "
        "FOREIGN KEY(software_id) REFERENCES software_items(id), "
        "FOREIGN KEY(category_id) REFERENCES categories(id)"
        ")";
    
    if (!executeQuery(createRelationsTable)) {
        return false;
    }
    
    return true;
}

bool DatabaseManager::executeQuery(const QString& sql)
{
    if (!isDatabaseValid()) {
        return false;
    }
    
    QSqlQuery query(m_database);
    if (!query.exec(sql)) {
        qCWarning(softwareManager) << "执行SQL失败:" << sql << ", 错误:" << query.lastError().text();
        return false;
    }
    
    return true;
}

QString DatabaseManager::getDatabasePath() const
{
    // 获取应用程序数据目录
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    
    // 创建目录（如果不存在）
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // 返回数据库文件路径
    return dataPath + "/software.db";
}

bool DatabaseManager::openDatabase()
{
    // 关闭现有连接
    closeDatabase();
    
    // 创建数据库连接
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_dbPath);
    
    if (!m_database.open()) {
        qCWarning(softwareManager) << "无法打开数据库:" << m_database.lastError().text();
        return false;
    }
    
    qCInfo(softwareManager) << "成功打开数据库:" << m_dbPath;
    return true;
}

void DatabaseManager::closeDatabase()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    
    // 移除数据库连接
    QSqlDatabase::removeDatabase(m_database.connectionName());
}

QString DatabaseManager::escapeString(const QString& str) const
{
    // 简单的字符串转义（实际应用中应该使用参数化查询）
    QString escaped = str;
    escaped.replace("'", "''");
    return escaped;
}
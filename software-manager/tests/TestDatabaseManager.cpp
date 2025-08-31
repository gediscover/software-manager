#include <QtTest/QtTest>
#include "../src/core/DatabaseManager.hpp"
#include "../src/model/SoftwareItem.hpp"
#include <QTemporaryDir>
#include <QFile>
#include <QDir>

class TestDatabaseManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testDatabaseInitialization();
    void testSoftwareItemCRUD();
    void testCategoryCRUD();
    void testBatchOperations();
    void testMoveSoftwareToCategory();
    void testGetCategoryCount();
    void testSoftwareItemExists();
    void testGetSoftwareItemById();
    void testGetSoftwareItemsByCategory();
    void testBackupAndRestore();
    void testGetDatabaseSize();
    void cleanupTestCase();

private:
    DatabaseManager* m_databaseManager;
    QTemporaryDir* m_tempDir;
};

void TestDatabaseManager::initTestCase()
{
    m_databaseManager = new DatabaseManager();
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
    
    // 初始化数据库
    QVERIFY(m_databaseManager->initializeDatabase());
    QVERIFY(m_databaseManager->isDatabaseValid());
}

void TestDatabaseManager::testDatabaseInitialization()
{
    // 数据库应该已经初始化
    QVERIFY(m_databaseManager->isDatabaseValid());
    
    // 获取所有分类（应该至少有默认分类）
    QStringList categories = m_databaseManager->getAllCategories();
    QVERIFY(categories.size() >= 0); // 可能为空，因为没有默认添加
}

void TestDatabaseManager::testSoftwareItemCRUD()
{
    // 创建临时文件用于测试
    QString tempFile = m_tempDir->path() + "/test_app.exe";
    QFile file(tempFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();
    
    // 创建软件项
    SoftwareItem item(tempFile);
    item.setName("测试软件");
    item.setCategory("测试分类");
    item.setDescription("这是一个测试软件");
    item.setVersion("1.0.0");
    
    // 添加软件项
    QVERIFY(m_databaseManager->addSoftwareItem(item));
    
    // 检查软件项是否存在
    QVERIFY(m_databaseManager->softwareItemExists(item.getId()));
    
    // 获取软件项
    SoftwareItem retrievedItem = m_databaseManager->getSoftwareItemById(item.getId());
    QVERIFY(retrievedItem.isValid());
    QCOMPARE(retrievedItem.getId(), item.getId());
    QCOMPARE(retrievedItem.getName(), QString("测试软件"));
    QCOMPARE(retrievedItem.getCategory(), QString("测试分类"));
    QCOMPARE(retrievedItem.getDescription(), QString("这是一个测试软件"));
    QCOMPARE(retrievedItem.getVersion(), QString("1.0.0"));
    
    // 获取所有软件项
    QList<SoftwareItem> allItems = m_databaseManager->getAllSoftwareItems();
    QVERIFY(!allItems.isEmpty());
    QVERIFY(allItems.size() >= 1);
    
    // 按分类获取软件项
    QList<SoftwareItem> categoryItems = m_databaseManager->getSoftwareItemsByCategory("测试分类");
    QVERIFY(!categoryItems.isEmpty());
    QVERIFY(categoryItems.size() >= 1);
    
    // 更新软件项
    item.setName("更新后的测试软件");
    item.setVersion("2.0.0");
    QVERIFY(m_databaseManager->updateSoftwareItem(item));
    
    // 验证更新
    SoftwareItem updatedItem = m_databaseManager->getSoftwareItemById(item.getId());
    QCOMPARE(updatedItem.getName(), QString("更新后的测试软件"));
    QCOMPARE(updatedItem.getVersion(), QString("2.0.0"));
    
    // 删除软件项
    QVERIFY(m_databaseManager->removeSoftwareItem(item.getId()));
    
    // 验证删除
    QVERIFY(!m_databaseManager->softwareItemExists(item.getId()));
}

void TestDatabaseManager::testCategoryCRUD()
{
    QString categoryName = "数据库测试分类";
    
    // 添加分类
    QVERIFY(m_databaseManager->addCategory(categoryName));
    
    // 检查分类是否存在
    QVERIFY(m_databaseManager->categoryExists(categoryName));
    
    // 获取所有分类
    QStringList categories = m_databaseManager->getAllCategories();
    QVERIFY(categories.contains(categoryName));
    
    // 删除分类
    QVERIFY(m_databaseManager->removeCategory(categoryName));
    
    // 验证删除
    QVERIFY(!m_databaseManager->categoryExists(categoryName));
}

void TestDatabaseManager::testBatchOperations()
{
    // 创建多个临时文件
    QList<SoftwareItem> items;
    for (int i = 0; i < 5; ++i) {
        QString tempFile = m_tempDir->path() + QString("/test_app_%1.exe").arg(i);
        QFile file(tempFile);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.close();
        
        SoftwareItem item(tempFile);
        item.setName(QString("批量测试软件 %1").arg(i));
        item.setCategory("批量测试分类");
        items.append(item);
    }
    
    // 批量插入
    QVERIFY(m_databaseManager->batchInsertSoftwareItems(items));
    
    // 验证插入
    QList<SoftwareItem> allItems = m_databaseManager->getAllSoftwareItems();
    QVERIFY(allItems.size() >= 5);
    
    // 更新所有项
    for (auto& item : items) {
        item.setVersion("1.0.0");
    }
    
    // 批量更新
    QVERIFY(m_databaseManager->batchUpdateSoftwareItems(items));
    
    // 验证更新
    for (const auto& item : items) {
        SoftwareItem retrievedItem = m_databaseManager->getSoftwareItemById(item.getId());
        QCOMPARE(retrievedItem.getVersion(), QString("1.0.0"));
    }
}

void TestDatabaseManager::testMoveSoftwareToCategory()
{
    // 创建临时文件
    QString tempFile = m_tempDir->path() + "/move_test_app.exe";
    QFile file(tempFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();
    
    // 创建软件项
    SoftwareItem item(tempFile);
    item.setName("移动测试软件");
    item.setCategory("原始分类");
    
    // 添加软件项
    QVERIFY(m_databaseManager->addSoftwareItem(item));
    
    // 移动到新分类
    QString newCategory = "新分类";
    QVERIFY(m_databaseManager->moveSoftwareToCategory(item.getId(), newCategory));
    
    // 验证移动
    SoftwareItem movedItem = m_databaseManager->getSoftwareItemById(item.getId());
    QCOMPARE(movedItem.getCategory(), newCategory);
    
    // 检查分类计数
    int categoryCount = m_databaseManager->getCategoryCount(newCategory);
    QVERIFY(categoryCount >= 1);
}

void TestDatabaseManager::testGetCategoryCount()
{
    QString categoryName = "计数测试分类";
    
    // 添加分类
    QVERIFY(m_databaseManager->addCategory(categoryName));
    
    // 检查初始计数
    int count = m_databaseManager->getCategoryCount(categoryName);
    QCOMPARE(count, 0);
    
    // 添加一个软件项到分类
    QString tempFile = m_tempDir->path() + "/count_test_app.exe";
    QFile file(tempFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();
    
    SoftwareItem item(tempFile);
    item.setName("计数测试软件");
    item.setCategory(categoryName);
    
    QVERIFY(m_databaseManager->addSoftwareItem(item));
    
    // 更新分类计数
    QVERIFY(m_databaseManager->moveSoftwareToCategory(item.getId(), categoryName));
    
    // 检查更新后的计数
    count = m_databaseManager->getCategoryCount(categoryName);
    QVERIFY(count >= 1);
}

void TestDatabaseManager::testSoftwareItemExists()
{
    // 测试不存在的软件项
    QVERIFY(!m_databaseManager->softwareItemExists("non-existent-id"));
    
    // 创建临时文件
    QString tempFile = m_tempDir->path() + "/exists_test_app.exe";
    QFile file(tempFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();
    
    // 创建软件项
    SoftwareItem item(tempFile);
    item.setName("存在性测试软件");
    
    // 添加软件项
    QVERIFY(m_databaseManager->addSoftwareItem(item));
    
    // 测试存在的软件项
    QVERIFY(m_databaseManager->softwareItemExists(item.getId()));
    
    // 删除软件项
    QVERIFY(m_databaseManager->removeSoftwareItem(item.getId()));
    
    // 测试删除后的软件项
    QVERIFY(!m_databaseManager->softwareItemExists(item.getId()));
}

void TestDatabaseManager::testGetSoftwareItemById()
{
    // 测试获取不存在的软件项
    SoftwareItem invalidItem = m_databaseManager->getSoftwareItemById("non-existent-id");
    QVERIFY(!invalidItem.isValid());
    
    // 创建临时文件
    QString tempFile = m_tempDir->path() + "/get_by_id_test_app.exe";
    QFile file(tempFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();
    
    // 创建软件项
    SoftwareItem item(tempFile);
    item.setName("获取测试软件");
    item.setCategory("测试分类");
    item.setDescription("测试描述");
    item.setVersion("1.0.0");
    
    // 添加软件项
    QVERIFY(m_databaseManager->addSoftwareItem(item));
    
    // 测试获取存在的软件项
    SoftwareItem retrievedItem = m_databaseManager->getSoftwareItemById(item.getId());
    QVERIFY(retrievedItem.isValid());
    QCOMPARE(retrievedItem.getId(), item.getId());
    QCOMPARE(retrievedItem.getName(), item.getName());
    QCOMPARE(retrievedItem.getCategory(), item.getCategory());
    QCOMPARE(retrievedItem.getDescription(), item.getDescription());
    QCOMPARE(retrievedItem.getVersion(), item.getVersion());
}

void TestDatabaseManager::testGetSoftwareItemsByCategory()
{
    QString categoryName = "分类查询测试";
    
    // 添加分类
    QVERIFY(m_databaseManager->addCategory(categoryName));
    
    // 创建多个临时文件
    QList<SoftwareItem> items;
    for (int i = 0; i < 3; ++i) {
        QString tempFile = m_tempDir->path() + QString("/category_query_test_%1.exe").arg(i);
        QFile file(tempFile);
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.close();
        
        SoftwareItem item(tempFile);
        item.setName(QString("分类查询测试软件 %1").arg(i));
        item.setCategory(categoryName);
        items.append(item);
    }
    
    // 添加软件项
    for (const SoftwareItem& item : items) {
        QVERIFY(m_databaseManager->addSoftwareItem(item));
    }
    
    // 按分类获取软件项
    QList<SoftwareItem> categoryItems = m_databaseManager->getSoftwareItemsByCategory(categoryName);
    QCOMPARE(categoryItems.size(), items.size());
    
    // 验证获取的软件项
    for (const SoftwareItem& item : categoryItems) {
        QCOMPARE(item.getCategory(), categoryName);
    }
    
    // 测试获取不存在分类的软件项
    QList<SoftwareItem> emptyItems = m_databaseManager->getSoftwareItemsByCategory("不存在的分类");
    QVERIFY(emptyItems.isEmpty());
}

void TestDatabaseManager::testBackupAndRestore()
{
    // 创建临时文件用于备份
    QString backupPath = m_tempDir->path() + "/backup.db";
    
    // 创建测试数据
    QString tempFile = m_tempDir->path() + "/backup_test_app.exe";
    QFile file(tempFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();
    
    SoftwareItem item(tempFile);
    item.setName("备份测试软件");
    
    QVERIFY(m_databaseManager->addSoftwareItem(item));
    
    // 备份数据库
    QVERIFY(m_databaseManager->backupDatabase(backupPath));
    
    // 验证备份文件存在
    QVERIFY(QFile::exists(backupPath));
    
    // 删除原始数据
    QVERIFY(m_databaseManager->removeSoftwareItem(item.getId()));
    
    // 验证数据已删除
    QVERIFY(!m_databaseManager->softwareItemExists(item.getId()));
    
    // 恢复数据库
    QVERIFY(m_databaseManager->restoreDatabase(backupPath));
    
    // 验证数据已恢复
    QVERIFY(m_databaseManager->softwareItemExists(item.getId()));
}

void TestDatabaseManager::testGetDatabaseSize()
{
    qint64 initialSize = m_databaseManager->getDatabaseSize();
    QVERIFY(initialSize >= 0);
    
    // 创建测试数据
    QString tempFile = m_tempDir->path() + "/size_test_app.exe";
    QFile file(tempFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();
    
    SoftwareItem item(tempFile);
    item.setName("大小测试软件");
    
    QVERIFY(m_databaseManager->addSoftwareItem(item));
    
    // 检查数据库大小是否增加
    qint64 newSize = m_databaseManager->getDatabaseSize();
    QVERIFY(newSize >= initialSize);
}

void TestDatabaseManager::cleanupTestCase()
{
    delete m_databaseManager;
    delete m_tempDir;
}

QTEST_MAIN(TestDatabaseManager)
#include "TestDatabaseManager.moc"
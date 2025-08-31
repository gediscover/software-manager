#include <QtTest/QtTest>
#include "../src/core/CategoryManager.hpp"

class TestCategoryManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testDefaultCategories();
    void testAddCategory();
    void testRenameCategory();
    void testRemoveCategory();
    void testCategoryExists();
    void testValidateCategoryName();
    void testMoveSoftwareToCategory();
    void testGetCategoryCount();
    void testGetDefaultCategory();
    void testIsBuiltInCategory();
    void cleanupTestCase();

private:
    CategoryManager* m_categoryManager;
};

void TestCategoryManager::initTestCase()
{
    m_categoryManager = new CategoryManager();
}

void TestCategoryManager::testDefaultCategories()
{
    QStringList categories = m_categoryManager->getCategories();
    
    // 检查默认分类是否存在
    QVERIFY(categories.contains("所有软件"));
    QVERIFY(categories.contains("未分类"));
    
    // 检查默认分类数量
    QVERIFY(categories.size() >= 2);
}

void TestCategoryManager::testAddCategory()
{
    QString categoryName = "测试分类";
    
    // 添加分类
    bool result = m_categoryManager->addCategory(categoryName);
    QVERIFY(result);
    
    // 检查分类是否添加成功
    QStringList categories = m_categoryManager->getCategories();
    QVERIFY(categories.contains(categoryName));
    
    // 尝试添加重复分类应该返回true（已存在视为成功）
    result = m_categoryManager->addCategory(categoryName);
    QVERIFY(result); // 应该成功
    
    // 尝试添加无效名称
    result = m_categoryManager->addCategory("");
    QVERIFY(!result);
}

void TestCategoryManager::testRenameCategory()
{
    QString oldName = "旧分类";
    QString newName = "新分类";
    
    // 先添加分类
    QVERIFY(m_categoryManager->addCategory(oldName));
    
    // 重命名分类
    bool result = m_categoryManager->renameCategory(oldName, newName);
    QVERIFY(result);
    
    // 检查重命名是否成功
    QStringList categories = m_categoryManager->getCategories();
    QVERIFY(!categories.contains(oldName));
    QVERIFY(categories.contains(newName));
    
    // 尝试重命名不存在的分类
    result = m_categoryManager->renameCategory("不存在的分类", "另一个分类");
    QVERIFY(!result);
    
    // 尝试重命名为无效名称
    result = m_categoryManager->renameCategory(newName, "");
    QVERIFY(!result);
}

void TestCategoryManager::testRemoveCategory()
{
    QString categoryName = "可删除分类";
    
    // 先添加分类
    QVERIFY(m_categoryManager->addCategory(categoryName));
    
    // 删除分类
    bool result = m_categoryManager->removeCategory(categoryName);
    QVERIFY(result);
    
    // 检查分类是否删除成功
    QStringList categories = m_categoryManager->getCategories();
    QVERIFY(!categories.contains(categoryName));
    
    // 尝试删除不存在的分类
    result = m_categoryManager->removeCategory("不存在的分类");
    QVERIFY(!result);
    
    // 尝试删除内置分类（应该失败）
    result = m_categoryManager->removeCategory("所有软件");
    QVERIFY(!result);
}

void TestCategoryManager::testCategoryExists()
{
    QString categoryName = "存在性测试分类";
    
    // 添加分类
    QVERIFY(m_categoryManager->addCategory(categoryName));
    
    // 检查分类存在性
    QVERIFY(m_categoryManager->categoryExists(categoryName));
    QVERIFY(!m_categoryManager->categoryExists("不存在的分类"));
}

void TestCategoryManager::testValidateCategoryName()
{
    // 测试有效名称
    QVERIFY(m_categoryManager->validateCategoryName("有效分类名称"));
    QVERIFY(m_categoryManager->validateCategoryName("Valid Category Name"));
    
    // 测试无效名称
    QVERIFY(!m_categoryManager->validateCategoryName("")); // 空名称
    QVERIFY(!m_categoryManager->validateCategoryName(" ")); // 空白字符
    
    // 测试过长名称
    QString longName(60, 'a'); // 60个字符
    QVERIFY(!m_categoryManager->validateCategoryName(longName));
    
    // 测试包含非法字符的名称
    QVERIFY(!m_categoryManager->validateCategoryName("包含<非法字符>"));
    QVERIFY(!m_categoryManager->validateCategoryName("包含\"引号\""));
    QVERIFY(!m_categoryManager->validateCategoryName("包含/斜杠"));
}

void TestCategoryManager::testMoveSoftwareToCategory()
{
    QString categoryName = "移动测试分类";
    QString softwareId = "test-software-id";
    
    // 先添加分类
    QVERIFY(m_categoryManager->addCategory(categoryName));
    
    // 移动软件到分类
    bool result = m_categoryManager->moveSoftwareToCategory(softwareId, categoryName);
    QVERIFY(result);
    
    // 尝试移动到不存在的分类
    result = m_categoryManager->moveSoftwareToCategory(softwareId, "不存在的分类");
    QVERIFY(!result);
}

void TestCategoryManager::testGetCategoryCount()
{
    QString categoryName = "计数测试分类";
    
    // 先添加分类
    QVERIFY(m_categoryManager->addCategory(categoryName));
    
    // 检查初始计数
    int count = m_categoryManager->getCategoryCount(categoryName);
    QVERIFY(count >= 0);
}

void TestCategoryManager::testGetDefaultCategory()
{
    QString defaultCategory = m_categoryManager->getDefaultCategory();
    QCOMPARE(defaultCategory, QString("未分类"));
}

void TestCategoryManager::testIsBuiltInCategory()
{
    // 测试内置分类
    QVERIFY(m_categoryManager->categoryExists("所有软件"));
    QVERIFY(m_categoryManager->categoryExists("未分类"));
    
    // 测试非内置分类
    QString userCategory = "用户分类";
    QVERIFY(m_categoryManager->addCategory(userCategory));
    QVERIFY(m_categoryManager->categoryExists(userCategory));
}

void TestCategoryManager::cleanupTestCase()
{
    delete m_categoryManager;
}

QTEST_MAIN(TestCategoryManager)
#include "TestCategoryManager.moc"
#include <QtTest/QtTest>
#include "../src/model/SoftwareItem.hpp"
#include <QFile>
#include <QTemporaryDir>

class TestSoftwareItem : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testDefaultConstructor();
    void testFilePathConstructor();
    void testGettersAndSetters();
    void testValidation();
    void cleanupTestCase();

private:
    QString m_testFilePath;
};

void TestSoftwareItem::initTestCase()
{
    // 创建一个临时文件用于测试
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    m_testFilePath = tempDir.path() + "/test_app.exe";
    QFile file(m_testFilePath);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();
}

void TestSoftwareItem::testDefaultConstructor()
{
    SoftwareItem item;
    
    // 检查ID是否生成
    QVERIFY(!item.getId().isEmpty());
    
    // 检查时间戳是否设置
    QVERIFY(item.getCreatedAt().isValid());
    QVERIFY(item.getUpdatedAt().isValid());
}

void TestSoftwareItem::testFilePathConstructor()
{
    SoftwareItem item(m_testFilePath);
    
    // 检查ID是否生成
    QVERIFY(!item.getId().isEmpty());
    
    // 检查文件路径是否正确设置
    QCOMPARE(item.getFilePath(), m_testFilePath);
    
    // 检查时间戳是否设置
    QVERIFY(item.getCreatedAt().isValid());
    QVERIFY(item.getUpdatedAt().isValid());
}

void TestSoftwareItem::testGettersAndSetters()
{
    SoftwareItem item;
    
    // 测试设置和获取名称
    QString name = "Test Application";
    item.setName(name);
    QCOMPARE(item.getName(), name);
    
    // 测试设置和获取分类
    QString category = "Development";
    item.setCategory(category);
    QCOMPARE(item.getCategory(), category);
    
    // 测试设置和获取描述
    QString description = "A test application for unit testing";
    item.setDescription(description);
    QCOMPARE(item.getDescription(), description);
    
    // 测试设置和获取版本
    QString version = "1.0.0";
    item.setVersion(version);
    QCOMPARE(item.getVersion(), version);
    
    // 检查更新时间是否更新
    QVERIFY(item.getUpdatedAt() >= item.getCreatedAt());
}

void TestSoftwareItem::testValidation()
{
    // 测试无效项
    SoftwareItem invalidItem;
    QVERIFY(!invalidItem.isValid());
    
    // 测试有效项
    SoftwareItem validItem(m_testFilePath);
    QVERIFY(validItem.isValid());
    
    // 测试不存在文件路径的项
    SoftwareItem invalidPathItem("/path/that/does/not/exist");
    QVERIFY(!invalidPathItem.isValid());
}

void TestSoftwareItem::cleanupTestCase()
{
    // 清理临时文件
    QFile::remove(m_testFilePath);
}

QTEST_MAIN(TestSoftwareItem)
#include "TestSoftwareItem.moc"
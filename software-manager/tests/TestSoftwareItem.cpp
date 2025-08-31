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
    void testSerialization();
    void testDeserialization();
    void testToVariantMap();
    void testFromVariantMap();
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

void TestSoftwareItem::testSerialization()
{
    // 创建软件项
    SoftwareItem item(m_testFilePath);
    item.setName("序列化测试软件");
    item.setCategory("测试分类");
    item.setDescription("用于测试序列化的软件");
    item.setVersion("1.0.0");
    
    // 序列化为VariantMap
    QVariantMap map = item.toVariantMap();
    
    // 检查必要的字段是否存在
    QVERIFY(map.contains("id"));
    QVERIFY(map.contains("name"));
    QVERIFY(map.contains("filePath"));
    QVERIFY(map.contains("category"));
    QVERIFY(map.contains("description"));
    QVERIFY(map.contains("version"));
    QVERIFY(map.contains("createdAt"));
    QVERIFY(map.contains("updatedAt"));
    
    // 检查字段值是否正确
    QCOMPARE(map["id"].toString(), item.getId());
    QCOMPARE(map["name"].toString(), QString("序列化测试软件"));
    QCOMPARE(map["filePath"].toString(), m_testFilePath);
    QCOMPARE(map["category"].toString(), QString("测试分类"));
    QCOMPARE(map["description"].toString(), QString("用于测试序列化的软件"));
    QCOMPARE(map["version"].toString(), QString("1.0.0"));
}

void TestSoftwareItem::testDeserialization()
{
    // 创建VariantMap
    QVariantMap map;
    map["id"] = "test-id-123";
    map["name"] = "反序列化测试软件";
    map["filePath"] = m_testFilePath;
    map["category"] = "测试分类";
    map["description"] = "用于测试反序列化的软件";
    map["version"] = "1.0.0";
    map["createdAt"] = QDateTime::currentDateTime();
    map["updatedAt"] = QDateTime::currentDateTime();
    
    // 从VariantMap创建SoftwareItem
    SoftwareItem item = SoftwareItem::fromVariantMap(map);
    
    // 检查字段值是否正确
    QCOMPARE(item.getId(), QString("test-id-123"));
    QCOMPARE(item.getName(), QString("反序列化测试软件"));
    QCOMPARE(item.getFilePath(), m_testFilePath);
    QCOMPARE(item.getCategory(), QString("测试分类"));
    QCOMPARE(item.getDescription(), QString("用于测试反序列化的软件"));
    QCOMPARE(item.getVersion(), QString("1.0.0"));
    QCOMPARE(item.getCreatedAt().toString(), map["createdAt"].toDateTime().toString());
    QCOMPARE(item.getUpdatedAt().toString(), map["updatedAt"].toDateTime().toString());
}

void TestSoftwareItem::testToVariantMap()
{
    // 创建软件项
    SoftwareItem item(m_testFilePath);
    item.setName("VariantMap测试软件");
    item.setCategory("测试分类");
    item.setDescription("用于测试toVariantMap方法的软件");
    item.setVersion("1.0.0");
    
    // 转换为VariantMap
    QVariantMap map = item.toVariantMap();
    
    // 验证所有字段
    QVERIFY(!map["id"].toString().isEmpty());
    QCOMPARE(map["name"].toString(), QString("VariantMap测试软件"));
    QCOMPARE(map["filePath"].toString(), m_testFilePath);
    QCOMPARE(map["category"].toString(), QString("测试分类"));
    QCOMPARE(map["description"].toString(), QString("用于测试toVariantMap方法的软件"));
    QCOMPARE(map["version"].toString(), QString("1.0.0"));
    QVERIFY(map["createdAt"].toDateTime().isValid());
    QVERIFY(map["updatedAt"].toDateTime().isValid());
}

void TestSoftwareItem::testFromVariantMap()
{
    // 创建VariantMap
    QVariantMap map;
    map["id"] = "variant-map-test-id";
    map["name"] = "FromVariantMap测试软件";
    map["filePath"] = m_testFilePath;
    map["category"] = "测试分类";
    map["description"] = "用于测试fromVariantMap方法的软件";
    map["version"] = "2.0.0";
    map["createdAt"] = QDateTime(QDate(2023, 1, 1), QTime(12, 0, 0));
    map["updatedAt"] = QDateTime(QDate(2023, 1, 2), QTime(14, 30, 0));
    
    // 从VariantMap创建SoftwareItem
    SoftwareItem item = SoftwareItem::fromVariantMap(map);
    
    // 验证所有字段
    QCOMPARE(item.getId(), QString("variant-map-test-id"));
    QCOMPARE(item.getName(), QString("FromVariantMap测试软件"));
    QCOMPARE(item.getFilePath(), m_testFilePath);
    QCOMPARE(item.getCategory(), QString("测试分类"));
    QCOMPARE(item.getDescription(), QString("用于测试fromVariantMap方法的软件"));
    QCOMPARE(item.getVersion(), QString("2.0.0"));
    QCOMPARE(item.getCreatedAt(), QDateTime(QDate(2023, 1, 1), QTime(12, 0, 0)));
    QCOMPARE(item.getUpdatedAt(), QDateTime(QDate(2023, 1, 2), QTime(14, 30, 0)));
}

void TestSoftwareItem::cleanupTestCase()
{
    // 清理临时文件
    QFile::remove(m_testFilePath);
}

QTEST_MAIN(TestSoftwareItem)
#include "TestSoftwareItem.moc"
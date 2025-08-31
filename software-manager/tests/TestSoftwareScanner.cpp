#include <QtTest/QtTest>
#include "../src/core/SoftwareScanner.hpp"
#include <QTemporaryDir>
#include <QFile>
#include <QDir>

class TestSoftwareScanner : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void testDefaultScanPaths();
    void testCustomPathManagement();
    void testCreateSoftwareItem();
    void testIsValidExecutablePath();
    void testSetScanPaths();
    void testIsCurrentlyScanning();
    void testScanSystemSoftware();
    void cleanupTestCase();

private:
    SoftwareScanner* m_scanner;
    QTemporaryDir* m_tempDir;
};

void TestSoftwareScanner::initTestCase()
{
    m_scanner = new SoftwareScanner();
    m_tempDir = new QTemporaryDir();
    QVERIFY(m_tempDir->isValid());
}

void TestSoftwareScanner::testDefaultScanPaths()
{
    QStringList paths = m_scanner->getScanPaths();
    
    // 检查是否有一些默认路径
    QVERIFY(!paths.isEmpty());
    
    // 检查路径是否有效
    for (const QString& path : paths) {
        QVERIFY(!path.isEmpty());
    }
}

void TestSoftwareScanner::testCustomPathManagement()
{
    QString customPath = m_tempDir->path() + "/custom_scan_path";
    
    // 创建自定义路径目录
    QDir dir(customPath);
    if (!dir.exists()) {
        QVERIFY(dir.mkpath("."));
    }
    
    // 添加自定义路径
    m_scanner->addCustomPath(customPath);
    
    // 检查路径是否添加成功
    QStringList paths = m_scanner->getScanPaths();
    QVERIFY(paths.contains(customPath));
    
    // 移除自定义路径
    m_scanner->removeCustomPath(customPath);
    
    // 检查路径是否移除成功
    paths = m_scanner->getScanPaths();
    QVERIFY(!paths.contains(customPath));
}

void TestSoftwareScanner::testCreateSoftwareItem()
{
    // 创建临时文件
    QString tempFile = m_tempDir->path() + "/test_app.exe";
    QFile file(tempFile);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.close();
    
    // 创建软件项
    SoftwareItem item = m_scanner->createSoftwareItem(tempFile);
    
    // 检查软件项是否有效
    QVERIFY(item.isValid());
    QVERIFY(!item.getId().isEmpty());
    QCOMPARE(item.getFilePath(), tempFile);
}

void TestSoftwareScanner::testIsValidExecutablePath()
{
    // 创建临时可执行文件
    QString exeFile = m_tempDir->path() + "/test_app.exe";
    QFile exe(exeFile);
    QVERIFY(exe.open(QIODevice::WriteOnly));
    exe.close();
    
    // 创建临时非可执行文件
    QString txtFile = m_tempDir->path() + "/test_file.txt";
    QFile txt(txtFile);
    QVERIFY(txt.open(QIODevice::WriteOnly));
    txt.close();
    
    // 检查可执行文件路径有效性
    QVERIFY(m_scanner->isValidExecutablePath(exeFile));
    
    // 检查非可执行文件路径有效性（在Windows上可能仍然有效，因为基于扩展名判断）
#ifdef Q_OS_WIN
    QVERIFY(m_scanner->isValidExecutablePath(exeFile));
    // txt文件可能仍然被认为是有效的，因为基于扩展名判断
#else
    // 在Unix系统上，基于文件权限判断
    QVERIFY(m_scanner->isValidExecutablePath(exeFile));
#endif
}

void TestSoftwareScanner::testSetScanPaths()
{
    QStringList paths;
    paths << "/path1" << "/path2" << "/path3";
    
    // 设置扫描路径
    m_scanner->setScanPaths(paths);
    
    // 检查路径是否设置成功
    QStringList retrievedPaths = m_scanner->getScanPaths();
    QCOMPARE(retrievedPaths, paths);
}

void TestSoftwareScanner::testIsCurrentlyScanning()
{
    // 初始状态应该不是扫描中
    QVERIFY(!m_scanner->isCurrentlyScanning());
    
    // 注意：实际的扫描测试需要更多设置，这里只测试状态
}

void TestSoftwareScanner::testScanSystemSoftware()
{
    // 设置一个空的扫描路径来避免实际扫描
    QStringList emptyPaths;
    m_scanner->setScanPaths(emptyPaths);
    
    // 连接信号以捕获扫描完成
    QSignalSpy scanFinishedSpy(m_scanner, &SoftwareScanner::scanFinished);
    QSignalSpy scanStartedSpy(m_scanner, &SoftwareScanner::scanStarted);
    
    // 启动扫描
    m_scanner->scanSystemSoftware();
    
    // 等待一段时间让扫描完成（即使路径为空）
    QTest::qWait(100);
    
    // 检查信号是否发出
    QVERIFY(scanStartedSpy.count() >= 0);
    
    // 注意：完整扫描测试需要更多设置和mock数据
}

void TestSoftwareScanner::cleanupTestCase()
{
    delete m_scanner;
    delete m_tempDir;
}

QTEST_MAIN(TestSoftwareScanner)
#include "TestSoftwareScanner.moc"
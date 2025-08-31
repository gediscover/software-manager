#include "SettingsDialog.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QGroupBox>
#include <QDir>
#include "../utils/Logging.hpp"

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setupUI();
    loadSettings();
    
    // 设置窗口属性
    setWindowTitle("设置");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(500, 400);
}

void SettingsDialog::onAddPathClicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "选择扫描路径");
    if (!path.isEmpty()) {
        // 检查路径是否已存在
        bool exists = false;
        for (int i = 0; i < m_scanPathsList->count(); ++i) {
            if (m_scanPathsList->item(i)->text() == path) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            m_scanPathsList->addItem(path);
        }
    }
}

void SettingsDialog::onRemovePathClicked()
{
    QListWidgetItem* currentItem = m_scanPathsList->currentItem();
    if (currentItem) {
        delete m_scanPathsList->takeItem(m_scanPathsList->row(currentItem));
    }
}

void SettingsDialog::onSaveClicked()
{
    saveSettings();
    accept();
}

void SettingsDialog::onCancelClicked()
{
    reject();
}

void SettingsDialog::onRestoreDefaultsClicked()
{
    int ret = QMessageBox::question(this, "确认", "确定要恢复默认设置吗？");
    if (ret == QMessageBox::Yes) {
        // 恢复默认设置
        m_autoScanCheckBox->setChecked(true);
        m_viewModeComboBox->setCurrentIndex(0); // 网格视图
        m_iconSizeSpinBox->setValue(64);
        m_minimizeToTrayCheckBox->setChecked(true);
        m_closeToTrayCheckBox->setChecked(true);
        
        // 清空扫描路径列表
        m_scanPathsList->clear();
        
        // 添加默认路径
#ifdef Q_OS_WIN
        m_scanPathsList->addItem("C:/ProgramData/Microsoft/Windows/Start Menu/Programs");
        m_scanPathsList->addItem(QDir::homePath() + "/Desktop");
#elif defined(Q_OS_MAC)
        m_scanPathsList->addItem("/Applications");
        m_scanPathsList->addItem(QDir::homePath() + "/Applications");
        m_scanPathsList->addItem(QDir::homePath() + "/Desktop");
#else
        m_scanPathsList->addItem("/usr/share/applications");
        m_scanPathsList->addItem(QDir::homePath() + "/.local/share/applications");
        m_scanPathsList->addItem(QDir::homePath() + "/Desktop");
#endif
        
        qCInfo(softwareManager) << "已恢复默认设置";
    }
}

void SettingsDialog::setupUI()
{
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 创建标签页布局（简化为单页）
    
    // 扫描设置组
    QGroupBox* scanGroup = new QGroupBox("扫描设置");
    QGridLayout* scanLayout = new QGridLayout(scanGroup);
    
    scanLayout->addWidget(new QLabel("扫描路径:"), 0, 0);
    
    m_scanPathsList = new QListWidget();
    scanLayout->addWidget(m_scanPathsList, 1, 0, 1, 2);
    
    QHBoxLayout* pathButtonLayout = new QHBoxLayout();
    m_addPathButton = new QPushButton("添加路径");
    m_removePathButton = new QPushButton("移除路径");
    pathButtonLayout->addWidget(m_addPathButton);
    pathButtonLayout->addWidget(m_removePathButton);
    pathButtonLayout->addStretch();
    scanLayout->addLayout(pathButtonLayout, 2, 0, 1, 2);
    
    m_autoScanCheckBox = new QCheckBox("启动时自动扫描");
    scanLayout->addWidget(m_autoScanCheckBox, 3, 0, 1, 2);
    
    // 视图设置组
    QGroupBox* viewGroup = new QGroupBox("视图设置");
    QGridLayout* viewLayout = new QGridLayout(viewGroup);
    
    viewLayout->addWidget(new QLabel("默认视图模式:"), 0, 0);
    m_viewModeComboBox = new QComboBox();
    m_viewModeComboBox->addItem("网格视图", "grid");
    m_viewModeComboBox->addItem("列表视图", "list");
    viewLayout->addWidget(m_viewModeComboBox, 0, 1);
    
    viewLayout->addWidget(new QLabel("图标大小:"), 1, 0);
    m_iconSizeSpinBox = new QSpinBox();
    m_iconSizeSpinBox->setRange(32, 128);
    m_iconSizeSpinBox->setSingleStep(16);
    viewLayout->addWidget(m_iconSizeSpinBox, 1, 1);
    
    // 系统托盘设置组
    QGroupBox* trayGroup = new QGroupBox("系统托盘");
    QVBoxLayout* trayLayout = new QVBoxLayout(trayGroup);
    
    m_minimizeToTrayCheckBox = new QCheckBox("最小化时隐藏到系统托盘");
    trayLayout->addWidget(m_minimizeToTrayCheckBox);
    
    m_closeToTrayCheckBox = new QCheckBox("关闭时隐藏到系统托盘");
    trayLayout->addWidget(m_closeToTrayCheckBox);
    
    // 添加到主布局
    mainLayout->addWidget(scanGroup);
    mainLayout->addWidget(viewGroup);
    mainLayout->addWidget(trayGroup);
    
    // 按钮布局
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    m_restoreDefaultsButton = new QPushButton("恢复默认设置");
    m_saveButton = new QPushButton("保存");
    m_cancelButton = new QPushButton("取消");
    
    buttonLayout->addWidget(m_restoreDefaultsButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号槽
    connect(m_addPathButton, &QPushButton::clicked, this, &SettingsDialog::onAddPathClicked);
    connect(m_removePathButton, &QPushButton::clicked, this, &SettingsDialog::onRemovePathClicked);
    connect(m_saveButton, &QPushButton::clicked, this, &SettingsDialog::onSaveClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancelClicked);
    connect(m_restoreDefaultsButton, &QPushButton::clicked, this, &SettingsDialog::onRestoreDefaultsClicked);
}

void SettingsDialog::loadSettings()
{
    QSettings settings;
    
    // 加载扫描设置
    m_scanPaths = settings.value("Scan/Paths").toStringList();
    for (const QString& path : m_scanPaths) {
        m_scanPathsList->addItem(path);
    }
    
    m_autoScanCheckBox->setChecked(settings.value("Scan/AutoScan", true).toBool());
    
    // 加载视图设置
    QString viewMode = settings.value("View/Mode", "grid").toString();
    m_viewModeComboBox->setCurrentIndex(viewMode == "grid" ? 0 : 1);
    m_iconSizeSpinBox->setValue(settings.value("View/IconSize", 64).toInt());
    
    // 加载系统托盘设置
    m_minimizeToTrayCheckBox->setChecked(settings.value("Tray/MinimizeToTray", true).toBool());
    m_closeToTrayCheckBox->setChecked(settings.value("Tray/CloseToTray", true).toBool());
}

void SettingsDialog::saveSettings()
{
    QSettings settings;
    
    // 保存扫描设置
    QStringList paths;
    for (int i = 0; i < m_scanPathsList->count(); ++i) {
        paths.append(m_scanPathsList->item(i)->text());
    }
    settings.setValue("Scan/Paths", paths);
    settings.setValue("Scan/AutoScan", m_autoScanCheckBox->isChecked());
    
    // 保存视图设置
    settings.setValue("View/Mode", m_viewModeComboBox->currentData().toString());
    settings.setValue("View/IconSize", m_iconSizeSpinBox->value());
    
    // 保存系统托盘设置
    settings.setValue("Tray/MinimizeToTray", m_minimizeToTrayCheckBox->isChecked());
    settings.setValue("Tray/CloseToTray", m_closeToTrayCheckBox->isChecked());
    
    qCInfo(softwareManager) << "设置已保存";
}
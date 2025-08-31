#include "SoftwareItemWidget.hpp"
#include "../model/SoftwareItem.hpp"
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include "../utils/Logging.hpp"

SoftwareItemWidget::SoftwareItemWidget(const SoftwareItem& item, QWidget* parent)
    : QWidget(parent)
    , m_softwareItem(item)
    , m_iconLabel(nullptr)
    , m_nameLabel(nullptr)
    , m_layout(nullptr)
    , m_iconSize(64, 64)
    , m_contextMenu(nullptr)
{
    setupUI();
    updateDisplay();
    createContextMenu();
    
    // 设置窗口属性
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setCursor(Qt::PointingHandCursor);
}

void SoftwareItemWidget::setIconSize(const QSize& size)
{
    m_iconSize = size;
    updateDisplay();
}

QSize SoftwareItemWidget::iconSize() const
{
    return m_iconSize;
}

QString SoftwareItemWidget::softwareId() const
{
    return m_softwareItem.getId();
}

SoftwareItem SoftwareItemWidget::softwareItem() const
{
    return m_softwareItem;
}

void SoftwareItemWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    } else if (event->button() == Qt::RightButton) {
        emit rightClicked(event->globalPos());
        if (m_contextMenu) {
            m_contextMenu->popup(event->globalPos());
        }
    }
    
    QWidget::mousePressEvent(event);
}

void SoftwareItemWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked();
        emit launchRequested(m_softwareItem.getId());
    }
    
    QWidget::mouseDoubleClickEvent(event);
}

void SoftwareItemWidget::onLaunchAction()
{
    emit launchRequested(m_softwareItem.getId());
}

void SoftwareItemWidget::onOpenLocationAction()
{
    QString filePath = m_softwareItem.getFilePath();
    QFileInfo fileInfo(filePath);
    
    // 打开文件所在目录
    QString dirPath = fileInfo.absolutePath();
    if (QDir(dirPath).exists()) {
#ifdef Q_OS_WIN
        // Windows: 使用explorer打开并选中文件
        QStringList args;
        args << "/select," << QDir::toNativeSeparators(filePath);
        QProcess::startDetached("explorer", args);
#elif defined(Q_OS_MAC)
        // macOS: 使用Finder打开并选中文件
        QStringList args;
        args << "-e" << "tell application \"Finder\"";
        args << "-e" << "activate";
        args << "-e" << QString("select POSIX file \"%1\"").arg(filePath);
        args << "-e" << "end tell";
        QProcess::startDetached("osascript", args);
#else
        // Linux: 打开文件所在目录
        QProcess::startDetached("xdg-open", QStringList() << dirPath);
#endif
    } else {
        QMessageBox::warning(this, "错误", "文件路径不存在");
    }
}

void SoftwareItemWidget::onRemoveAction()
{
    int ret = QMessageBox::question(this, "确认", 
                                  QString("确定要从管理器中移除 \"%1\" 吗？\n(注意：这不会删除实际的软件文件)").arg(m_softwareItem.getName()));
    if (ret == QMessageBox::Yes) {
        emit removeRequested(m_softwareItem.getId());
    }
}

void SoftwareItemWidget::onPropertiesAction()
{
    emit propertiesRequested(m_softwareItem.getId());
}

void SoftwareItemWidget::setupUI()
{
    // 创建布局
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(5, 5, 5, 5);
    m_layout->setSpacing(5);
    
    // 创建图标标签
    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setFixedSize(m_iconSize);
    
    // 创建名称标签
    m_nameLabel = new QLabel(this);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setWordWrap(true);
    
    // 添加到布局
    m_layout->addWidget(m_iconLabel);
    m_layout->addWidget(m_nameLabel);
}

void SoftwareItemWidget::updateDisplay()
{
    if (m_iconLabel && m_nameLabel) {
        // 设置图标
        QIcon icon = m_softwareItem.getIcon();
        if (!icon.isNull()) {
            m_iconLabel->setPixmap(icon.pixmap(m_iconSize));
        } else {
            // 使用默认图标
            m_iconLabel->setPixmap(QIcon().pixmap(m_iconSize));
        }
        
        // 设置名称
        QString name = m_softwareItem.getName();
        // 如果名称过长，截断并添加省略号
        if (name.length() > 15) {
            name = name.left(12) + "...";
        }
        m_nameLabel->setText(name);
        
        // 设置工具提示
        m_iconLabel->setToolTip(m_softwareItem.getName());
        m_nameLabel->setToolTip(m_softwareItem.getName());
    }
}

void SoftwareItemWidget::createContextMenu()
{
    m_contextMenu = new QMenu(this);
    
    QAction* launchAction = new QAction("启动", this);
    connect(launchAction, &QAction::triggered, this, &SoftwareItemWidget::onLaunchAction);
    m_contextMenu->addAction(launchAction);
    
    QAction* openLocationAction = new QAction("打开文件位置", this);
    connect(openLocationAction, &QAction::triggered, this, &SoftwareItemWidget::onOpenLocationAction);
    m_contextMenu->addAction(openLocationAction);
    
    m_contextMenu->addSeparator();
    
    QAction* removeAction = new QAction("从管理器移除", this);
    connect(removeAction, &QAction::triggered, this, &SoftwareItemWidget::onRemoveAction);
    m_contextMenu->addAction(removeAction);
    
    QAction* propertiesAction = new QAction("属性", this);
    connect(propertiesAction, &QAction::triggered, this, &SoftwareItemWidget::onPropertiesAction);
    m_contextMenu->addAction(propertiesAction);
}
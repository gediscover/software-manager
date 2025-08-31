#include "SoftwareItemWidget.hpp"
#include "../model/SoftwareItem.hpp"
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(softwareManager, "softwaremanager")

SoftwareItemWidget::SoftwareItemWidget(const SoftwareItem& item, QWidget* parent)
    : QWidget(parent)
    , m_softwareItem(item)
    , m_iconLabel(nullptr)
    , m_nameLabel(nullptr)
    , m_layout(nullptr)
    , m_iconSize(64, 64)
{
    setupUI();
    updateDisplay();
    
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

void SoftwareItemWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    } else if (event->button() == Qt::RightButton) {
        emit rightClicked(event->globalPos());
    }
    
    QWidget::mousePressEvent(event);
}

void SoftwareItemWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked();
    }
    
    QWidget::mouseDoubleClickEvent(event);
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
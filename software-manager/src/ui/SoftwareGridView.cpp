#include "SoftwareGridView.hpp"
#include "SoftwareItemWidget.hpp"  // 需要创建这个类
#include "../model/SoftwareItem.hpp"
#include <QScrollArea>
#include <QGridLayout>
#include "../utils/Logging.hpp"

SoftwareGridView::SoftwareGridView(QWidget* parent)
    : QWidget(parent)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
    , m_gridLayout(nullptr)
    , m_iconSize(64)
    , m_columns(10)
{
    setupUI();
}

void SoftwareGridView::addSoftwareItem(const SoftwareItem& item)
{
    m_softwareItems.append(item);
    updateLayout();
    qCInfo(softwareManager) << "添加软件项到网格视图:" << item.getName();
}

void SoftwareGridView::removeSoftwareItem(const QString& id)
{
    // 查找并移除指定ID的软件项
    for (auto it = m_softwareItems.begin(); it != m_softwareItems.end(); ++it) {
        if (it->getId() == id) {
            m_softwareItems.erase(it);
            break;
        }
    }
    
    updateLayout();
    qCInfo(softwareManager) << "从网格视图移除软件项:" << id;
}

void SoftwareGridView::updateSoftwareItem(const SoftwareItem& item)
{
    // 查找并更新指定ID的软件项
    for (auto& softwareItem : m_softwareItems) {
        if (softwareItem.getId() == item.getId()) {
            softwareItem = item;
            break;
        }
    }
    
    updateLayout();
    qCInfo(softwareManager) << "更新网格视图中的软件项:" << item.getName();
}

void SoftwareGridView::clearAllItems()
{
    m_softwareItems.clear();
    updateLayout();
    qCInfo(softwareManager) << "清空网格视图中的所有软件项";
}

void SoftwareGridView::setSoftwareItems(const QList<SoftwareItem>& items)
{
    m_softwareItems = items;
    updateLayout();
    qCInfo(softwareManager) << "设置网格视图软件项，共" << items.size() << "个";
}

void SoftwareGridView::setIconSize(int size)
{
    m_iconSize = size;
    updateLayout();
    qCInfo(softwareManager) << "设置网格视图图标大小:" << size;
}

int SoftwareGridView::iconSize() const
{
    return m_iconSize;
}

void SoftwareGridView::setupUI()
{
    // 创建滚动区域
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // 创建内容部件
    m_contentWidget = new QWidget(m_scrollArea);
    m_scrollArea->setWidget(m_contentWidget);
    
    // 创建网格布局
    m_gridLayout = new QGridLayout(m_contentWidget);
    m_gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_gridLayout->setSpacing(10);
    m_gridLayout->setContentsMargins(10, 10, 10, 10);
    
    // 设置主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_scrollArea);
}

void SoftwareGridView::updateLayout()
{
    // 清除现有布局项
    QLayoutItem* item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    
    // 重新添加软件项
    int row = 0;
    int col = 0;
    
    for (const SoftwareItem& item : m_softwareItems) {
        // 创建软件项控件（需要实现SoftwareItemWidget类）
        // SoftwareItemWidget* widget = new SoftwareItemWidget(item, this);
        // widget->setIconSize(QSize(m_iconSize, m_iconSize));
        
        // m_gridLayout->addWidget(widget, row, col);
        
        // 更新行列位置
        col++;
        if (col >= m_columns) {
            col = 0;
            row++;
        }
    }
}
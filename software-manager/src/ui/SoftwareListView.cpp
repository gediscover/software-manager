#include "SoftwareListView.hpp"
#include "../model/SoftwareItem.hpp"
#include <QTableWidget>
#include <QHeaderView>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(softwareManager, "softwaremanager")

SoftwareListView::SoftwareListView(QWidget* parent)
    : QWidget(parent)
    , m_tableWidget(nullptr)
{
    setupUI();
}

void SoftwareListView::addSoftwareItem(const SoftwareItem& item)
{
    m_softwareItems.append(item);
    updateTable();
    qCInfo(softwareManager) << "添加软件项到列表视图:" << item.getName();
}

void SoftwareListView::removeSoftwareItem(const QString& id)
{
    // 查找并移除指定ID的软件项
    for (auto it = m_softwareItems.begin(); it != m_softwareItems.end(); ++it) {
        if (it->getId() == id) {
            m_softwareItems.erase(it);
            break;
        }
    }
    
    updateTable();
    qCInfo(softwareManager) << "从列表视图移除软件项:" << id;
}

void SoftwareListView::updateSoftwareItem(const SoftwareItem& item)
{
    // 查找并更新指定ID的软件项
    for (auto& softwareItem : m_softwareItems) {
        if (softwareItem.getId() == item.getId()) {
            softwareItem = item;
            break;
        }
    }
    
    updateTable();
    qCInfo(softwareManager) << "更新列表视图中的软件项:" << item.getName();
}

void SoftwareListView::clearAllItems()
{
    m_softwareItems.clear();
    updateTable();
    qCInfo(softwareManager) << "清空列表视图中的所有软件项";
}

void SoftwareListView::setSoftwareItems(const QList<SoftwareItem>& items)
{
    m_softwareItems = items;
    updateTable();
    qCInfo(softwareManager) << "设置列表视图软件项，共" << items.size() << "个";
}

void SoftwareListView::setColumnWidth(int column, int width)
{
    if (m_tableWidget) {
        m_tableWidget->setColumnWidth(column, width);
    }
}

void SoftwareListView::setSortColumn(int column, Qt::SortOrder order)
{
    if (m_tableWidget) {
        m_tableWidget->sortByColumn(column, order);
    }
}

void SoftwareListView::setupUI()
{
    // 创建表格控件
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 设置列数和表头
    m_tableWidget->setColumnCount(5);
    QStringList headers;
    headers << "名称" << "分类" << "路径" << "版本" << "描述";
    m_tableWidget->setHorizontalHeaderLabels(headers);
    
    // 设置列宽
    m_tableWidget->setColumnWidth(0, 200);  // 名称
    m_tableWidget->setColumnWidth(1, 100);  // 分类
    m_tableWidget->setColumnWidth(2, 300);  // 路径
    m_tableWidget->setColumnWidth(3, 80);   // 版本
    m_tableWidget->setColumnWidth(4, 200);  // 描述
    
    // 设置表头行为
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    m_tableWidget->verticalHeader()->setVisible(false);
    
    // 设置主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_tableWidget);
}

void SoftwareListView::updateTable()
{
    if (!m_tableWidget) {
        return;
    }
    
    // 清空表格
    m_tableWidget->setRowCount(0);
    
    // 设置行数
    m_tableWidget->setRowCount(m_softwareItems.size());
    
    // 填充数据
    for (int i = 0; i < m_softwareItems.size(); ++i) {
        const SoftwareItem& item = m_softwareItems[i];
        
        // 名称
        QTableWidgetItem* nameItem = new QTableWidgetItem(item.getName());
        nameItem->setData(Qt::UserRole, item.getId());
        m_tableWidget->setItem(i, 0, nameItem);
        
        // 分类
        m_tableWidget->setItem(i, 1, new QTableWidgetItem(item.getCategory()));
        
        // 路径
        m_tableWidget->setItem(i, 2, new QTableWidgetItem(item.getFilePath()));
        
        // 版本
        m_tableWidget->setItem(i, 3, new QTableWidgetItem(item.getVersion()));
        
        // 描述
        m_tableWidget->setItem(i, 4, new QTableWidgetItem(item.getDescription()));
    }
}
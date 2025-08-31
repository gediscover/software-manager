#include "SoftwareListView.hpp"
#include "../model/SoftwareItem.hpp"
#include <QTableWidget>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include "../utils/Logging.hpp"

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
    m_softwareRowMap.clear();
    updateTable();
    qCInfo(softwareManager) << "清空列表视图中的所有软件项";
}

void SoftwareListView::setSoftwareItems(const QList<SoftwareItem>& items)
{
    m_softwareItems = items;
    m_softwareRowMap.clear();
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

void SoftwareListView::onItemDoubleClicked(QTableWidgetItem* item)
{
    if (item) {
        QString softwareId = item->data(Qt::UserRole).toString();
        if (!softwareId.isEmpty()) {
            emit softwareItemLaunched(softwareId);
        }
    }
}

void SoftwareListView::onItemRightClicked(const QPoint& pos)
{
    QTableWidgetItem* item = m_tableWidget->itemAt(pos);
    if (item) {
        QString softwareId = item->data(Qt::UserRole).toString();
        if (!softwareId.isEmpty()) {
            // 创建右键菜单
            QMenu contextMenu(this);
            
            QAction* launchAction = contextMenu.addAction("启动");
            QAction* openLocationAction = contextMenu.addAction("打开文件位置");
            contextMenu.addSeparator();
            QAction* removeAction = contextMenu.addAction("从管理器移除");
            QAction* propertiesAction = contextMenu.addAction("属性");
            
            // 连接动作
            connect(launchAction, &QAction::triggered, [this, softwareId]() {
                emit softwareItemLaunched(softwareId);
            });
            
            connect(openLocationAction, &QAction::triggered, [this, softwareId]() {
                // 查找对应的软件项
                for (const SoftwareItem& item : m_softwareItems) {
                    if (item.getId() == softwareId) {
                        QString filePath = item.getFilePath();
                        QFileInfo fileInfo(filePath);
                        
                        // 打开文件所在目录
                        QString dirPath = fileInfo.absolutePath();
                        if (QDir(dirPath).exists()) {
#ifdef Q_OS_WIN
                            QStringList args;
                            args << "/select," << QDir::toNativeSeparators(filePath);
                            QProcess::startDetached("explorer", args);
#elif defined(Q_OS_MAC)
                            QStringList args;
                            args << "-e" << "tell application \"Finder\"";
                            args << "-e" << "activate";
                            args << "-e" << QString("select POSIX file \"%1\"").arg(filePath);
                            args << "-e" << "end tell";
                            QProcess::startDetached("osascript", args);
#else
                            QProcess::startDetached("xdg-open", QStringList() << dirPath);
#endif
                        } else {
                            QMessageBox::warning(this, "错误", "文件路径不存在");
                        }
                        break;
                    }
                }
            });
            
            connect(removeAction, &QAction::triggered, [this, softwareId]() {
                int ret = QMessageBox::question(this, "确认", "确定要从管理器中移除该软件吗？\n(注意：这不会删除实际的软件文件)");
                if (ret == QMessageBox::Yes) {
                    emit softwareItemRemoved(softwareId);
                }
            });
            
            connect(propertiesAction, &QAction::triggered, [this, softwareId]() {
                emit softwareItemPropertiesRequested(softwareId);
            });
            
            // 显示菜单
            contextMenu.exec(m_tableWidget->viewport()->mapToGlobal(pos));
        }
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
    
    // 启用上下文菜单
    m_tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    
    // 设置主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_tableWidget);
    
    // 连接信号槽
    connect(m_tableWidget, &QTableWidget::itemDoubleClicked, 
            this, &SoftwareListView::onItemDoubleClicked);
    connect(m_tableWidget, &QTableWidget::customContextMenuRequested, 
            this, &SoftwareListView::onItemRightClicked);
}

void SoftwareListView::updateTable()
{
    if (!m_tableWidget) {
        return;
    }
    
    // 清空表格
    m_tableWidget->setRowCount(0);
    m_softwareRowMap.clear();
    
    // 设置行数
    m_tableWidget->setRowCount(m_softwareItems.size());
    
    // 填充数据
    for (int i = 0; i < m_softwareItems.size(); ++i) {
        const SoftwareItem& item = m_softwareItems[i];
        
        // 存储行映射
        m_softwareRowMap[item.getId()] = i;
        
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
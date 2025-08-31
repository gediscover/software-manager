#include "SidebarWidget.hpp"
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include "../utils/Logging.hpp"

SidebarWidget::SidebarWidget(QWidget* parent)
    : QWidget(parent)
    , m_categoryList(nullptr)
    , m_addButton(nullptr)
    , m_removeButton(nullptr)
{
    setupUI();
    loadCategories();
}

void SidebarWidget::addCategory(const QString& name)
{
    if (!name.isEmpty() && m_categoryList) {
        m_categoryList->addItem(name);
        emit categoryAdded(name);
        qCInfo(softwareManager) << "添加分类到侧边栏:" << name;
    }
}

void SidebarWidget::removeCategory(const QString& name)
{
    if (m_categoryList) {
        QList<QListWidgetItem*> items = m_categoryList->findItems(name, Qt::MatchExactly);
        for (QListWidgetItem* item : items) {
            delete m_categoryList->takeItem(m_categoryList->row(item));
        }
        emit categoryRemoved(name);
        qCInfo(softwareManager) << "从侧边栏移除分类:" << name;
    }
}

void SidebarWidget::updateCategoryList()
{
    loadCategories();
}

QStringList SidebarWidget::getCategories() const
{
    QStringList categories;
    if (m_categoryList) {
        for (int i = 0; i < m_categoryList->count(); ++i) {
            categories.append(m_categoryList->item(i)->text());
        }
    }
    return categories;
}

void SidebarWidget::onCategoryItemClicked()
{
    if (m_categoryList) {
        QListWidgetItem* currentItem = m_categoryList->currentItem();
        if (currentItem) {
            QString category = currentItem->text();
            emit categorySelected(category);
        }
    }
}

void SidebarWidget::onAddCategoryClicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, "添加分类", "请输入分类名称:", QLineEdit::Normal, "", &ok);
    
    if (ok && !name.isEmpty()) {
        // 验证分类名称
        if (name.length() > 50) {
            QMessageBox::warning(this, "错误", "分类名称不能超过50个字符");
            return;
        }
        
        // 检查是否已存在
        if (getCategories().contains(name)) {
            QMessageBox::warning(this, "错误", "分类名称已存在");
            return;
        }
        
        addCategory(name);
    }
}

void SidebarWidget::onRemoveCategoryClicked()
{
    if (m_categoryList) {
        QListWidgetItem* currentItem = m_categoryList->currentItem();
        if (currentItem) {
            QString category = currentItem->text();
            
            // 不允许删除内置分类
            if (category == "所有软件" || category == "未分类") {
                QMessageBox::warning(this, "错误", "不能删除内置分类");
                return;
            }
            
            int ret = QMessageBox::question(this, "确认", 
                                          QString("确定要删除分类 \"%1\" 吗？").arg(category));
            if (ret == QMessageBox::Yes) {
                removeCategory(category);
            }
        } else {
            QMessageBox::information(this, "提示", "请先选择要删除的分类");
        }
    }
}

void SidebarWidget::onCategoryItemDoubleClicked()
{
    // 双击分类项可以重命名
    if (m_categoryList) {
        QListWidgetItem* currentItem = m_categoryList->currentItem();
        if (currentItem) {
            QString oldName = currentItem->text();
            
            // 不允许重命名内置分类
            if (oldName == "所有软件" || oldName == "未分类") {
                QMessageBox::warning(this, "错误", "不能重命名内置分类");
                return;
            }
            
            bool ok;
            QString newName = QInputDialog::getText(this, "重命名分类", "请输入新的分类名称:", 
                                                  QLineEdit::Normal, oldName, &ok);
            
            if (ok && !newName.isEmpty() && newName != oldName) {
                // 验证新名称
                if (newName.length() > 50) {
                    QMessageBox::warning(this, "错误", "分类名称不能超过50个字符");
                    return;
                }
                
                // 检查是否已存在
                if (getCategories().contains(newName)) {
                    QMessageBox::warning(this, "错误", "分类名称已存在");
                    return;
                }
                
                currentItem->setText(newName);
                emit categoryRemoved(oldName);
                emit categoryAdded(newName);
                qCInfo(softwareManager) << "重命名分类:" << oldName << "->" << newName;
            }
        }
    }
}

void SidebarWidget::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    
    // 创建分类列表
    m_categoryList = new QListWidget(this);
    m_categoryList->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(m_categoryList);
    
    // 创建按钮布局
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_addButton = new QPushButton("＋ 添加分类", this);
    m_removeButton = new QPushButton("－ 删除分类", this);
    
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_removeButton);
    
    layout->addLayout(buttonLayout);
    
    // 连接信号槽
    connect(m_categoryList, &QListWidget::itemClicked, 
            this, &SidebarWidget::onCategoryItemClicked);
    connect(m_categoryList, &QListWidget::itemDoubleClicked, 
            this, &SidebarWidget::onCategoryItemDoubleClicked);
    connect(m_addButton, &QPushButton::clicked, 
            this, &SidebarWidget::onAddCategoryClicked);
    connect(m_removeButton, &QPushButton::clicked, 
            this, &SidebarWidget::onRemoveCategoryClicked);
}

void SidebarWidget::loadCategories()
{
    if (m_categoryList) {
        // 清空现有项
        m_categoryList->clear();
        
        // 添加默认分类
        m_categoryList->addItem("所有软件");
        m_categoryList->addItem("未分类");
        
        // 这里应该从CategoryManager加载用户自定义分类
        // 暂时添加一些示例分类
        m_categoryList->addItem("办公软件");
        m_categoryList->addItem("开发工具");
        m_categoryList->addItem("娱乐媒体");
        m_categoryList->addItem("系统工具");
        
        // 默认选中"所有软件"
        if (m_categoryList->count() > 0) {
            m_categoryList->setCurrentRow(0);
        }
    }
}
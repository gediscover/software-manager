#include "SearchDialog.hpp"
#include "../model/SoftwareItem.hpp"
#include "../core/DatabaseManager.hpp"
#include "MainWindow.hpp"
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QApplication>
#include "../utils/Logging.hpp"

SearchDialog::SearchDialog(QWidget* parent)
    : QDialog(parent)
    , m_searchLineEdit(nullptr)
    , m_resultListWidget(nullptr)
    , m_searchButton(nullptr)
    , m_launchButton(nullptr)
    , m_closeButton(nullptr)
{
    setupUI();
    
    // 设置窗口属性
    setWindowTitle("搜索软件");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    resize(500, 400);
    
    // 延迟初始化数据库管理器
    QTimer::singleShot(0, this, [this]() {
        // 获取主窗口的数据库管理器
        QWidget* parentWidget = this->parentWidget();
        while (parentWidget && !parentWidget->inherits("MainWindow")) {
            parentWidget = parentWidget->parentWidget();
        }
        
        if (parentWidget) {
            // 这里需要MainWindow提供访问数据库管理器的方法
            // 暂时使用局部搜索
        }
    });
}

void SearchDialog::setSearchKeyword(const QString& keyword)
{
    if (m_searchLineEdit) {
        m_searchLineEdit->setText(keyword);
        performSearch();
    }
}

QString SearchDialog::searchKeyword() const
{
    if (m_searchLineEdit) {
        return m_searchLineEdit->text();
    }
    return QString();
}

void SearchDialog::setSearchResults(const QList<SoftwareItem>& results)
{
    m_searchResults = results;
    updateSearchResults();
}

QList<SoftwareItem> SearchDialog::searchResults() const
{
    return m_searchResults;
}

void SearchDialog::onSearchTextChanged()
{
    // 延迟搜索以提高性能
    static QTimer* searchTimer = nullptr;
    if (!searchTimer) {
        searchTimer = new QTimer(this);
        searchTimer->setSingleShot(true);
        connect(searchTimer, &QTimer::timeout, this, &SearchDialog::performSearch);
    }
    
    searchTimer->start(300); // 300ms延迟
}

void SearchDialog::onSearchButtonClicked()
{
    performSearch();
}

void SearchDialog::onLaunchButtonClicked()
{
    if (m_resultListWidget) {
        QListWidgetItem* currentItem = m_resultListWidget->currentItem();
        if (currentItem) {
            QString softwareId = currentItem->data(Qt::UserRole).toString();
            qCInfo(softwareManager) << "启动软件:" << currentItem->text() << "ID:" << softwareId;
            
            // 发射信号让主窗口处理启动逻辑
            emit softwareLaunchRequested(softwareId);
            
            // 启动成功后关闭对话框
            accept();
        }
    }
}

void SearchDialog::onListItemDoubleClicked()
{
    // 双击列表项启动软件
    onLaunchButtonClicked();
}

void SearchDialog::setupUI()
{
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 创建搜索区域
    QHBoxLayout* searchLayout = new QHBoxLayout();
    
    m_searchLineEdit = new QLineEdit(this);
    m_searchLineEdit->setPlaceholderText("输入软件名称进行搜索...");
    
    m_searchButton = new QPushButton("搜索", this);
    
    searchLayout->addWidget(new QLabel("搜索:", this));
    searchLayout->addWidget(m_searchLineEdit);
    searchLayout->addWidget(m_searchButton);
    
    mainLayout->addLayout(searchLayout);
    
    // 创建结果列表
    m_resultListWidget = new QListWidget(this);
    mainLayout->addWidget(m_resultListWidget);
    
    // 创建按钮区域
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    m_launchButton = new QPushButton("启动", this);
    m_closeButton = new QPushButton("关闭", this);
    
    // 设置启动按钮默认不可用
    m_launchButton->setEnabled(false);
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_launchButton);
    buttonLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号槽
    connect(m_searchLineEdit, &QLineEdit::textChanged, 
            this, &SearchDialog::onSearchTextChanged);
    connect(m_searchButton, &QPushButton::clicked, 
            this, &SearchDialog::onSearchButtonClicked);
    connect(m_launchButton, &QPushButton::clicked, 
            this, &SearchDialog::onLaunchButtonClicked);
    connect(m_closeButton, &QPushButton::clicked, 
            this, &QDialog::reject);
    connect(m_resultListWidget, &QListWidget::itemDoubleClicked, 
            this, &SearchDialog::onListItemDoubleClicked);
    connect(m_resultListWidget, &QListWidget::itemSelectionChanged, 
            this, [this]() {
                m_launchButton->setEnabled(m_resultListWidget->currentItem() != nullptr);
            });
}

void SearchDialog::performSearch()
{
    QString keyword = searchKeyword();
    
    // 如果关键字为空，清空结果
    if (keyword.isEmpty()) {
        m_searchResults.clear();
        updateSearchResults();
        return;
    }
    
    // 获取主窗口的数据库管理器
    MainWindow* mainWindow = nullptr;
    QWidget* parentWidget = this->parentWidget();
    while (parentWidget) {
        if (MainWindow* mw = qobject_cast<MainWindow*>(parentWidget)) {
            mainWindow = mw;
            break;
        }
        parentWidget = parentWidget->parentWidget();
    }
    
    if (mainWindow && mainWindow->databaseManager()) {
        // 从数据库中搜索匹配的软件项
        QList<SoftwareItem> allItems = mainWindow->databaseManager()->getAllSoftwareItems();
        
        // 筛选匹配的项目
        m_searchResults.clear();
        for (const SoftwareItem& item : allItems) {
            if (item.getName().contains(keyword, Qt::CaseInsensitive) || 
                item.getDescription().contains(keyword, Qt::CaseInsensitive)) {
                m_searchResults.append(item);
            }
        }
    } else {
        // 如果无法获取数据库管理器，使用模拟数据
        m_searchResults.clear();
        
        // 模拟搜索结果
        // SoftwareItem item1("Example Software 1");
        // item1.setCategory("办公软件");
        // m_searchResults.append(item1);
    }
    
    updateSearchResults();
    
    qCInfo(softwareManager) << "执行搜索，关键字:" << keyword << "，结果数量:" << m_searchResults.size();
}

void SearchDialog::updateSearchResults()
{
    if (!m_resultListWidget) {
        return;
    }
    
    // 清空列表
    m_resultListWidget->clear();
    
    // 添加搜索结果
    for (const SoftwareItem& item : m_searchResults) {
        QListWidgetItem* listItem = new QListWidgetItem(item.getName(), m_resultListWidget);
        listItem->setData(Qt::UserRole, item.getId());
        listItem->setToolTip(QString("分类: %1\n路径: %2").arg(item.getCategory(), item.getFilePath()));
    }
    
    // 更新启动按钮状态
    m_launchButton->setEnabled(m_resultListWidget->count() > 0);
}
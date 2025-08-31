#include "CategoryManager.hpp"
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QRegularExpression>
#include "../utils/Logging.hpp"

CategoryManager::CategoryManager(QObject* parent)
    : QObject(parent)
{
    // 添加默认分类
    m_categories << "所有软件" << "未分类";
    
    // 加载保存的分类
    loadCategories();
}

QStringList CategoryManager::getCategories() const
{
    QMutexLocker locker(&m_mutex);
    return m_categories;
}

bool CategoryManager::addCategory(const QString& name)
{
    QMutexLocker locker(&m_mutex);
    
    if (!validateCategoryName(name)) {
        qCWarning(softwareManager) << "分类名称无效:" << name;
        return false;
    }
    
    if (categoryExists(name)) {
        qCWarning(softwareManager) << "分类已存在:" << name;
        return false;
    }
    
    m_categories.append(name);
    m_categorySoftwareCount[name] = 0;
    
    saveCategories();
    
    emit categoryAdded(name);
    
    qCInfo(softwareManager) << "成功添加分类:" << name;
    return true;
}

bool CategoryManager::renameCategory(const QString& oldName, const QString& newName)
{
    QMutexLocker locker(&m_mutex);
    
    if (!categoryExists(oldName)) {
        qCWarning(softwareManager) << "分类不存在:" << oldName;
        return false;
    }
    
    if (!validateCategoryName(newName)) {
        qCWarning(softwareManager) << "新分类名称无效:" << newName;
        return false;
    }
    
    if (categoryExists(newName)) {
        qCWarning(softwareManager) << "新分类名称已存在:" << newName;
        return false;
    }
    
    // 更新分类列表
    int index = m_categories.indexOf(oldName);
    if (index != -1) {
        m_categories[index] = newName;
    }
    
    // 更新软件计数映射
    if (m_categorySoftwareCount.contains(oldName)) {
        int count = m_categorySoftwareCount[oldName];
        m_categorySoftwareCount.remove(oldName);
        m_categorySoftwareCount[newName] = count;
    }
    
    saveCategories();
    
    emit categoryRenamed(oldName, newName);
    
    qCInfo(softwareManager) << "成功重命名分类:" << oldName << "->" << newName;
    return true;
}

bool CategoryManager::removeCategory(const QString& name)
{
    QMutexLocker locker(&m_mutex);
    
    if (!categoryExists(name)) {
        qCWarning(softwareManager) << "分类不存在:" << name;
        return false;
    }
    
    // 不允许删除内置分类
    if (isBuiltInCategory(name)) {
        qCWarning(softwareManager) << "不允许删除内置分类:" << name;
        return false;
    }
    
    m_categories.removeAll(name);
    m_categorySoftwareCount.remove(name);
    
    saveCategories();
    
    emit categoryRemoved(name);
    
    qCInfo(softwareManager) << "成功删除分类:" << name;
    return true;
}

bool CategoryManager::categoryExists(const QString& name) const
{
    QMutexLocker locker(&m_mutex);
    return m_categories.contains(name);
}

bool CategoryManager::validateCategoryName(const QString& name) const
{
    QMutexLocker locker(&m_mutex);
    
    // 检查名称是否为空
    if (name.trimmed().isEmpty()) {
        return false;
    }
    
    // 检查名称长度
    if (name.length() > 50) {
        return false;
    }
    
    // 检查是否包含非法字符
    QRegularExpression invalidChars(QStringLiteral("[<>:\"/\\\\|?*]"));
    if (invalidChars.match(name).hasMatch()) {
        return false;
    }
    
    return true;
}

int CategoryManager::getCategoryCount(const QString& category) const
{
    QMutexLocker locker(&m_mutex);
    
    if (m_categorySoftwareCount.contains(category)) {
        return m_categorySoftwareCount[category];
    }
    
    return 0;
}

bool CategoryManager::moveSoftwareToCategory(const QString& softwareId, const QString& newCategory)
{
    QMutexLocker locker(&m_mutex);
    
    // 检查分类是否存在
    if (!categoryExists(newCategory)) {
        qCWarning(softwareManager) << "目标分类不存在:" << newCategory;
        return false;
    }
    
    // 更新软件计数（实际实现中需要从数据库获取当前分类）
    // 这里简化处理，只更新目标分类计数
    m_categorySoftwareCount[newCategory] = m_categorySoftwareCount[newCategory] + 1;
    
    emit softwareCategoryChanged(softwareId, newCategory);
    
    qCInfo(softwareManager) << "成功移动软件到分类:" << softwareId << "->" << newCategory;
    return true;
}

QString CategoryManager::getDefaultCategory() const
{
    return "未分类";
}

void CategoryManager::loadCategories()
{
    QSettings settings;
    QStringList savedCategories = settings.value("categories").toStringList();
    
    // 合并保存的分类和默认分类
    for (const QString& category : savedCategories) {
        if (!m_categories.contains(category) && !isBuiltInCategory(category)) {
            m_categories.append(category);
        }
    }
    
    qCInfo(softwareManager) << "成功加载分类列表，共" << m_categories.size() << "个分类";
}

void CategoryManager::saveCategories()
{
    QSettings settings;
    
    // 只保存非内置分类
    QStringList userCategories;
    for (const QString& category : m_categories) {
        if (!isBuiltInCategory(category)) {
            userCategories.append(category);
        }
    }
    
    settings.setValue("categories", userCategories);
    
    qCInfo(softwareManager) << "成功保存分类列表";
}

bool CategoryManager::isBuiltInCategory(const QString& name) const
{
    return name == "所有软件" || name == "未分类";
}
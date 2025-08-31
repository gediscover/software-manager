#ifndef CATEGORYMANAGER_H
#define CATEGORYMANAGER_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QMutex>

class CategoryManager : public QObject {
    Q_OBJECT

public:
    explicit CategoryManager(QObject* parent = nullptr);
    
    // 分类管理方法
    QStringList getCategories() const;
    bool addCategory(const QString& name);
    bool renameCategory(const QString& oldName, const QString& newName);
    bool removeCategory(const QString& name);
    bool categoryExists(const QString& name) const;
    bool validateCategoryName(const QString& name) const;
    
    // 软件分类关联方法
    int getCategoryCount(const QString& category) const;
    bool moveSoftwareToCategory(const QString& softwareId, const QString& newCategory);
    
    // 默认分类
    QString getDefaultCategory() const;
    
signals:
    // 信号定义
    void categoryAdded(const QString& name);
    void categoryRemoved(const QString& name);
    void categoryRenamed(const QString& oldName, const QString& newName);
    void softwareCategoryChanged(const QString& softwareId, const QString& newCategory);
    
private:
    QStringList m_categories;
    QMap<QString, int> m_categorySoftwareCount;
    mutable QMutex m_mutex;
    
    // 私有方法
    void loadCategories();
    void saveCategories();
    bool isBuiltInCategory(const QString& name) const;
};

#endif // CATEGORYMANAGER_H
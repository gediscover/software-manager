#ifndef SIDEBARWIDGET_H
#define SIDEBARWIDGET_H

#include <QWidget>
#include <QStringList>

class QListWidget;
class QPushButton;

class SidebarWidget : public QWidget {
    Q_OBJECT

public:
    explicit SidebarWidget(QWidget* parent = nullptr);
    
    // 分类管理方法
    void addCategory(const QString& name);
    void removeCategory(const QString& name);
    void updateCategoryList();
    QStringList getCategories() const;
    
signals:
    void categorySelected(const QString& category);
    void categoryAdded(const QString& name);
    void categoryRemoved(const QString& name);
    
private slots:
    void onCategoryItemClicked();
    void onAddCategoryClicked();
    void onRemoveCategoryClicked();
    void onCategoryItemDoubleClicked();
    
private:
    void setupUI();
    void loadCategories();
    
    QListWidget* m_categoryList;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;
};

#endif // SIDEBARWIDGET_H
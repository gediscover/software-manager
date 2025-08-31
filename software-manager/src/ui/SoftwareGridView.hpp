#ifndef SOFTWAREGRIDVIEW_H
#define SOFTWAREGRIDVIEW_H

#include <QWidget>
#include <QList>
#include <QMap>
#include "../model/SoftwareItem.hpp"

class QGridLayout;
class QScrollArea;

class SoftwareGridView : public QWidget {
    Q_OBJECT

public:
    explicit SoftwareGridView(QWidget* parent = nullptr);
    
    // 软件项管理方法
    void addSoftwareItem(const SoftwareItem& item);
    void removeSoftwareItem(const QString& id);
    void updateSoftwareItem(const SoftwareItem& item);
    void clearAllItems();
    void setSoftwareItems(const QList<SoftwareItem>& items);
    
    // 视图控制方法
    void setIconSize(int size);
    int iconSize() const;
    
private:
    void setupUI();
    void updateLayout();
    
    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
    QGridLayout* m_gridLayout;
    
    QList<SoftwareItem> m_softwareItems;
    QMap<QString, QWidget*> m_softwareWidgets;
    
    int m_iconSize;
    int m_columns;
};

#endif // SOFTWAREGRIDVIEW_H
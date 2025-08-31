#ifndef SOFTWARELISTVIEW_H
#define SOFTWARELISTVIEW_H

#include <QWidget>
#include <QList>
#include <QMap>
#include <QVBoxLayout>
#include "../model/SoftwareItem.hpp"

class QTableWidget;

class SoftwareListView : public QWidget {
    Q_OBJECT

public:
    explicit SoftwareListView(QWidget* parent = nullptr);
    
    // 软件项管理方法
    void addSoftwareItem(const SoftwareItem& item);
    void removeSoftwareItem(const QString& id);
    void updateSoftwareItem(const SoftwareItem& item);
    void clearAllItems();
    void setSoftwareItems(const QList<SoftwareItem>& items);
    
    // 视图控制方法
    void setColumnWidth(int column, int width);
    void setSortColumn(int column, Qt::SortOrder order = Qt::AscendingOrder);
    
private:
    void setupUI();
    void updateTable();
    
    QTableWidget* m_tableWidget;
    QList<SoftwareItem> m_softwareItems;
    QMap<QString, int> m_softwareRowMap;
};

#endif // SOFTWARELISTVIEW_H
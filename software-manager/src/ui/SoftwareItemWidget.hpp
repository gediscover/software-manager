#ifndef SOFTWAREITEMWIDGET_H
#define SOFTWAREITEMWIDGET_H

#include <QWidget>
#include <QIcon>
#include "../model/SoftwareItem.hpp"

class QLabel;
class QVBoxLayout;
class QMenu;

class SoftwareItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit SoftwareItemWidget(const SoftwareItem& item, QWidget* parent = nullptr);
    
    // 属性设置方法
    void setIconSize(const QSize& size);
    QSize iconSize() const;
    
    // 获取关联的软件项ID
    QString softwareId() const;
    
    // 获取关联的软件项
    SoftwareItem softwareItem() const;
    
signals:
    void clicked();
    void doubleClicked();
    void rightClicked(const QPoint& pos);
    void launchRequested(const QString& softwareId);
    void removeRequested(const QString& softwareId);
    void propertiesRequested(const QString& softwareId);
    
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    
private slots:
    void onLaunchAction();
    void onOpenLocationAction();
    void onRemoveAction();
    void onPropertiesAction();
    
private:
    void setupUI();
    void updateDisplay();
    void createContextMenu();
    
    SoftwareItem m_softwareItem;
    QLabel* m_iconLabel;
    QLabel* m_nameLabel;
    QVBoxLayout* m_layout;
    
    QSize m_iconSize;
    
    // 右键菜单
    QMenu* m_contextMenu;
};

#endif // SOFTWAREITEMWIDGET_H
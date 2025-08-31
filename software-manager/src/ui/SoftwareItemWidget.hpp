#ifndef SOFTWAREITEMWIDGET_H
#define SOFTWAREITEMWIDGET_H

#include <QWidget>
#include <QIcon>

class QLabel;
class QVBoxLayout;
class SoftwareItem;

class SoftwareItemWidget : public QWidget {
    Q_OBJECT

public:
    explicit SoftwareItemWidget(const SoftwareItem& item, QWidget* parent = nullptr);
    
    // 属性设置方法
    void setIconSize(const QSize& size);
    QSize iconSize() const;
    
    // 获取关联的软件项ID
    QString softwareId() const;
    
signals:
    void clicked();
    void doubleClicked();
    void rightClicked(const QPoint& pos);
    
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    
private:
    void setupUI();
    void updateDisplay();
    
    SoftwareItem m_softwareItem;
    QLabel* m_iconLabel;
    QLabel* m_nameLabel;
    QVBoxLayout* m_layout;
    
    QSize m_iconSize;
};

#endif // SOFTWAREITEMWIDGET_H
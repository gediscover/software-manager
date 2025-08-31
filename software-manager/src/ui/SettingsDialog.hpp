#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QList>

class QCheckBox;
class QSpinBox;
class QLineEdit;
class QPushButton;
class QListWidget;
class QComboBox;

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    
private slots:
    void onAddPathClicked();
    void onRemovePathClicked();
    void onSaveClicked();
    void onCancelClicked();
    void onRestoreDefaultsClicked();
    
private:
    void setupUI();
    void loadSettings();
    void saveSettings();
    
    // 扫描设置
    QListWidget* m_scanPathsList;
    QPushButton* m_addPathButton;
    QPushButton* m_removePathButton;
    QCheckBox* m_autoScanCheckBox;
    
    // 视图设置
    QComboBox* m_viewModeComboBox;
    QSpinBox* m_iconSizeSpinBox;
    
    // 系统托盘设置
    QCheckBox* m_minimizeToTrayCheckBox;
    QCheckBox* m_closeToTrayCheckBox;
    
    // 按钮
    QPushButton* m_saveButton;
    QPushButton* m_cancelButton;
    QPushButton* m_restoreDefaultsButton;
    
    // 临时存储设置值
    QStringList m_scanPaths;
    bool m_autoScanEnabled;
    QString m_viewMode;
    int m_iconSize;
    bool m_minimizeToTray;
    bool m_closeToTray;
};

#endif // SETTINGSDIALOG_H
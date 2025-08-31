#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QList>

class QLineEdit;
class QListWidget;
class QPushButton;
class SoftwareItem;

class SearchDialog : public QDialog {
    Q_OBJECT

public:
    explicit SearchDialog(QWidget* parent = nullptr);
    
    // 搜索方法
    void setSearchKeyword(const QString& keyword);
    QString searchKeyword() const;
    
    // 搜索结果管理
    void setSearchResults(const QList<SoftwareItem>& results);
    QList<SoftwareItem> searchResults() const;
    
private slots:
    void onSearchTextChanged();
    void onSearchButtonClicked();
    void onLaunchButtonClicked();
    void onListItemDoubleClicked();
    
private:
    void setupUI();
    void performSearch();
    void updateSearchResults();
    
    QLineEdit* m_searchLineEdit;
    QListWidget* m_resultListWidget;
    QPushButton* m_searchButton;
    QPushButton* m_launchButton;
    QPushButton* m_closeButton;
    
    QList<SoftwareItem> m_searchResults;
};

#endif // SEARCHDIALOG_H
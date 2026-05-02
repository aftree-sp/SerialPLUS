#pragma once
#include <QMainWindow>
#include <QTabWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QAction>
#include <QList>

class SerialTab;
class ThemeManager;
class StatusBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void addNewTab();
    void closeTab(int index);
    void onTabChanged(int index);
    void toggleTheme();
    void showAbout();
    void showSettings();

private:
    void setupUi();
    void setupToolBar();
    void setupMenuBar();
    void setupStatusBar();
    void saveSettings();
    void loadSettings();

    QTabWidget   *m_tabWidget    = nullptr;
    QToolBar     *m_toolBar      = nullptr;
    StatusBar    *m_statusBar    = nullptr;

    QAction *m_actNewTab    = nullptr;
    QAction *m_actTheme     = nullptr;
    QAction *m_actAbout     = nullptr;
    QAction *m_actSettings  = nullptr;

    int m_tabCounter = 0;
};

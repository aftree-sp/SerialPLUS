#include "MainWindow.h"
#include "SerialTab.h"
#include "ThemeManager.h"
#include "widgets/StatusBar.h"
#include "widgets/MaterialButton.h"
#include <QTabBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QKeyEvent>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("SerialPLUS");
    setMinimumSize(1100, 700);
    resize(1280, 800);
    setupUi();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    loadSettings();
    addNewTab();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUi()
{
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setMovable(true);
    m_tabWidget->setDocumentMode(true);
    m_tabWidget->setObjectName("mainTabWidget");

    // "+" button to add tab
    auto *addBtn = new MaterialButton("+", m_tabWidget);
    addBtn->setFixedSize(32, 28);
    addBtn->setObjectName("addTabBtn");
    addBtn->setToolTip("New Serial Port Tab");
    m_tabWidget->setCornerWidget(addBtn, Qt::TopRightCorner);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addNewTab);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    setCentralWidget(m_tabWidget);
}

void MainWindow::setupMenuBar()
{
    auto *fileMenu = menuBar()->addMenu("&文件");
    fileMenu->addAction("新建标签页", this, &MainWindow::addNewTab, QKeySequence("Ctrl+T"));
    fileMenu->addSeparator();
    fileMenu->addAction("退出", qApp, &QApplication::quit, QKeySequence::Quit);

    auto *viewMenu = menuBar()->addMenu("&视图");
    m_actTheme = viewMenu->addAction("切换主题", this, &MainWindow::toggleTheme);

    auto *helpMenu = menuBar()->addMenu("&帮助");
    helpMenu->addAction("关于", this, &MainWindow::showAbout);
}

void MainWindow::setupToolBar()
{
    m_toolBar = addToolBar("Main");
    m_toolBar->setObjectName("mainToolBar");
    m_toolBar->setMovable(false);
    m_toolBar->setIconSize(QSize(20, 20));
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    m_actNewTab = m_toolBar->addAction("＋  新建标签页");
    m_actNewTab->setToolTip("打开新的串口标签页 (Ctrl+T)");
    connect(m_actNewTab, &QAction::triggered, this, &MainWindow::addNewTab);

    m_toolBar->addSeparator();

    m_actTheme = m_toolBar->addAction("☀  浅色模式");
    connect(m_actTheme, &QAction::triggered, this, &MainWindow::toggleTheme);

    m_toolBar->addSeparator();

    m_actAbout = m_toolBar->addAction("ℹ  关于");
    connect(m_actAbout, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::setupStatusBar()
{
    m_statusBar = new StatusBar(this);
    setStatusBar(m_statusBar);
}

void MainWindow::addNewTab()
{
    m_tabCounter++;
    auto *tab = new SerialTab(this);
    int idx = m_tabWidget->addTab(tab, QString("COM%1").arg(m_tabCounter));
    m_tabWidget->setCurrentIndex(idx);

    connect(tab, &SerialTab::portNameChanged, this, [this, tab](const QString &name) {
        int i = m_tabWidget->indexOf(tab);
        if (i >= 0) m_tabWidget->setTabText(i, name.isEmpty() ? "---" : name);
    });
    connect(tab, &SerialTab::connectionChanged, this, [this, tab](bool connected) {
        int i = m_tabWidget->indexOf(tab);
        if (i >= 0) {
            QString text = m_tabWidget->tabText(i);
            // Add dot indicator
            if (text.endsWith(" ●")) text.chop(2);
            if (connected) text += " ●";
            m_tabWidget->setTabText(i, text);
        }
        if (m_statusBar) {
            m_statusBar->setConnected(connected);
        }
    });
    connect(tab, &SerialTab::statusMessage, this, [this](const QString &msg) {
        statusBar()->showMessage(msg, 3000);
    });
}

void MainWindow::closeTab(int index)
{
    auto *tab = qobject_cast<SerialTab*>(m_tabWidget->widget(index));
    if (tab) {
        tab->disconnectPort();
        m_tabWidget->removeTab(index);
        tab->deleteLater();
    }
    // 如果所有标签页都关闭了，自动新建一个
    if (m_tabWidget->count() == 0) {
        addNewTab();
    }
}

void MainWindow::onTabChanged(int index)
{
    Q_UNUSED(index)
}

void MainWindow::toggleTheme()
{
    auto &tm = ThemeManager::instance();
    if (tm.currentTheme() == ThemeManager::Dark) {
        tm.applyTheme(ThemeManager::Light);
        m_actTheme->setText("🌙  深色模式");
    } else {
        tm.applyTheme(ThemeManager::Dark);
        m_actTheme->setText("☀  浅色模式");
    }
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "关于 SerialPLUS",
        "<h2>SerialPLUS v1.0</h2>"
        "<p>基于 Qt6 的强大串口终端工具。</p>"
        "<p>功能：多标签页、HEX/文本收发、自动发送、宏命令、<br>"
        "Modbus/自定义协议解析、脚本支持、数据可视化。</p>"
        "<p>基于 C++ Qt6 构建</p>");
}

void MainWindow::showSettings()
{
    // TODO: Global settings dialog
}

void MainWindow::saveSettings()
{
    QSettings s("SerialPLUS", "SerialPLUS");
    s.setValue("geometry", saveGeometry());
    s.setValue("windowState", saveState());
}

void MainWindow::loadSettings()
{
    QSettings s("SerialPLUS", "SerialPLUS");
    restoreGeometry(s.value("geometry").toByteArray());
    restoreState(s.value("windowState").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_T) {
        addNewTab();
    } else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_W) {
        closeTab(m_tabWidget->currentIndex());
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

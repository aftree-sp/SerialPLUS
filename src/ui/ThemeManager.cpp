#include "ThemeManager.h"
#include <QApplication>

ThemeManager &ThemeManager::instance()
{
    static ThemeManager inst;
    return inst;
}

void ThemeManager::applyTheme(Theme t)
{
    m_theme = t;
    QString qss = (t == Dark) ? darkStyleSheet() : lightStyleSheet();
    qApp->setStyleSheet(qss);
    emit themeChanged(t);
}

QString ThemeManager::darkStyleSheet() const
{
    return R"(
/* ===== Material Design Dark Theme ===== */
/* Base Colors:
   Background:  #1E1E2E (deep dark)
   Surface:     #2A2A3E
   Card:        #313145
   Primary:     #7C4DFF (deep purple)
   Secondary:   #03DAC6 (teal)
   Accent:      #FF6B6B
   Text:        #E0E0E0
   Hint:        #888888
*/

QMainWindow, QDialog {
    background-color: #1E1E2E;
    color: #E0E0E0;
}

QWidget {
    background-color: #1E1E2E;
    color: #E0E0E0;
    font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
    font-size: 10pt;
}

/* ---- Menu Bar ---- */
QMenuBar {
    background-color: #2A2A3E;
    color: #E0E0E0;
    border-bottom: 1px solid #3A3A5E;
    padding: 2px 0;
}
QMenuBar::item {
    padding: 4px 12px;
    border-radius: 4px;
}
QMenuBar::item:selected {
    background-color: #3A2A6E;
    color: #BB86FC;
}
QMenu {
    background-color: #2A2A3E;
    color: #E0E0E0;
    border: 1px solid #3A3A5E;
    border-radius: 6px;
    padding: 4px;
}
QMenu::item {
    padding: 6px 24px 6px 12px;
    border-radius: 4px;
}
QMenu::item:selected {
    background-color: #3A2A6E;
    color: #BB86FC;
}
QMenu::separator {
    height: 1px;
    background: #3A3A5E;
    margin: 4px 8px;
}

/* ---- Toolbar ---- */
QToolBar {
    background-color: #2A2A3E;
    border-bottom: 1px solid #3A3A5E;
    spacing: 4px;
    padding: 4px 8px;
}
QToolBar QToolButton {
    background-color: transparent;
    color: #B0B0D0;
    border-radius: 6px;
    padding: 4px 10px;
}
QToolBar QToolButton:hover {
    background-color: #3A2A6E;
    color: #BB86FC;
}
QToolBar QToolButton:pressed {
    background-color: #4A3A8E;
}
QToolBar::separator {
    width: 1px;
    background: #3A3A5E;
    margin: 4px 4px;
}

/* ---- TabWidget ---- */
QTabWidget::pane {
    background-color: #1E1E2E;
    border: none;
    border-top: 2px solid #7C4DFF;
}
QTabBar::tab {
    background-color: #2A2A3E;
    color: #888;
    padding: 8px 16px;
    border-bottom: 2px solid transparent;
    min-width: 80px;
    font-size: 9.5pt;
}
QTabBar::tab:selected {
    background-color: #1E1E2E;
    color: #BB86FC;
    border-bottom: 2px solid #7C4DFF;
    font-weight: 600;
}
QTabBar::tab:hover:!selected {
    background-color: #252535;
    color: #C0C0E0;
}
QTabBar::close-button {
    subcontrol-position: right;
    padding-left: 4px;
}

/* Main tab widget (outer) */
QTabWidget#mainTabWidget QTabBar::tab {
    min-width: 100px;
    padding: 9px 18px;
}

/* Side tabs */
QTabWidget#sideTabs::pane {
    border: none;
    border-top: 1px solid #3A3A5E;
}
QTabWidget#sideTabs QTabBar::tab {
    padding: 6px 10px;
    min-width: 60px;
    font-size: 9pt;
}

/* ---- GroupBox ---- */
QGroupBox {
    border: 1px solid #3A3A5E;
    border-radius: 8px;
    margin-top: 12px;
    padding: 8px 4px 4px 4px;
    color: #BB86FC;
    font-weight: 600;
    font-size: 9.5pt;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    padding: 0 6px;
    left: 10px;
    top: -2px;
}

/* ---- Port Config Widget ---- */
QWidget#portConfigWidget {
    background-color: #252538;
    border-bottom: 1px solid #3A3A5E;
}
QLabel#cfgLabel {
    color: #888;
    font-size: 9pt;
}
QComboBox#cfgCombo {
    background-color: #2A2A3E;
    color: #E0E0E0;
    border: 1px solid #3A3A5E;
    border-radius: 4px;
    padding: 3px 8px;
    min-height: 24px;
}
QComboBox#cfgCombo:hover {
    border: 1px solid #7C4DFF;
}
QComboBox#cfgCombo:focus {
    border: 2px solid #7C4DFF;
}
QComboBox#cfgCombo::drop-down {
    border: none;
    width: 20px;
}
QComboBox#cfgCombo QAbstractItemView {
    background-color: #2A2A3E;
    color: #E0E0E0;
    border: 1px solid #7C4DFF;
    selection-background-color: #3A2A6E;
}

/* Status dot */
QLabel#statusDot {
    color: #444;
    font-size: 16px;
}
QLabel#statusDot[connected="true"] {
    color: #4CAF50;
}

/* Connect button */
QPushButton#connectBtn {
    background-color: #7C4DFF;
    color: #FFFFFF;
    border: none;
    border-radius: 6px;
    padding: 6px 18px;
    font-weight: 600;
    font-size: 9.5pt;
}
QPushButton#connectBtn:hover {
    background-color: #9965FF;
}
QPushButton#connectBtn:pressed {
    background-color: #6030DD;
}

/* Refresh button */
QPushButton#refreshBtn {
    background-color: #2A2A3E;
    color: #03DAC6;
    border: 1px solid #3A3A5E;
    border-radius: 4px;
    font-size: 14px;
    font-weight: bold;
}
QPushButton#refreshBtn:hover {
    background-color: #1A3A3E;
    border-color: #03DAC6;
}

/* ---- Terminal ---- */
QWidget#termToolbar, QWidget#inputRow {
    background-color: #252538;
}

QPlainTextEdit#termDisplay {
    background-color: #0D0D1A;
    color: #E0E0E0;
    border: none;
    font-family: "Consolas", "Courier New", monospace;
    font-size: 10pt;
    selection-background-color: #3A2A6E;
}

QLineEdit#termInput {
    background-color: #2A2A3E;
    color: #E0E0E0;
    border: 1px solid #3A3A5E;
    border-radius: 6px;
    padding: 6px 10px;
    font-family: "Consolas", monospace;
}
QLineEdit#termInput:focus {
    border: 2px solid #7C4DFF;
}

QPushButton#sendBtn {
    background-color: #03DAC6;
    color: #000000;
    border: none;
    border-radius: 6px;
    padding: 6px 16px;
    font-weight: 700;
}
QPushButton#sendBtn:hover {
    background-color: #04F5DA;
}
QPushButton#sendBtn:pressed {
    background-color: #02AFA0;
}

QPushButton#clearBtn {
    background-color: transparent;
    color: #888;
    border: 1px solid #3A3A5E;
    border-radius: 6px;
    padding: 4px 12px;
}
QPushButton#clearBtn:hover {
    color: #E0E0E0;
    border-color: #888;
}

/* RX/TX labels */
QLabel#rxLabel {
    color: #4FC3F7;
    font-size: 9pt;
    padding: 0 6px;
}
QLabel#txLabel {
    color: #FFB74D;
    font-size: 9pt;
    padding: 0 6px;
}

/* ---- Separators ---- */
QFrame#separator {
    border: none;
    background-color: #2D2D44;
    max-height: 1px;
    min-height: 1px;
}
QFrame#cfgSep {
    border: none;
    background-color: #3A3A5E;
    max-width: 1px;
    min-width: 1px;
}

/* ---- Scrollbars ---- */
QScrollBar:vertical {
    background-color: #1A1A2E;
    width: 8px;
    border-radius: 4px;
}
QScrollBar::handle:vertical {
    background-color: #3A3A6E;
    border-radius: 4px;
    min-height: 20px;
}
QScrollBar::handle:vertical:hover {
    background-color: #7C4DFF;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
QScrollBar:horizontal {
    background-color: #1A1A2E;
    height: 8px;
    border-radius: 4px;
}
QScrollBar::handle:horizontal {
    background-color: #3A3A6E;
    border-radius: 4px;
    min-width: 20px;
}
QScrollBar::handle:horizontal:hover { background-color: #7C4DFF; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }

/* ---- CheckBox ---- */
QCheckBox {
    color: #C0C0D0;
    spacing: 6px;
}
QCheckBox::indicator {
    width: 16px; height: 16px;
    border: 2px solid #555;
    border-radius: 3px;
    background-color: transparent;
}
QCheckBox::indicator:checked {
    background-color: #7C4DFF;
    border-color: #7C4DFF;
}
QCheckBox::indicator:hover {
    border-color: #7C4DFF;
}

/* ---- SpinBox ---- */
QSpinBox {
    background-color: #2A2A3E;
    color: #E0E0E0;
    border: 1px solid #3A3A5E;
    border-radius: 4px;
    padding: 3px 6px;
}
QSpinBox:focus { border: 2px solid #7C4DFF; }
QSpinBox::up-button, QSpinBox::down-button {
    background-color: #3A3A5E;
    border: none;
    width: 16px;
}
QSpinBox::up-button:hover, QSpinBox::down-button:hover {
    background-color: #7C4DFF;
}

/* ---- LineEdit ---- */
QLineEdit {
    background-color: #2A2A3E;
    color: #E0E0E0;
    border: 1px solid #3A3A5E;
    border-radius: 4px;
    padding: 4px 8px;
}
QLineEdit:focus { border: 2px solid #7C4DFF; }
QLineEdit[readOnly="true"] {
    background-color: #252535;
    color: #888;
}

/* ---- List/Table ---- */
QListWidget {
    background-color: #1A1A2E;
    color: #E0E0E0;
    border: 1px solid #3A3A5E;
    border-radius: 6px;
}
QListWidget::item {
    padding: 6px 10px;
    border-radius: 4px;
}
QListWidget::item:selected {
    background-color: #3A2A6E;
    color: #BB86FC;
}
QListWidget::item:hover:!selected {
    background-color: #252545;
}

QTableWidget {
    background-color: #1A1A2E;
    color: #E0E0E0;
    border: 1px solid #3A3A5E;
    border-radius: 6px;
    gridline-color: #2A2A4E;
    alternate-background-color: #1E1E35;
}
QTableWidget::item:selected {
    background-color: #3A2A6E;
    color: #BB86FC;
}
QHeaderView::section {
    background-color: #252545;
    color: #BB86FC;
    padding: 6px 8px;
    border: none;
    border-right: 1px solid #3A3A5E;
    font-weight: 600;
    font-size: 9pt;
}

/* ---- Macro buttons ---- */
QPushButton#macroBtn {
    background-color: #2A2A3E;
    color: #C0C0D0;
    border: 1px solid #3A3A5E;
    border-radius: 6px;
    padding: 5px 12px;
}
QPushButton#macroBtn:hover {
    background-color: #3A2A6E;
    color: #BB86FC;
    border-color: #7C4DFF;
}
QPushButton#macroBtn:pressed {
    background-color: #4A3A8E;
}

/* Auto toggle */
QPushButton#autoToggleBtn {
    background-color: #1A3A2E;
    color: #4CAF50;
    border: 1px solid #4CAF50;
    border-radius: 6px;
    padding: 5px 14px;
    font-weight: 600;
}
QPushButton#autoToggleBtn:checked {
    background-color: #3A1A1A;
    color: #EF5350;
    border-color: #EF5350;
}
QPushButton#autoToggleBtn:hover {
    opacity: 0.9;
}

/* ---- Script Editor ---- */
QPlainTextEdit#scriptEditor {
    background-color: #0D1117;
    color: #E0E0E0;
    border: 1px solid #3A3A5E;
    border-radius: 6px;
    font-family: "Consolas", monospace;
    selection-background-color: #3A2A6E;
}
QPlainTextEdit#scriptOutput {
    background-color: #0D0D1A;
    color: #E0E0E0;
    border: 1px solid #3A3A5E;
    border-radius: 6px;
    font-family: "Consolas", monospace;
}

/* ---- Status Bar ---- */
QStatusBar {
    background-color: #252538;
    color: #888;
    border-top: 1px solid #3A3A5E;
    font-size: 9pt;
}
QStatusBar QLabel {
    background-color: transparent;
    padding: 2px 8px;
}

/* ---- Splitters ---- */
QSplitter::handle {
    background-color: #2D2D44;
}
QSplitter::handle:hover {
    background-color: #7C4DFF;
}
QSplitter#hSplitter::handle {
    width: 3px;
}

/* ---- Log status ---- */
QLabel#logStatus {
    color: #888;
    font-size: 9pt;
    padding: 2px 4px;
}

/* Add tab button */
QPushButton#addTabBtn {
    background-color: transparent;
    color: #7C4DFF;
    border: none;
    font-size: 18px;
    font-weight: bold;
    border-radius: 4px;
}
QPushButton#addTabBtn:hover {
    background-color: #3A2A6E;
}

/* Chart view */
QChartView#chartView {
    border: 1px solid #3A3A5E;
    border-radius: 8px;
}

/* Hex input */
QLineEdit#hexInput {
    font-family: "Consolas", monospace;
    letter-spacing: 1px;
}
QLineEdit#hexPreview {
    font-family: "Consolas", monospace;
    background-color: #0D1117;
    color: #03DAC6;
}

/* Script status */
QLabel#scriptStatus {
    color: #888;
    font-size: 9pt;
}
)";
}

QString ThemeManager::lightStyleSheet() const
{
    return R"(
/* ===== Material Design Light Theme ===== */
QMainWindow, QDialog, QWidget {
    background-color: #F5F5F5;
    color: #212121;
    font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
    font-size: 10pt;
}
QMenuBar {
    background-color: #FFFFFF;
    color: #212121;
    border-bottom: 1px solid #E0E0E0;
}
QMenuBar::item:selected { background-color: #EDE7F6; color: #6200EE; }
QMenu {
    background-color: #FFFFFF;
    color: #212121;
    border: 1px solid #E0E0E0;
    border-radius: 6px;
}
QMenu::item:selected { background-color: #EDE7F6; color: #6200EE; }
QToolBar {
    background-color: #FFFFFF;
    border-bottom: 1px solid #E0E0E0;
    padding: 4px 8px;
}
QToolBar QToolButton { border-radius: 6px; padding: 4px 10px; color: #555; }
QToolBar QToolButton:hover { background-color: #EDE7F6; color: #6200EE; }

QTabWidget::pane { background-color: #F5F5F5; border: none; border-top: 2px solid #6200EE; }
QTabBar::tab { background-color: #EEEEEE; color: #888; padding: 8px 16px; border-bottom: 2px solid transparent; }
QTabBar::tab:selected { background-color: #F5F5F5; color: #6200EE; border-bottom: 2px solid #6200EE; font-weight: 600; }

QGroupBox { border: 1px solid #E0E0E0; border-radius: 8px; margin-top: 12px; padding: 8px; color: #6200EE; font-weight: 600; }
QGroupBox::title { subcontrol-origin: margin; left: 10px; top: -2px; padding: 0 6px; }

QWidget#portConfigWidget { background-color: #FFFFFF; border-bottom: 1px solid #E0E0E0; }

QComboBox#cfgCombo { background-color: #FFFFFF; color: #212121; border: 1px solid #BDBDBD; border-radius: 4px; padding: 3px 8px; }
QComboBox#cfgCombo:hover { border: 1px solid #6200EE; }
QComboBox#cfgCombo QAbstractItemView { background-color: #FFFFFF; color: #212121; selection-background-color: #EDE7F6; }

QLabel#statusDot { color: #BDBDBD; font-size: 16px; }
QLabel#statusDot[connected="true"] { color: #4CAF50; }

QPushButton#connectBtn { background-color: #6200EE; color: #FFFFFF; border: none; border-radius: 6px; padding: 6px 18px; font-weight: 600; }
QPushButton#connectBtn:hover { background-color: #7C4DFF; }

QPlainTextEdit#termDisplay { background-color: #FAFAFA; color: #212121; border: none; font-family: "Consolas", monospace; }
QLineEdit#termInput { background-color: #FFFFFF; color: #212121; border: 1px solid #BDBDBD; border-radius: 6px; padding: 6px 10px; }
QLineEdit#termInput:focus { border: 2px solid #6200EE; }

QPushButton#sendBtn { background-color: #018786; color: #FFFFFF; border: none; border-radius: 6px; padding: 6px 16px; font-weight: 700; }
QPushButton#sendBtn:hover { background-color: #019688; }
QPushButton#clearBtn { background-color: transparent; color: #757575; border: 1px solid #BDBDBD; border-radius: 6px; padding: 4px 12px; }

QLabel#rxLabel { color: #0288D1; font-size: 9pt; padding: 0 6px; }
QLabel#txLabel { color: #F57C00; font-size: 9pt; padding: 0 6px; }

QScrollBar:vertical { background-color: #F5F5F5; width: 8px; }
QScrollBar::handle:vertical { background-color: #BDBDBD; border-radius: 4px; }
QScrollBar::handle:vertical:hover { background-color: #6200EE; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }

QCheckBox { color: #212121; spacing: 6px; }
QCheckBox::indicator { width: 16px; height: 16px; border: 2px solid #BDBDBD; border-radius: 3px; background-color: transparent; }
QCheckBox::indicator:checked { background-color: #6200EE; border-color: #6200EE; }

QSpinBox, QLineEdit { background-color: #FFFFFF; color: #212121; border: 1px solid #BDBDBD; border-radius: 4px; padding: 3px 6px; }
QSpinBox:focus, QLineEdit:focus { border: 2px solid #6200EE; }

QListWidget { background-color: #FFFFFF; color: #212121; border: 1px solid #E0E0E0; border-radius: 6px; }
QListWidget::item:selected { background-color: #EDE7F6; color: #6200EE; }
QTableWidget { background-color: #FFFFFF; color: #212121; border: 1px solid #E0E0E0; gridline-color: #E0E0E0; alternate-background-color: #F9F9F9; }
QHeaderView::section { background-color: #EDE7F6; color: #6200EE; padding: 6px 8px; border: none; border-right: 1px solid #E0E0E0; font-weight: 600; }

QPushButton#macroBtn { background-color: #FFFFFF; color: #555; border: 1px solid #BDBDBD; border-radius: 6px; padding: 5px 12px; }
QPushButton#macroBtn:hover { background-color: #EDE7F6; color: #6200EE; border-color: #6200EE; }

QStatusBar { background-color: #FFFFFF; color: #757575; border-top: 1px solid #E0E0E0; font-size: 9pt; }
QSplitter::handle { background-color: #E0E0E0; }
QSplitter::handle:hover { background-color: #6200EE; }

QPushButton#addTabBtn { background-color: transparent; color: #6200EE; border: none; font-size: 18px; }
QPushButton#addTabBtn:hover { background-color: #EDE7F6; }

QFrame#separator { background-color: #E0E0E0; max-height: 1px; }
QFrame#cfgSep { background-color: #E0E0E0; max-width: 1px; }

QPlainTextEdit#scriptEditor { background-color: #FFFFFF; color: #212121; border: 1px solid #E0E0E0; font-family: "Consolas", monospace; }
QPlainTextEdit#scriptOutput { background-color: #FAFAFA; color: #212121; border: 1px solid #E0E0E0; font-family: "Consolas", monospace; }
QLineEdit#hexPreview { font-family: "Consolas", monospace; background-color: #F5F5F5; color: #018786; }
QWidget#termToolbar, QWidget#inputRow { background-color: #FFFFFF; }
)";
}

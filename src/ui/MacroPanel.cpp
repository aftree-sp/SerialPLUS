#include "MacroPanel.h"
#include "../core/MacroManager.h"
#include "../core/AutoSender.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QPushButton>
#include <QMessageBox>

MacroPanel::MacroPanel(MacroManager *mgr, AutoSender *sender, QWidget *parent)
    : QWidget(parent), m_mgr(mgr), m_sender(sender)
{
    setupUi();
    refreshList();

    connect(m_mgr, &MacroManager::macrosChanged, this, &MacroPanel::refreshList);
    connect(m_sender, &AutoSender::sent, this, [this](int count) {
        m_sentLabel->setText(QString("已发送：%1").arg(count));
    });
    connect(m_sender, &AutoSender::finished, this, [this]() {
        m_autoToggle->setText("▶ 开始");
        m_autoToggle->setChecked(false);
    });
}

void MacroPanel::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    // Macro list
    auto *listGroup = new QGroupBox("快捷发送宏", this);
    auto *listLayout = new QVBoxLayout(listGroup);

    m_listWidget = new QListWidget(this);
    m_listWidget->setObjectName("macroList");
    m_listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listLayout->addWidget(m_listWidget);

    auto *btnRow = new QHBoxLayout();
    m_addBtn    = new QPushButton("＋ 添加",    this);
    m_removeBtn = new QPushButton("－ 删除", this);
    m_editBtn   = new QPushButton("✎ 编辑",   this);
    m_addBtn->setObjectName("macroBtn");
    m_removeBtn->setObjectName("macroBtn");
    m_editBtn->setObjectName("macroBtn");
    btnRow->addWidget(m_addBtn);
    btnRow->addWidget(m_removeBtn);
    btnRow->addWidget(m_editBtn);
    listLayout->addLayout(btnRow);
    layout->addWidget(listGroup);

    connect(m_addBtn,    &QPushButton::clicked, this, &MacroPanel::onAddMacro);
    connect(m_removeBtn, &QPushButton::clicked, this, &MacroPanel::onRemoveMacro);
    connect(m_editBtn,   &QPushButton::clicked, this, &MacroPanel::onEditMacro);
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
        int idx = m_listWidget->row(item);
        onSendMacro(idx);
    });

    // Auto sender
    m_autoGroup = new QGroupBox("自动发送", this);
    auto *autoLayout = new QFormLayout(m_autoGroup);

    m_autoData = new QLineEdit(this);
    m_autoData->setPlaceholderText("自动发送的数据...");
    autoLayout->addRow("数据位：", m_autoData);

    m_autoHex = new QCheckBox("HEX 模式", this);
    autoLayout->addRow("", m_autoHex);

    m_intervalSpin = new QSpinBox(this);
    m_intervalSpin->setRange(10, 99999);
    m_intervalSpin->setValue(1000);
    m_intervalSpin->setSuffix(" 毫秒");
    autoLayout->addRow("间隔：", m_intervalSpin);

    m_repeatSpin = new QSpinBox(this);
    m_repeatSpin->setRange(-1, 99999);
    m_repeatSpin->setValue(-1);
    m_repeatSpin->setSpecialValueText("∞ 无限");
    autoLayout->addRow("重复次数：", m_repeatSpin);

    auto *autoCtrlRow = new QHBoxLayout();
    m_autoToggle = new QPushButton("▶ 开始", this);
    m_autoToggle->setObjectName("autoToggleBtn");
    m_autoToggle->setCheckable(true);
    m_sentLabel = new QLabel("已发送：0", this);
    autoCtrlRow->addWidget(m_autoToggle);
    autoCtrlRow->addWidget(m_sentLabel);
    autoLayout->addRow(autoCtrlRow);

    layout->addWidget(m_autoGroup);
    layout->addStretch();

    connect(m_autoToggle, &QPushButton::toggled, this, &MacroPanel::onAutoSendToggle);
}

void MacroPanel::refreshList()
{
    m_listWidget->clear();
    for (int i = 0; i < m_mgr->count(); i++) {
        const MacroItem &item = m_mgr->macros()[i];
        QString label = QString("[%1] %2").arg(item.isHex ? "HEX" : "TXT").arg(item.name);
        if (!item.shortcut.isEmpty()) label += QString("  [%1]").arg(item.shortcut);
        m_listWidget->addItem(label);
    }
}

void MacroPanel::onAddMacro()
{
    QDialog dlg(this);
    dlg.setWindowTitle("添加宏");
    auto *form = new QFormLayout(&dlg);
    auto *nameEdit = new QLineEdit(&dlg);
    auto *dataEdit = new QLineEdit(&dlg);
    auto *hexCheck = new QCheckBox("HEX 模式", &dlg);
    auto *shortcutEdit = new QLineEdit(&dlg);
    form->addRow("名称：", nameEdit);
    form->addRow("数据位：", dataEdit);
    form->addRow("", hexCheck);
    form->addRow("快捷键：", shortcutEdit);
    auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    if (dlg.exec() == QDialog::Accepted) {
        MacroItem item;
        item.name = nameEdit->text();
        item.data = dataEdit->text();
        item.isHex = hexCheck->isChecked();
        item.shortcut = shortcutEdit->text();
        if (!item.name.isEmpty())
            m_mgr->addMacro(item);
    }
}

void MacroPanel::onRemoveMacro()
{
    int idx = m_listWidget->currentRow();
    if (idx < 0) return;
    m_mgr->removeMacro(idx);
}

void MacroPanel::onEditMacro()
{
    int idx = m_listWidget->currentRow();
    if (idx < 0 || idx >= m_mgr->count()) return;
    const MacroItem &cur = m_mgr->macros()[idx];

    QDialog dlg(this);
    dlg.setWindowTitle("编辑宏");
    auto *form = new QFormLayout(&dlg);
    auto *nameEdit = new QLineEdit(cur.name, &dlg);
    auto *dataEdit = new QLineEdit(cur.data, &dlg);
    auto *hexCheck = new QCheckBox("HEX 模式", &dlg);
    hexCheck->setChecked(cur.isHex);
    auto *shortcutEdit = new QLineEdit(cur.shortcut, &dlg);
    form->addRow("名称：", nameEdit);
    form->addRow("数据位：", dataEdit);
    form->addRow("", hexCheck);
    form->addRow("快捷键：", shortcutEdit);
    auto *btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    form->addRow(btns);
    connect(btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    if (dlg.exec() == QDialog::Accepted) {
        MacroItem item;
        item.name = nameEdit->text();
        item.data = dataEdit->text();
        item.isHex = hexCheck->isChecked();
        item.shortcut = shortcutEdit->text();
        m_mgr->updateMacro(idx, item);
    }
}

void MacroPanel::onSendMacro(int index)
{
    QByteArray data = m_mgr->getMacroData(index);
    if (!data.isEmpty()) emit sendRequested(data);
}

void MacroPanel::onAutoSendToggle(bool on)
{
    if (on) {
        QString txt = m_autoData->text();
        if (txt.isEmpty()) { m_autoToggle->setChecked(false); return; }
        QByteArray data;
        if (m_autoHex->isChecked())
            data = QByteArray::fromHex(txt.simplified().remove(' ').toLatin1());
        else
            data = txt.toUtf8();
        m_sender->setData(data);
        m_sender->setInterval(m_intervalSpin->value());
        m_sender->setRepeatCount(m_repeatSpin->value());
        m_sender->setSendCallback([this](const QByteArray &d) -> bool {
            emit sendRequested(d);
            return true;
        });
        m_sender->start();
        m_autoToggle->setText("■ 停止");
        m_sentLabel->setText("已发送：0");
    } else {
        m_sender->stop();
        m_autoToggle->setText("▶ 开始");
    }
}

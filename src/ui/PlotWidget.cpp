#include "PlotWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QSplitter>

const QStringList PlotWidget::COLORS = {
    "#4FC3F7", "#A5D6A7", "#FFB74D", "#F48FB1",
    "#CE93D8", "#80CBC4", "#FFF176", "#FF8A65"
};

PlotWidget::PlotWidget(QWidget *parent) : QWidget(parent)
{
    setObjectName("plotWidget");
    setupUi();
}

void PlotWidget::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    // Controls
    auto *ctrlWidget = new QWidget(this);
    auto *ctrlLayout = new QHBoxLayout(ctrlWidget);
    ctrlLayout->setContentsMargins(0, 0, 0, 0);
    ctrlLayout->setSpacing(6);

    ctrlLayout->addWidget(new QLabel("通道数：", this));
    m_channelSpin = new QSpinBox(this);
    m_channelSpin->setRange(1, 8);
    m_channelSpin->setValue(1);
    ctrlLayout->addWidget(m_channelSpin);

    ctrlLayout->addWidget(new QLabel("窗口大小：", this));
    m_windowSpin = new QSpinBox(this);
    m_windowSpin->setRange(10, 2000);
    m_windowSpin->setValue(200);
    m_windowSpin->setSuffix(" 点");
    ctrlLayout->addWidget(m_windowSpin);

    ctrlLayout->addWidget(new QLabel("分隔符：", this));
    m_sepCombo = new QComboBox(this);
    m_sepCombo->addItem("逗号 ,", ",");
    m_sepCombo->addItem("空格",   " ");
    m_sepCombo->addItem("制表符",     "\t");
    m_sepCombo->addItem("冒号 :", ":");
    ctrlLayout->addWidget(m_sepCombo);

    m_clearBtn = new QPushButton("清除", this);
    m_clearBtn->setObjectName("clearBtn");
    ctrlLayout->addWidget(m_clearBtn);

    ctrlLayout->addStretch();
    m_infoLabel = new QLabel("采样数：0", this);
    ctrlLayout->addWidget(m_infoLabel);

    layout->addWidget(ctrlWidget);

    // Chart
    m_chart = new QChart();
    m_chart->setTheme(QChart::ChartThemeDark);
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    m_chart->setTitle("串口数据波形图");
    m_chart->setBackgroundBrush(QBrush(QColor(0x1E, 0x1E, 0x2E)));
    m_chart->setTitleBrush(QBrush(Qt::white));
    m_chart->setMargins(QMargins(2, 2, 2, 2));

    m_axisX = new QValueAxis();
    m_axisX->setRange(0, m_windowSize);
    m_axisX->setLabelFormat("%d");
    m_axisX->setGridLineColor(QColor(60, 60, 80));
    m_axisX->setLabelsColor(Qt::lightGray);

    m_axisY = new QValueAxis();
    m_axisY->setRange(-10, 10);
    m_axisY->setGridLineColor(QColor(60, 60, 80));
    m_axisY->setLabelsColor(Qt::lightGray);

    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    // Init 1 channel
    onChannelsChanged(1);

    m_chartView = new QChartView(m_chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setObjectName("chartView");
    layout->addWidget(m_chartView, 1);

    connect(m_clearBtn,    &QPushButton::clicked, this, &PlotWidget::onClearClicked);
    connect(m_channelSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PlotWidget::onChannelsChanged);
    connect(m_windowSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int v) { m_windowSize = v; });
}

void PlotWidget::onChannelsChanged(int n)
{
    m_channels = n;
    // Remove old series
    for (auto *s : m_series) {
        m_chart->removeSeries(s);
        delete s;
    }
    m_series.clear();

    // Add new series
    for (int i = 0; i < n; i++) {
        auto *s = new QLineSeries();
        s->setName(QString("CH%1").arg(i+1));
        QPen pen;
        pen.setColor(QColor(COLORS[i % COLORS.size()]));
        pen.setWidth(2);
        s->setPen(pen);
        m_chart->addSeries(s);
        s->attachAxis(m_axisX);
        s->attachAxis(m_axisY);
        m_series.append(s);
    }
}

void PlotWidget::feedData(const QByteArray &data)
{
    m_buffer += QString::fromUtf8(data);
    // Process complete lines
    while (true) {
        int nl = m_buffer.indexOf('\n');
        if (nl < 0) break;
        QString line = m_buffer.left(nl).trimmed();
        m_buffer.remove(0, nl + 1);
        if (!line.isEmpty()) parseLine(line);
    }
}

void PlotWidget::parseLine(const QString &line)
{
    QString sep = m_sepCombo->currentData().toString();
    QStringList parts = line.split(sep, Qt::SkipEmptyParts);

    bool hasDouble = false;
    for (int i = 0; i < qMin(parts.size(), m_channels); i++) {
        bool ok;
        double val = parts[i].trimmed().toDouble(&ok);
        if (ok) {
            addPoint(i, val);
            hasDouble = true;
        }
    }

    if (hasDouble) {
        m_sampleIndex++;
        m_infoLabel->setText(QString("采样数：%1").arg(m_sampleIndex));
    }
}

void PlotWidget::addPoint(int channel, double value)
{
    if (channel >= m_series.size()) return;
    auto *s = m_series[channel];
    s->append(m_sampleIndex, value);

    // Update Y axis
    double yMin = m_axisY->min();
    double yMax = m_axisY->max();
    if (value < yMin) m_axisY->setMin(value * 1.1);
    if (value > yMax) m_axisY->setMax(value * 1.1);

    // Scroll X window
    if (m_sampleIndex > m_windowSize) {
        m_axisX->setRange(m_sampleIndex - m_windowSize, m_sampleIndex);
        // Remove old points
        auto pts = s->points();
        while (pts.size() > m_windowSize + 50) {
            pts.removeFirst();
        }
        s->replace(pts);
    }
}

void PlotWidget::clearPlot()
{
    for (auto *s : m_series) s->clear();
    m_sampleIndex = 0;
    m_axisX->setRange(0, m_windowSize);
    m_axisY->setRange(-10, 10);
    m_buffer.clear();
    m_infoLabel->setText("采样数：0");
}

void PlotWidget::onClearClicked()
{
    clearPlot();
}

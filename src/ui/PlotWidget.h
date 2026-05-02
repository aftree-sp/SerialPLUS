#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

class PlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = nullptr);

    void feedData(const QByteArray &data);
    void clearPlot();

private slots:
    void onClearClicked();
    void onChannelsChanged(int n);

private:
    void setupUi();
    void parseLine(const QString &line);
    void addPoint(int channel, double value);

    QChartView    *m_chartView   = nullptr;
    QChart        *m_chart       = nullptr;
    QValueAxis    *m_axisX       = nullptr;
    QValueAxis    *m_axisY       = nullptr;
    QList<QLineSeries*> m_series;
    QPushButton   *m_clearBtn    = nullptr;
    QSpinBox      *m_channelSpin = nullptr;
    QSpinBox      *m_windowSpin  = nullptr;
    QComboBox     *m_sepCombo    = nullptr;
    QLabel        *m_infoLabel   = nullptr;

    QString        m_buffer;
    qint64         m_sampleIndex = 0;
    int            m_channels    = 1;
    int            m_windowSize  = 200;

    static const QStringList COLORS;
};

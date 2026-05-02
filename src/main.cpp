#include <QApplication>
#include <QFont>
#include <QDir>
#include "ui/MainWindow.h"
#include "ui/ThemeManager.h"

int main(int argc, char *argv[])
{
    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    QApplication app(argc, argv);

    app.setApplicationName("SerialPLUS");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("SerialPLUS");

    // Material Design 字体
    QFont font("Roboto", 10);
    font.setHintingPreference(QFont::PreferNoHinting);
    app.setFont(font);

    // 应用 Material Dark 主题
    ThemeManager::instance().applyTheme(ThemeManager::Dark);

    MainWindow w;
    w.show();

    return app.exec();
}

#include <QApplication>
#include <QTextCodec>

#include "appinfo.h"
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QCoreApplication::setOrganizationName(companyName());
    QCoreApplication::setApplicationName(appName());

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}

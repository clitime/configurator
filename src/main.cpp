#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/favicon.ico"));
    a.setApplicationDisplayName("conf_PTZ");
    MainWindow *w = new MainWindow();
    w->show();
    return a.exec();
}

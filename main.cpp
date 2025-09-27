#include <QApplication>
#include "mainwindow.h"

// Точка входа в программу. Создаёт QApplication, главное окно и запускает цикл событий Qt.
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}

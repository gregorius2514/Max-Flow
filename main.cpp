#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication prog(argc, argv);
    MainWindow window;
    window.setWindowTitle("Maksymalny Przeplyw");
    window.show();
    
    return prog.exec();
}

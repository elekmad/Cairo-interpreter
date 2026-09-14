#include <QApplication>
#include "mainwindow.h"

extern "C"
{
#include <CanvaCtx.h>
#include <OpCanva.h>
#include <OpProgram.h>
#include <parser.tab.h>
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    std::setlocale(LC_ALL, "C");//Rétablie la locale "C" globale pour le parsing "classique" des nombre à virgule.

    MainWindow w;
    w.setWindowTitle("Cairo DSL Editor (C++ / Qt)");
    w.show();

    return app.exec();
}

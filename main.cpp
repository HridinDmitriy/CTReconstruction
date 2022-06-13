#include "MainWindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QApplication a(argc, argv);

    MainWindow w;

    w.setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    w.show();

    return a.exec();
}

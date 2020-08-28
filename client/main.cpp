#include "socket.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    socket w;
    w.show();
    return a.exec();
}

#include "MKCompress.h"
#include "stdafx.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MKCompress w;
    w.show();

    return a.exec();
}

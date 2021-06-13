#include "MKCompress.h"
#include "stdafx.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MKCompress w;
    /*
    QTranslator* chinese_trans = new QTranslator();

    if (chinese_trans->load("mkcompress_zh.qm")) {
        qDebug() << "start loda chinese language";
        a.installTranslator(chinese_trans);
    }
    */
    w.show();
    return a.exec();
}

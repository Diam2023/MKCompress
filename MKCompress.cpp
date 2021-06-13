#include "MKCompress.h"
#include "stdafx.h"

#include <iostream>

using namespace bit7z;
using namespace std;


MKCompress::MKCompress(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    /*
    try {
        Bit7zLibrary lib{ L"7za.dll" };
        BitArchiveInfo arc{ lib, L"C:\\Users\\monoliths\\Downloads\\edge\\7z1900-extra.7z", BitFormat::SevenZip };

        //printing archive metadata
        qDebug() << "Archive properties";
        qDebug() << " Items count: " << arc.itemsCount();
        qDebug() << " Folders count: " << arc.foldersCount();
        qDebug() << " Files count: " << arc.filesCount();
        qDebug() << " Size: " << arc.size();
        qDebug() << " Packed size: " << arc.packSize();
        qDebug() << "test output";

        //printing archive items metadata
        qDebug() << "Archive items";
        auto arc_items = arc.items();
        for (auto& item : arc_items) {
            qDebug() << " Item index: " << item.index();
            qDebug() << "  Name: " << item.name();
            qDebug() << "  Extension: " << item.extension();
            qDebug() << "  Path: " << item.path();
            qDebug() << "  IsDir: " << item.isDir();
            qDebug() << "  Size: " << item.size();
            qDebug() << "  Packed size: " << item.packSize();
        }
    }
    catch (const BitException& ex) {
        qDebug() << ex.what();
    }
    */
}

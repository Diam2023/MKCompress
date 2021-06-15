#include "MKCompress.h"
#include "stdafx.h"

#include <iostream>

#include <qlist.h>

using namespace bit7z;  
using namespace std;

MKCompress::MKCompress(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    init();

    /* connect(ui.fileChoose, QOverload<bool>::of(&QPushButton::clicked), [=](bool check) {
        qDebug() << "test";
    });
    */

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

void MKCompress::init()
{
    fileListData = new QStringList();

    ui.fileListView->setContextMenuPolicy(Qt::CustomContextMenu);

    // connect

    connect(
        ui.fileChoose, QOverload<bool>::of(&QPushButton::clicked), [=](bool check) {
            CFileDialog fileDialog;
            if (fileDialog.exec() == QDialog::Accepted)
            {
                for each (QString element in fileDialog.selectedFiles().toList())
                {
                    if (fileListData->indexOf(element) == -1) {

                        fileListData->append(element);
                    }
                }
                flushData();
            }
        }
    );

    connect(ui.fileListView, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(custumContextMenu(const QPoint&)));

}

void MKCompress::flushData()
{
    ui.fileListView->clear();
    ui.fileListView->addItems(*fileListData);
}

void MKCompress::custumContextMenu(const QPoint& pos)
{
    QListWidgetItem* curItem = ui.fileListView->itemAt(pos);
    if (curItem == NULL)
        return;

    QMenu* popMenu = new QMenu(this);
    QAction* deleteSeed = new QAction(tr("Delete"), this);
    QAction* clearSeeds = new QAction(tr("Clear"), this);
    popMenu->addAction(deleteSeed);
    popMenu->addAction(clearSeeds);
    connect(deleteSeed, SIGNAL(triggered()), this, SLOT(deleteSeedSlot()));
    connect(clearSeeds, SIGNAL(triggered()), this, SLOT(clearSeedsSlot()));
    popMenu->exec(QCursor::pos());
    delete popMenu;
    delete deleteSeed;
    delete clearSeeds;
}

void MKCompress::deleteSeedSlot()
{
    int ch = QMessageBox::warning(NULL, "Warning",
        "Are you sure to delete?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (ch != QMessageBox::Yes)
        return;

    QListWidgetItem* item = ui.fileListView->currentItem();
    if (item == NULL)
        return;

    int curIndex = ui.fileListView->row(item);

    fileListData->takeAt(fileListData->indexOf(item->text()));
    ui.fileListView->takeItem(curIndex);
    delete item;
    flushData();
}

void MKCompress::clearSeedsSlot()
{
    int ch = QMessageBox::warning(NULL, "Warning",
        "Are you sure to clear this list?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (ch != QMessageBox::Yes)
        return;

    QListWidgetItem* item = ui.fileListView->currentItem();
    if (item == NULL)
        return;

    ui.fileListView->clear();
    fileListData->clear();
    delete item;
    flushData();
}



#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MKCompress.h"

class MKCompress : public QMainWindow
{
    Q_OBJECT

public:
    MKCompress(QWidget *parent = Q_NULLPTR);

private:
    Ui::MKCompressClass ui;
    QStringList* fileListData;
    void init();
    void flushData();

private slots:
    void custumContextMenu(const QPoint& pos);
    void deleteSeedSlot();
    void clearSeedsSlot();
};

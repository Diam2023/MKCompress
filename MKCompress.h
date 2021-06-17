#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MKCompress.h"
#include "stdafx.h"

#include "MKCTDLL.h"

const QString DEFAULT_TYPE = ".mkc";

class MKCompress : public QMainWindow
{
    Q_OBJECT

public:
    MKCompress(QWidget *parent = Q_NULLPTR);

private:
    Ui::MKCompressClass ui;

    char* MK_HEAD = " '";
    char* SEVENZ_HEAD = "7z��";

    QStringList* fileListData;
    QString outputFileName;
    QString outputFilePath;
    bool isChangeHead;
    bool isCompress;
    bool isEncryption;
    QString password;

    bool decompress;

    void init();
    void flushData();

    void launchCompress(QStringList inputFiles, QString outputFile, bool isCompress, QString pwd);
    void launchDecompress(QString inputFile, QString outputPath, QString pwd);

private slots:
    void custumContextMenu(const QPoint& pos);
    void deleteSeedSlot();
    void clearSeedsSlot();
    void flushView(bool check);
};

#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MKCompress.h"
#include "stdafx.h"

const QString DEFAULT_TYPE = ".mkc";

// prepar use to next version
enum class RUN_MODE
{
    ONLY_CHANGEHEADTO,
    ONLY_COMPRESS,
    CHANGEHEADTO_ADN_COMPRESS,
    COMPRESS_AND_ENCRYPTION,
    MKCOMPRESS, // changeHeadTo Compress and Encryption
    ONLY_CHANGEHEADBACK,
    ONLY_DECOMPRESS,
    CHANGEHEADBACK_AND_DECOMPRESS,
    DECOMPRESS_AND_ENCRYPTION,
    DEMKCOMPRESS
};

class MKCompress : public QMainWindow
{
    Q_OBJECT

public:
    MKCompress(QWidget *parent = Q_NULLPTR);

private:
    Ui::MKCompressClass ui;

    const std::byte MKC_HEAD[4] { std::byte{0x20}, std::byte{0x15}, std::byte{0x12}, std::byte{0x27} };

    const std::byte SEVENZ_HEAD[4] { std::byte{0x37}, std::byte{0x7a}, std::byte{0xbc}, std::byte{0xaf} };

    QStringList* fileListData;
    QString outputFileName;
    QString outputFilePath;
    bool isChangeHead;
    bool isCompress;
    bool isEncryption;
    bool decompressable;
    QString password;

    bool decompress;

    void init();
    void flushData();

    void launchCompress(QStringList inputFiles, QString outputFile, bool isCompress, QString pwd);
    void launchDecompress(QString inputFile, QString outputPath, QString pwd);

    char* getCharsMKC_HEAD();
    char* getCharsSEVENZ_HEAD();

private slots:
    void openDialog();
    void custumContextMenu(const QPoint& pos);
    void deleteSeedSlot();
    void clearSeedsSlot();
};

#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MKCompress.h"
#include "stdafx.h"

const QString DEFAULT_TYPE = ".mkc";

// prepar use to next version
enum class RUN_MODE
{
	CHANGEHEADER,
	COMPRESS,
	ENCRYPRION, // changeHeadTo Compress and Encryption
};

class MKCompress : public QMainWindow
{
	Q_OBJECT

public:
	MKCompress(QWidget* parent = Q_NULLPTR);
	~MKCompress();

private:
	Ui::MKCompressClass ui;

	// mkc type header
	const std::byte MKC_HEAD[4]{ std::byte{0x20}, std::byte{0x15}, std::byte{0x12}, std::byte{0x27} };

	// 7z header (only use 4 byte)
	const std::byte SEVENZ_HEAD[4]{ std::byte{0x37}, std::byte{0x7a}, std::byte{0xbc}, std::byte{0xaf} };

	// unique_ptr fileListData;
	std::unique_ptr<std::vector<QString>, std::default_delete<std::vector<QString>>> fileListData = std::make_unique<std::vector<QString>>();

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

	void launchCompress(QString outputFile, bool isCompress, QString pwd);
	void launchDecompress(QString inputFile, QString outputPath, QString pwd);

	char* getCharsMKC_HEAD();
	char* getCharsSEVENZ_HEAD();

private slots:
	void openDialog();
	void custumContextMenu(const QPoint& pos);
	void deleteSeedSlot();
	void clearSeedsSlot();
};

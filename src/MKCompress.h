#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MKCompress.h"
#include <QMutex>

const QString DEFAULT_TYPE = ".mkc";

// prepar use to next version
enum class RUN_MODE
{
	CHANGEHEADER,
	COMPRESS,
	ENCRYPRION, // changeHeadTo Compress and Encryption
};

/// <summary>
///	<author>author: monoliths</author>
/// <br/>
/// <version>version: 1.1.0</version>
/// </summary>
class MKCompress : public QMainWindow
{
	Q_OBJECT

public:
	MKCompress(QWidget* parent = Q_NULLPTR);
	~MKCompress();

private:
	Ui::MKCompressClass ui;

	QMutex mutex;

	bool changeHeader;

	QString parent;

	// mkc type header
	const std::byte MKC_HEAD[4]{ std::byte{0x20}, std::byte{0x15}, std::byte{0x12}, std::byte{0x27} };

	void init();

	void launchChangeHeaderTo(const QString inputFile, const QString outputPath);
	void launchChangeHeaderBack(QString inputFile, QString outputPath);

	char* getCharsMKC_HEAD();

private slots:
	void chooseFileDialog();
};

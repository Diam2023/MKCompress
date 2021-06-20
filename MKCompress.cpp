#include "MKCompress.h"
#include "MKCTDLL.hpp"
#include "CFileDialog.h"

using namespace bit7z;

MKCompress::MKCompress(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	init();
}

char* MKCompress::getCharsMKC_HEAD() {
	static char data[4];
	int i = 0;
	for each (std::byte item in MKC_HEAD)
	{
		data[i++] = std::to_integer<char>(item);
	}
	return data;
}

char* MKCompress::getCharsSEVENZ_HEAD() {
	static char data[4];
	int i = 0;
	for each (std::byte item in SEVENZ_HEAD)
	{
		data[i++] = std::to_integer<char>(item);
	}
	return data;
}

/// <summary>
/// main initialize
/// </summary>
void MKCompress::init()
{
	// init fileListData
	fileListData = std::make_unique<QStringList>();

	// to solve add menu
	ui.fileListView->setContextMenuPolicy(Qt::CustomContextMenu);

	flushData();

	connect(ui.openAction, SIGNAL(triggered()), this, SLOT(openDialog()));

	connect(ui.exitAction, SIGNAL(triggered()), qApp, SLOT(exit()));

	connect(ui.fileChoose, SIGNAL(clicked(bool)), this, SLOT(openDialog()));

	connect(ui.fileListView, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(custumContextMenu(const QPoint&)));

	connect(
		ui.outputPathChoose, QOverload<bool>::of(&QPushButton::clicked), [=](bool check) {
			QString filePath = QFileDialog::getExistingDirectory(this, "Choose output path", "./");
			if (filePath.isEmpty())
			{
				return;
			}
			ui.outputPathContent->setText(filePath);
			outputFilePath = filePath;
			flushData();
		}
	);

	connect(
		ui.isChangeHead, QOverload<bool>::of(&QCheckBox::toggled), [=](bool check) {
			if (!decompress) {
				ui.isCompress->setEnabled(false);
				ui.isCompress->setCheckable(true);
			}
			else {
				if (decompressable) {
					ui.isCompress->setEnabled(true);
				}
			}
		}
	);

	connect(
		ui.isCompress, QOverload<bool>::of(&QCheckBox::toggled), [=](bool check) {
			if (!decompress) {
				ui.isCompress->setText(tr("压缩"));
				ui.isEncryption->setText(tr("加密"));
				ui.isEncryption->setChecked(false);
				ui.isEncryption->setEnabled(ui.isCompress->isChecked());
			}
			else {
				ui.isCompress->setText(tr("解压"));
				ui.isEncryption->setText(tr("解密"));
				ui.outPutFileNameContent->setText("");
				ui.outPutFileNameContent->setEnabled(false);
				if (decompressable && ui.isCompress->isChecked()) {
					ui.isEncryption->setEnabled(true);
					ui.isEncryption->setChecked(true);
				}
				else {
					ui.isEncryption->setEnabled(false);
					ui.isEncryption->setChecked(false);
				}
			}
			flushData();
		}
	);
	connect(
		ui.isEncryption, QOverload<bool>::of(&QCheckBox::toggled), [=](bool check) {
			ui.passwordContent->setText("");
			ui.passwordContent->setEnabled(ui.isEncryption->isChecked());
		}
	);

	connect(
		ui.startAction, QOverload<bool>::of(&QPushButton::clicked), [=](bool check) {
			if (outputFileName = ui.outPutFileNameContent->text(), password = ui.passwordContent->text();  !fileListData.get()->isEmpty() && outputFilePath != "") {
				if (decompress && fileListData.get()->size() == 1) {
					launchDecompress(fileListData.get()->at(0), outputFilePath, password);
					return;
				}
				else if (outputFileName != "") {
					launchCompress(*fileListData.get(), outputFilePath + "/" + outputFileName, ui.isCompress->isChecked(), password);
					return;
				}
			}
			ui.outputContent->append(tr("input or output option error!"));
		}
	);

}

/// <summary>
/// flush fileListData to fileListView
/// </summary>
void MKCompress::flushData()
{
	// clear file view
	ui.fileListView->clear();
	// add file data to file view
	ui.fileListView->addItems(*fileListData.get());
	// initialize save path
	if (!fileListData.get()->isEmpty()) {
		QFileInfo* file = new QFileInfo(fileListData.get()->at(0));
		// get file parent path
		if (ui.outputPathContent->text() == "") {
			QString parentPath = file->absolutePath();
			ui.outputPathContent->setText(parentPath);
			outputFilePath = parentPath;
		}
		if (!decompress && ui.isCompress->isChecked() && ui.outPutFileNameContent->text() == "") {
			QString fileName = file->baseName() + DEFAULT_TYPE;
			ui.outPutFileNameContent->setText(fileName);
			outputFileName = fileName;
		}
		// free space
		delete file;
	}
	ui.outputContent->clear();
	ui.isChangeHead->setChecked(true);
	if (!fileListData.get()->isEmpty()) {
		QString fileName = fileListData.get()->at(0);
		auto fileInfo = std::make_unique<QFileInfo>(fileName);
		wchar_t* file = _wcsdup(fileName.toStdWString().c_str());
		// if file is .mkc
		if ((fileListData.get()->size() == 1) && fileInfo.get()->isFile() && (*_getFileHeader(file) == *getCharsMKC_HEAD())) {
			ui.outputContent->append(tr("已识别到MKC文件 \n"));
			ui.outputContent->append(tr("还原模式 \n"));
			// decompress mode
			decompressable = (*_getMKFileHeader(file) == *getCharsSEVENZ_HEAD());
			decompress = true;

			ui.isChangeHead->setEnabled(false);
			ui.isChangeHead->setChecked(true);
			if (decompressable) {
				ui.outputContent->append(tr("已识别到7z文件，可进行解压 \n"));
				ui.isCompress->setEnabled(true);
				ui.isCompress->setChecked(true);
				ui.isEncryption->setEnabled(true);
				ui.isEncryption->setChecked(false);
			}
			else {
				ui.isCompress->setEnabled(false);
				ui.isCompress->setChecked(false);
				ui.isEncryption->setEnabled(false);
				ui.isEncryption->setChecked(false);
			}
		}
		else {
			ui.outputContent->append(tr("压缩模式 \n"));
			// compress mode
			ui.outPutFileNameContent->setEnabled(true);
			ui.isChangeHead->setEnabled(true);
			ui.isChangeHead->setChecked(true);
			ui.isCompress->setChecked(true);
			ui.isCompress->setEnabled(false);
			ui.isEncryption->setChecked(false);
			ui.passwordContent->setEnabled(false);
			// flag
			decompress = false;
		}
		free(file);
		fileInfo.release();
	}
	else {
		ui.outputContent->append(tr("请选择文件 \n"));
		// init check box
		ui.isChangeHead->setChecked(true);
		ui.isCompress->setChecked(false);
		ui.isCompress->setEnabled(false);
		ui.isEncryption->setChecked(false);
		ui.isEncryption->setEnabled(false);
		ui.passwordContent->setEnabled(false);
		decompress = false;
	}
}

void MKCompress::launchCompress(QStringList inputFiles, QString outputFile, bool isCompress, QString pwd)
{
	ui.outputContent->append(tr("Compress message:"));
	ui.outputContent->append(tr("|- Compress file list:"));
	ui.outputContent->append(tr("|- Output file path:") + outputFile);
	ui.outputContent->append(tr("|- Password:") + pwd);

	try {
		if (ui.isCompress->isChecked()) {
			Bit7zLibrary lib{ L"7za.dll" };
			BitCompressor compressor{ lib, BitFormat::SevenZip };
			if (pwd != "") {
				compressor.setPassword(pwd.toStdWString());
			}
			compressor.setUpdateMode(true);
			for each (QString element in inputFiles.toList())
			{
				QFileInfo file(element);
				if (file.isDir()) {
					compressor.compressDirectory(element.toStdWString(), outputFile.toStdWString());
				}
				else if (file.isFile()) {
					compressor.compressFile(element.toStdWString(), outputFile.toStdWString());
				}
				else {
					ui.outputContent->append(tr("|- ignor:") + element);
				}
				ui.outputContent->append(tr("  |- ") + element);
			}
			ui.outputContent->append(tr("comrpess action successful \n"));
		}
		else {
			outputFile = inputFiles.at(0);
		}
		if (ui.isChangeHead->isChecked()) {
			wchar_t* file = _wcsdup(outputFile.toStdWString().c_str());
			int result = static_cast<int>(_changeHeaderTo(file));
			if (result == 0) {
				ui.outputContent->append(tr("change header successful"));
			}
			else {
				ui.outputContent->append(tr("change header error"));
				ui.outputContent->append(tr("return value: " + result));
			}
			ui.outputContent->append(tr("change head to action successful \n"));
			free(file);
		}
	}
	catch (const BitException& ex) {
		// synchronize to conslot
		qDebug() << ex.what();
		ui.outputContent->append(ex.what());
	}
}

void MKCompress::launchDecompress(QString inputFile, QString outputPath, QString pwd)
{
	ui.outputContent->append("Decompress message:");
	ui.outputContent->append("|- Decompress file:" + inputFile);
	ui.outputContent->append("|- Output file path:" + ui.outputPathContent->text());
	ui.outputContent->append("|- Password:" + pwd);

	wchar_t* file = _wcsdup(inputFile.toStdWString().c_str());
	try {
		// change head back
		int result = static_cast<int>(_changeHeaderBack(file));
		if (result == 0) {
			ui.outputContent->append("change header successful");

			if (ui.isCompress->isChecked()) {
				Bit7zLibrary lib{ L"7za.dll" };
				BitExtractor extractor{ lib, BitFormat::SevenZip };
				if (pwd != "") {
					extractor.setPassword(pwd.toStdWString());
				}
				extractor.extract(inputFile.toStdWString(), outputPath.toStdWString());
			}
			if (!ui.isCompress->isChecked()) {
				// change head to
				result = static_cast<int>(_changeHeaderTo(file));
				if (result == 0) {
					ui.outputContent->append("action successful \n");
				}
			}
			ui.outputContent->append(tr("decomrpess action successful \n"));
		}
		if (result != 0) {
			ui.outputContent->append("change header error");
			ui.outputContent->append(tr("return value: " + result));
		}
	}
	catch (const BitException& ex) {
		// synchronize to conslot
		qDebug() << ex.what();
		ui.outputContent->append(ex.what());
	}
	free(file);
}

void MKCompress::custumContextMenu(const QPoint& pos)
{
	QListWidgetItem* curItem = ui.fileListView->itemAt(pos);
	if (curItem == NULL)
		return;

	auto popMenu = std::make_unique<QMenu>(this);
	auto deleteSeed = std::make_unique<QAction>(tr("Delete"), this);
	auto clearSeeds = std::make_unique<QAction>(tr("Clear"), this);

	popMenu.get()->addAction(deleteSeed.get());
	popMenu.get()->addAction(clearSeeds.get());
	connect(deleteSeed.get(), SIGNAL(triggered()), this, SLOT(deleteSeedSlot()));
	connect(clearSeeds.get(), SIGNAL(triggered()), this, SLOT(clearSeedsSlot()));
	popMenu->exec(QCursor::pos());
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

	fileListData.get()->takeAt(fileListData.get()->indexOf(item->text()));
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
	fileListData.get()->clear();
	flushData();
}

void MKCompress::openDialog()
{
	CFileDialog fileDialog;
	if (fileDialog.exec() == QDialog::Accepted)
	{
		for each (QString element in fileDialog.selectedFiles().toList())
		{
			if (fileListData.get()->indexOf(element) == -1) {
				fileListData.get()->append(element);
			}
		}
		flushData();
	}
}
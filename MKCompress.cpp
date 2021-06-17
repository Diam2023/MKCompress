#include "MKCompress.h"
#include "stdafx.h"

#include <iostream>

#include <qlist.h>

using namespace bit7z;
using namespace std;

MKCompress::MKCompress(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	init();
}

/// <summary>
/// main initialize
/// </summary>
void MKCompress::init()
{
	// init fileListData
	fileListData = new QStringList();

	// to solve add menu
	ui.fileListView->setContextMenuPolicy(Qt::CustomContextMenu);

	flushData();

	flushView(false);

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

	connect(ui.isChangeHead, SIGNAL(toggled(bool)), this, SLOT(flushView(bool)));
	connect(ui.isCompress, SIGNAL(toggled(bool)), this, SLOT(flushView(bool)));
	connect(ui.isEncryption, SIGNAL(toggled(bool)), this, SLOT(flushView(bool)));

	connect(
		ui.startAction, QOverload<bool>::of(&QPushButton::clicked), [=](bool check) {
			if (outputFileName = ui.outPutFileNameContent->text(), password = ui.passwordContent->text();  !fileListData->isEmpty() && outputFilePath != "") {
				if (decompress && fileListData->size() == 1) {
					launchDecompress(fileListData->at(0), outputFilePath, password);
					return;
				}
				else if (outputFileName != "") {
					launchCompress(*fileListData, outputFilePath + "/" + outputFileName, ui.isCompress->isChecked(), password);
					return;
				}
			}
			ui.outputContent->append("input or output option error!");
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
	ui.fileListView->addItems(*fileListData);
	// initialize save path
	if (!fileListData->isEmpty()) {
		QFileInfo* file = new QFileInfo(fileListData->at(0));
		// get file parent path
		if (ui.outputPathContent->text() == "") {
			QString parentPath = file->absolutePath();
			ui.outputPathContent->setText(parentPath);
			outputFilePath = parentPath;
		}
		if (ui.outPutFileNameContent->text() == "") {
			QString fileName = file->baseName() + DEFAULT_TYPE;
			ui.outPutFileNameContent->setText(fileName);
			outputFileName = fileName;
		}
		// free space
		delete file;
	}
	ui.outputContent->clear();
	ui.isChangeHead->setChecked(true);
	if (!fileListData->isEmpty()) {
		QString fileName = fileListData->at(0);
		QFileInfo file(fileName);
		// if file is .mkc
		if ((fileListData->size() == 1) && file.isFile() && (*_getFileHeader(fileName.toStdString()) == *MK_HEAD)) {
			ui.outputContent->append("The file to be extracted has been identified \n");
			ui.outputContent->append("decpompress mode \n");
			// decompress mode
			ui.isChangeHead->setEnabled(false);
			ui.isCompress->setEnabled(false);
			if (*_getMKFileHeader(fileName.toStdString()) == *SEVENZ_HEAD) {
				ui.isCompress->setChecked(true);
			}
			else {
				ui.isCompress->setChecked(false);
			}
			ui.outPutFileNameContent->setEnabled(false);
			ui.outPutFileNameContent->setText("");
			decompress = true;
		}
		else {
			ui.outputContent->append("compress mode \n");
			// compress mode
			ui.isChangeHead->setEnabled(true);
			ui.isCompress->setEnabled(true);
			ui.isCompress->setChecked(true);
			decompress = false;
		}
	}
	else {
		ui.outputContent->append("please choose file \n");
		// init
		ui.isChangeHead->setEnabled(true);
		ui.isCompress->setEnabled(false);
		ui.isEncryption->setEnabled(false);
	}
}

void MKCompress::launchCompress(QStringList inputFiles, QString outputFile, bool isCompress, QString pwd)
{
	ui.outputContent->append("Compress message:");
	ui.outputContent->append("|- Compress file list:");
	ui.outputContent->append("|- Output file path:" + outputFile);
	ui.outputContent->append("|- Password:" + pwd);
	try {
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
				ui.outputContent->append("|- ignor:" + element);
			}
			ui.outputContent->append("  |- " + element);
		}

		if (_changeHeaderTo(outputFile.toStdString()) == FileErrEnum::NO_ERR) {
			ui.outputContent->append("change header successful");
		}
		else {
			ui.outputContent->append("change header error");
		}
	}
	catch (const BitException& ex) {
		ui.outputContent->append(ex.what());
	}

}

void MKCompress::launchDecompress(QString inputFile, QString outputPath, QString pwd)
{
	ui.outputContent->append("Decompress message:");
	ui.outputContent->append("|- Decompress file:" + inputFile);
	ui.outputContent->append("|- Output file path:" + outputFilePath);
	ui.outputContent->append("|- Password:" + pwd);

	// _changeHeaderTo(inputFile.toStdString());

	try {
		Bit7zLibrary lib{ L"7za.dll" };
		BitArchiveInfo arc{ lib, inputFile.toStdWString(), BitFormat::SevenZip };
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
		ui.outputContent->append(ex.what());
	}

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
	flushData();
}

void MKCompress::flushView(bool check)
{
	/*
	if (ui.isChangeHead->isEnabled()) {
		if (ui.isChangeHead->isChecked()) {
			ui.isChangeHead->setEnabled(true);
			if (fileListData->size() > 1) {
				ui.isChangeHead->setEnabled(true);
				ui.isChangeHead->setChecked(true);
				ui.isCompress->setEnabled(true);
				ui.isCompress->setChecked(true);
			}
			if (ui.isCompress->isEnabled()) {
				if (ui.isCompress->isChecked()) {
					ui.isEncryption->setEnabled(true);
				}
				else {
					ui.isEncryption->setEnabled(false);
					ui.isEncryption->setChecked(false);
				}
			}
			else {
				ui.isCompress->setEnabled(true);
			}
		}
		else {
			ui.isCompress->setEnabled(false);
			ui.isCompress->setChecked(false);
			ui.isEncryption->setEnabled(false);
			ui.isEncryption->setChecked(false);
		}
	}
	else {
		if (!fileListData->isEmpty()) {
			ui.isChangeHead->setEnabled(true);
		}
		else {
			ui.isChangeHead->setEnabled(false);
			ui.isChangeHead->setChecked(false);
		}
	}
	if (ui.isEncryption->isEnabled() && ui.isEncryption->isChecked()) {
		ui.passwordContent->setEnabled(true);
	}
	else {
		ui.passwordContent->setEnabled(false);
	}
	*/
}

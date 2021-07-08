# pragma warning (disable:4819)

#include "MKCompress.h"
#include "MKCTDLL.hpp"
#include "CFileDialog.h"

using namespace bit7z;

MKCompress::MKCompress(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	this->setWindowIcon(QIcon(":/img/mkcompress_o.png"));

	init();
}

MKCompress::~MKCompress() {

	delete fileListData.release();
	fileListData = NULL;

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
	// fileListData = new std::vector<QString>();

	// to solve add menu
	ui.fileListView->setContextMenuPolicy(Qt::CustomContextMenu);

	flushData();

	connect(ui.openAction, SIGNAL(triggered()), this, SLOT(openDialog()));

	connect(ui.exitAction, SIGNAL(triggered()), qApp, SLOT(exit()));

	connect(ui.fileChoose, SIGNAL(clicked(bool)), this, SLOT(openDialog()));

	connect(ui.fileListView, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(custumContextMenu(const QPoint&)));

	connect(
		ui.outputPathChoose, QOverload<bool>::of(&QPushButton::clicked), [=](bool check) {
			QString filePath = QFileDialog::getExistingDirectory(this, tr("选择输出路径"), tr("/"));
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
			if (outputFileName = ui.outPutFileNameContent->text(), password = ui.passwordContent->text();  !fileListData->empty() && outputFilePath != "") {
				if (decompress && fileListData->size() == 1) {
					launchDecompress(fileListData->at(0), outputFilePath, password);
					return;
				}
				else if (outputFileName != "") {
					launchCompress(outputFilePath + tr("/") + outputFileName, ui.isCompress->isChecked(), password);
					return;
				}
			}
			ui.outputContent->append(tr("输入文件无效、输出路径或输出文件名错误!"));
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
	for (auto iterator = fileListData->begin(); iterator != fileListData->end(); iterator++)
	{
		ui.fileListView->addItem(*iterator);
	}
	// initialize save path

	if (!fileListData->empty()) {
		auto file = std::make_unique<QFileInfo>(fileListData->at(0));
		// get file parent path
		if (ui.outputPathContent->text() == "") {
			QString parentPath = file->absolutePath();
			ui.outputPathContent->setText(parentPath);
			outputFilePath = parentPath;
		}
		if (!decompress && (ui.isChangeHead->isChecked() || ui.isCompress->isChecked()) && ui.outPutFileNameContent->text() == "") {
			QString fileName = file->baseName() + DEFAULT_TYPE;
			ui.outPutFileNameContent->setText(fileName);
			outputFileName = fileName;
		}
		// free space
		ui.outputContent->clear();
		ui.isChangeHead->setChecked(true);
		// if file is .mkc
		const wchar_t* fileWString = reinterpret_cast<const wchar_t*>(fileListData->at(0).utf16());
		if ((fileListData->size() == 1) && file->isFile() && (*_getFileHeader(fileWString) == *getCharsMKC_HEAD())) {
			ui.outputContent->append(tr("已识别到MKC文件 \n"));
			ui.outputContent->append(tr("还原模式 \n"));
			// decompress mode
			decompressable = (*_getMKFileHeader(fileWString) == *getCharsSEVENZ_HEAD());
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

		delete file.release();
		file = NULL;
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

void MKCompress::launchCompress(QString outputFile, bool isCompress, QString pwd)
{
	ui.outputContent->append(tr("Compress message:"));
	ui.outputContent->append(tr("|- Compress file list:"));
	ui.outputContent->append(tr("|- Output file path:") + outputFile);

	try {
		if (ui.isCompress->isChecked()) {
			Bit7zLibrary lib{ L"7za.dll" };

			auto compressor = std::make_unique<BitCompressor>(lib, BitFormat::SevenZip);
			if (pwd != "") {
				const wstring password = std::wstring(reinterpret_cast<const wchar_t*>(pwd.utf16()));
				compressor->setPassword(password);
				ui.outputContent->append(tr("|- Password:") + pwd);
			}
			compressor->setUpdateMode(true);

			for (auto iterator = fileListData->begin(); iterator != fileListData->end(); iterator++)
			{
				auto element = *iterator;
				auto file = std::make_unique<QFileInfo>(element);

				const wstring inputFileWString = std::wstring(reinterpret_cast<const wchar_t*>(iterator->utf16()));
				const wstring outputFileWString = std::wstring(reinterpret_cast<const wchar_t*>(outputFile.utf16()));

				if (file->isDir()) {
					compressor->compressDirectory(inputFileWString, outputFileWString);
				}
				else if (file->isFile()) {
					compressor->compressFile(inputFileWString, outputFileWString);
				}
				else {
					ui.outputContent->append(tr("|- ignor:") + *iterator);
				}
				ui.outputContent->append(tr("  |- ") + *iterator);
				delete file.release();
				file = NULL;
			}

			delete compressor.release();
			compressor = NULL;

			ui.outputContent->append(tr("压缩成功 \n"));
		}
		if (ui.isChangeHead->isChecked()) {
			auto opf = std::make_unique<QFileInfo>(outputFile);

			if (!opf->exists()) {
				ui.outputContent->append(tr("准备拷贝文件"));

				if (QFile::copy(fileListData->at(0), outputFile)) {
					ui.outputContent->append(tr("文件拷贝成功!"));
				}
				else {
					ui.outputContent->append(tr("文件拷贝失败!"));
				}
			}
			delete opf.release();
			opf = NULL;

			const wchar_t* outputFileWChar = reinterpret_cast<const wchar_t*>(outputFile.utf16());
			qDebug();
			errno_t result = _changeHeaderTo(outputFileWChar);

			if (result == 0) {
				ui.outputContent->append(tr("change header successful"));
			}
			else {
				ui.outputContent->append(tr("change header to error"));
				ui.outputContent->append(tr("错误返回值:"));
				ui.outputContent->append(QString::QString(QChar(result)));
			}
			ui.outputContent->append(tr("change head to successful \n"));
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
	ui.outputContent->append(tr("Decompress message:"));
	ui.outputContent->append(tr("|- Decompress file:") + inputFile);
	ui.outputContent->append(tr("|- Output file path:") + ui.outputPathContent->text());

	try {
		// change head back
		const wchar_t* fileWString = reinterpret_cast<const wchar_t*>(inputFile.utf16());

		errno_t result = _changeHeaderBack(fileWString);
		if (result == 0) {
			ui.outputContent->append(tr("-change header back successful"));

			// check can extractor
			if (ui.isCompress->isChecked()) {
				Bit7zLibrary lib{ L"7za.dll" };

				// create a smart pointer
				auto extractor = std::make_unique<BitExtractor>(lib, BitFormat::SevenZip);
				if (pwd != "") {
					const wstring password = std::wstring(reinterpret_cast<const wchar_t*>(pwd.utf16()));
					ui.outputContent->append(tr("|- Password:") + pwd);
					extractor->setPassword(password);
				}

				const wstring inputFileWString = std::wstring(reinterpret_cast<const wchar_t*>(inputFile.utf16()));
				const wstring outputPathWString = std::wstring(reinterpret_cast<const wchar_t*>(outputPath.utf16()));

				extractor->extract(inputFileWString, outputPathWString);

				// release pointer
				delete extractor.release();
				extractor = NULL;
			}
			if (!ui.isCompress->isChecked()) {
				// change head to
				result = static_cast<int>(_changeHeaderTo(fileWString));
				if (result == 0) {
					ui.outputContent->append(tr("change head to error \n"));
				}
			}
			ui.outputContent->append(tr("解压成功 \n"));
		}
		if (result != 0) {
			ui.outputContent->append(tr("change head back error"));
			ui.outputContent->append(tr("返回值: " + result));
		}
	}
	catch (const BitException& ex) {
		// synchronize to conslot
		qDebug() << ex.what();
		ui.outputContent->append(ex.what());
	}
}

void MKCompress::custumContextMenu(const QPoint& pos)
{
	QListWidgetItem* curItem = ui.fileListView->itemAt(pos);
	if (curItem == NULL) return;

	auto popMenu = std::make_unique<QMenu>(this);
	auto deleteSeed = std::make_unique<QAction>(tr("移除选中文件"), this);
	auto clearSeeds = std::make_unique<QAction>(tr("清空输入列表"), this);

	popMenu.get()->addAction(deleteSeed.get());
	popMenu.get()->addAction(clearSeeds.get());
	connect(deleteSeed.get(), SIGNAL(triggered()), this, SLOT(deleteSeedSlot()));
	connect(clearSeeds.get(), SIGNAL(triggered()), this, SLOT(clearSeedsSlot()));
	popMenu->exec(QCursor::pos());
}

void MKCompress::deleteSeedSlot()
{
	int ch = QMessageBox::warning(NULL, tr("Warning"),
		tr("你确定要删除吗?"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No);

	if (ch != QMessageBox::Yes) return;

	QListWidgetItem* item = ui.fileListView->currentItem();
	if (item == NULL) return;

	int curIndex = ui.fileListView->row(item);

	for (auto iterator = fileListData->begin(); iterator != fileListData->end(); )
	{
		if (*iterator == item->text()) {
			iterator = this->fileListData->erase(iterator);
		}
		else {
			++iterator;
		}
	}

	ui.fileListView->takeItem(curIndex);

	delete item;
	flushData();
}

void MKCompress::clearSeedsSlot()
{
	int ch = QMessageBox::warning(NULL, tr("Warning"),
		tr("你确定要清除列表吗?"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No);

	if (ch != QMessageBox::Yes) return;
	QListWidgetItem* item = ui.fileListView->currentItem();
	if (item == NULL) return;
	ui.fileListView->clear();
	fileListData->clear();
	flushData();
}

void MKCompress::openDialog()
{
	auto fileDialog = std::make_unique<CFileDialog>();

	if (fileDialog->exec() == QDialog::Accepted)
	{
		for each (const QString element in fileDialog->selectedFiles())
		{
			bool flag = true;

			std::vector<QString>::const_iterator iterator;
			for (iterator = fileListData->begin(); iterator != fileListData->end(); iterator++)
			{
				if (*iterator == element) {
					flag = false;
				}
			}
			if (flag) {
				this->fileListData->push_back(element);
			}

			flag = true;
		}
		flushData();
	}
	delete fileDialog.release();
	fileDialog = NULL;
}

#include "MKCompress.h"
#include "MKCTDLL.hpp"
#include "CFileDialog.h"

using namespace bit7z;

MKCompress::MKCompress(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	MKC_HEADER = mkc::stdBytesToChars(mkc::MKC_HEAD);

	SEVENZ_HEADER = mkc::stdBytesToChars(mkc::SEVENZ_HEAD);

	this->setWindowIcon(QIcon(tr(":/img/mkcompress_o.png")));

	init();
}

MKCompress::~MKCompress()
{
	delete fileListData.release();
	fileListData = NULL;

	free(MKC_HEADER);
	free(SEVENZ_HEADER);

	MKC_HEADER = nullptr;
	SEVENZ_HEADER = nullptr;
}

void MKCompress::outputFlag()
{
	qDebug() << tr("------------------------");
	qDebug() << tr("forward:") << launchFlag->forward;
	qDebug() << tr("change headerable:") << launchFlag->changeHeaderAble;
	qDebug() << tr("compressable:") << launchFlag->compressAble;
	qDebug() << tr("change header:") << launchFlag->changeHeader;
	qDebug() << tr("compress:") << launchFlag->compress;
	qDebug() << tr("mode:") << static_cast<int>(launchFlag->mode);
}

void MKCompress::init()
{
	flushData();

	// Solve add menu
	ui.fileListView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui.openAction, SIGNAL(triggered()), this, SLOT(openDialog()));
	connect(ui.fileChoose, SIGNAL(clicked(bool)), this, SLOT(openDialog()));

	connect(ui.exitAction, SIGNAL(triggered()), qApp, SLOT(exit()));

	connect(ui.fileListView, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(custumContextMenu(const QPoint&)));

	connect(
		ui.outputPathChoose, QOverload<bool>::of(&QPushButton::clicked), [=](bool check) {
			QString filePath = QFileDialog::getExistingDirectory(this, tr("选择输出路径"), tr("/"));
			if (filePath.isEmpty())
			{
				return;
			}
			ui.outputPathContent->setText(filePath);
		}
	);

	connect(
		ui.isChangeHead, QOverload<bool>::of(&QCheckBox::toggled), [=](bool check) {
			launchFlag->changeHeader = ui.isChangeHead->isChecked();

			flushLaunchStatus();
			
			flushOutputPath();
		}
	);

	connect(
		ui.isCompress, QOverload<bool>::of(&QCheckBox::toggled), [=](bool check) {
			launchFlag->compress = ui.isCompress->isChecked();

			flushLaunchStatus();

			flushCheckBoxStatus();

			flushOutputPath();

			if (launchFlag->compressAble && launchFlag->compress) {
				ui.passwordContent->setText(tr(""));
				ui.passwordContent->setEnabled(true);
			}
			else {
				ui.passwordContent->setText("");
				ui.passwordContent->setEnabled(false);
			}
		}
	);

	connect(
		ui.startAction, QOverload<bool>::of(&QPushButton::clicked), [=](bool check) {
			QString outputFileName;
			QString outputFilePath;

			if (outputFilePath = ui.outputPathContent->text();
				!fileListData->empty() && outputFilePath != "") {

				QString opf;
				
				if (launchFlag->forward) {
					if (outputFileName = ui.outPutFileNameContent->text(); outputFileName != "") {
						password = ui.passwordContent->text();
						// unique pointer
						auto dir = std::make_unique<QDir>(outputFilePath);
						opf = dir->absoluteFilePath(outputFileName);
						// release dir space
						delete dir.release();
						dir = nullptr;
					}
					else {
						ui.outputContent->append(tr("输入无效!"));
						return;
					}
				}
				else {
					opf = outputFilePath;
				}
				// wait to reconstruct
				launchCompress(launchFlag->compress, fileListData, opf, password);
			}
			else {
				ui.outputContent->append(tr("输入无效!"));
				return;
			}
			qDebug() << tr("launch");
			outputFlag();
		}
	);
}

void MKCompress::flushData()
{
	launchFlag->changeHeader = ui.isChangeHead->isChecked();
	launchFlag->compress = ui.isCompress->isChecked();

	// clear file view
	ui.fileListView->clear();
	// add file data to file view
	for (auto iterator = fileListData->begin(); iterator != fileListData->end(); iterator++)
	{
		ui.fileListView->addItem(*iterator);
	}

	// initialize save path
	if (!fileListData->empty()) {
		// git first file to initialize output
		auto file = std::make_unique<QFileInfo>(fileListData->at(0));
		// clear output
		ui.outputContent->clear();

		const wchar_t* fileWString = reinterpret_cast<const wchar_t*>(fileListData->at(0).utf16());

		// if has data
		if (fileListData->size() == 1 && file->isFile()) {
			// if is file
			// MKC file to change header back
			if (*_getFileHeader(fileWString) == *MKC_HEADER) {
				// default to unzip
				launchFlag->forward = false;
				launchFlag->changeHeaderAble = true;
				// change header back mode
				if (*_getMKFileHeader(fileWString) == *SEVENZ_HEADER) {
					// It can be compress
					launchFlag->compressAble = true;
				} // It can not be compress
				else {
					launchFlag->compressAble = false;
				}

				// TODO wait to add output file name
				ui.outputContent->append(tr("已识别到MKC文件\n"));
				ui.outputContent->append(tr("还原模式\n"));
			} // to change header
			else {
				launchFlag->forward = true;
				launchFlag->changeHeaderAble = true;
				launchFlag->compressAble = true;

				ui.outputContent->append(tr("换头模式\n"));
			}
		}
		// if is not file
		// if is a floder or have more than one file
		// it will be Compress
		else {
			// can be compress and change header
			launchFlag->forward = true;
			launchFlag->compressAble = true;
			launchFlag->changeHeaderAble = false;

			ui.outputContent->append(tr("压缩模式\n"));
		}

		flushCheckBoxStatus();

		flushOutputPath();

		delete file.release();
		file = NULL;
	} // init check box
	else {
		ui.isChangeHead->setChecked(false);
		ui.isChangeHead->setCheckable(false);
		ui.isCompress->setChecked(false);
		ui.isCompress->setCheckable(false);
		ui.passwordContent->setEnabled(false);
		ui.passwordContent->setText("");
		ui.outPutFileNameContent->setText("");
		ui.outputContent->setText("");
	}
}

void MKCompress::flushOutputPath() {
	if (!fileListData->empty()) {
		auto file = std::make_unique<QFileInfo>(fileListData->at(0));
		if (launchFlag->forward) {
			// set output file name
			if (launchFlag->compressAble) {
				// QString fileName = (ui.outPutFileNameContent->text() == "") ? file->baseName() : ui.outPutFileNameContent->text();
				QString fileName = file->baseName();
				if (launchFlag->changeHeader) {
					fileName += mkc::MKC_TYPE;
				}
				else if (launchFlag->compress) {
					fileName += mkc::SEVENZ_TYPE;
				}
				else {
					fileName = "";
					qDebug() << tr("skip output file name");
					// ignor
				}
				ui.outPutFileNameContent->setText(fileName);
			}

			// set output file path
			if (ui.outputPathContent->text() == "") {
				ui.outputPathContent->setText(file->absolutePath());
			}
		}
		else {
			// set output file path
			auto dir = std::make_unique<QDir>(file->absolutePath());
			ui.outputPathContent->setText(dir->absoluteFilePath(file->baseName()));
			// release dir space
			delete dir.release();
			dir = nullptr;

			// TODO to get file name
		}

		delete file.release();
		file = NULL;
	}
}
void MKCompress::flushCheckBoxStatus() {
	if (launchFlag->changeHeaderAble || launchFlag->compress) {
		ui.isChangeHead->setCheckable(true);
		ui.isChangeHead->setChecked(true);
	}
	else {
		ui.isChangeHead->setChecked(false);
		ui.isChangeHead->setCheckable(false);
	}
	if (launchFlag->compressAble) {
		ui.isCompress->setCheckable(true);
		if (launchFlag->changeHeaderAble) {
			ui.isChangeHead->setChecked(true);
		}
		else {
			ui.isChangeHead->setChecked(false);
		}
	}
	else {
		ui.isCompress->setChecked(false);
		ui.isCompress->setCheckable(false);
	}

	if (launchFlag->forward) {
		ui.isCompress->setText(tr("压缩"));
		ui.outPutFileNameContent->setEnabled(true);
	}
	else {
		ui.isCompress->setText(tr("解压"));
		ui.outPutFileNameContent->setText("");
		ui.outPutFileNameContent->setEnabled(false);
	}
}

void MKCompress::flushLaunchStatus()
{
	if (launchFlag->changeHeader && launchFlag->compress) {
		launchFlag->mode = mkc::COMPRESS_MODE::COMPRESS_AND_CHANGE_HEADER;
	}
	else if (launchFlag->changeHeader) {
		launchFlag->mode = mkc::COMPRESS_MODE::ONLY_CHANGE_HEADER;
	}
	else if (launchFlag->compress) {
		launchFlag->mode = mkc::COMPRESS_MODE::ONLY_COMPRESS;
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
		tr("你确定要删除?"),
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
		tr("你确定要清除列表?"),
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

void MKCompress::launchCompress(bool flag, std::unique_ptr<std::vector<QString>, std::default_delete<std::vector<QString>>>& inputFileList, QString outputFile, QString pwd)
{
}

void MKCompress::changeHeader(bool flag, QString path)
{
}
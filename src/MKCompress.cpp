#include "MKCompress.h"
//#include "MKCTDLL.hpp"
#include <QFileDialog>
#include <memory>

/*
* use version 1.0.0 .
*/

MKCompress::MKCompress(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	this->setWindowIcon(QIcon(":/../resources/mkcompress_o.png"));

	init();
}

MKCompress::~MKCompress() {
}

char* MKCompress::getCharsMKC_HEAD() {
	static char data[4];
	int i = 0;
	for (std::byte item : MKC_HEAD)
	{
		data[i++] = std::to_integer<char>(item);
	}
	return data;
}

/// <summary>
/// <p>main initialize</p>
/// </summary>
void MKCompress::init()
{
	changeHeader = true;

	/// <summary>
	/// <p> slight MLCompres 1.0.0 version </p>
	/// can not cover choose path
	/// </summary>
	parent = "";
	ui.outputPathContent->setEnabled(false);
	ui.outputPathChoose->setEnabled(false);

	// show main message
	ui.status->addWidget(new QLabel(tr("请选择文件")));

	connect(ui.openAction, SIGNAL(triggered()), this, SLOT(chooseFileDialog()));

	connect(ui.exitAction, SIGNAL(triggered()), qApp, SLOT(exit()));

	connect(ui.fileChoose, SIGNAL(clicked(bool)), this, SLOT(chooseFileDialog()));

	connect(
		ui.outputPathChoose, QOverload<bool>::of(&QPushButton::clicked), [=](bool check) {
			QString filePath = QFileDialog::getExistingDirectory(this, tr("选择输出路径"), tr("/"));
			if (filePath.isEmpty())
			{
				return;
			}
			if ((parent != "") && (filePath != parent)) {
				ui.outputPathContent->setEnabled(true);
				ui.outputPathContent->setText(filePath);
				ui.status->showMessage("输出路径不能与源文件相同!");
			}
		}
	);

	connect(
		ui.startAction, QOverload<bool>::of(&QPushButton::clicked), [=](bool check) {
			if (changeHeader) {
				launchChangeHeaderTo(ui.inputFilePathContent->toPlainText(), parent);
			}
			else {
				launchChangeHeaderBack(ui.inputFilePathContent->toPlainText(), parent);
			}
		}
	);

}

void MKCompress::launchChangeHeaderTo(const QString inputFile, const QString outputPath)
{
	mutex.lock();

	const wchar_t* inputFileWString = reinterpret_cast<const wchar_t*>(inputFile.utf16());
	// const wchar_t* outputPathWString = reinterpret_cast<const wchar_t*>(outputPath.utf16());
    error_t result = _changeHeaderTo(inputFileWString);

	if (result == 0) {
		ui.status->addPermanentWidget(new QLabel(tr("操作成功!")));
		changeHeader = false;
	}
	else {
		ui.status->addPermanentWidget(
			new QLabel(tr(
				"<a href=\"https://docs.microsoft.com/en-us/cpp/c-runtime-library/errno-constants?view=msvc-160\">错误返回值:</a>"
			) + QString(static_cast<char>(result)))
		);
	}

	mutex.unlock();
}

void MKCompress::launchChangeHeaderBack(QString inputFile, QString outputPath)
{
	mutex.lock();

	const wchar_t* inputFileWString = reinterpret_cast<const wchar_t*>(inputFile.utf16());
	// const wchar_t* outputPathWString = reinterpret_cast<const wchar_t*>(outputPath.utf16());

	error_t result = _changeHeaderBack(inputFileWString);

	if (result == 0) {
		ui.status->addPermanentWidget(new QLabel(tr("操作成功!")));
		changeHeader = true;
	}
	else {
		ui.status->addPermanentWidget(
			new QLabel(tr(
				"<a href=\"https://docs.microsoft.com/en-us/cpp/c-runtime-library/errno-constants?view=msvc-160\">错误返回值:</a>"
			) + QString(static_cast<char>(result)))
		);
	}

	mutex.unlock();
}

void MKCompress::chooseFileDialog() {
	// initialize a file choose dialog to choose input file
	QString inputFile = QFileDialog::getOpenFileName(this, tr("选择文件"), tr("/"));
	if (inputFile.isEmpty())
	{
		return;
	}
	const wchar_t* fileWString = reinterpret_cast<const wchar_t*>(inputFile.utf16());
	ui.status->clearMessage();
	// get a unique ptr instence
	auto fileInfo = std::make_unique<QFileInfo>(inputFile);
	if (fileInfo->exists() && fileInfo->isFile()) {
		// show input file path
		ui.inputFilePathContent->setText(inputFile);
		ui.status->clearMessage();

		if (*_getFileHeader(fileWString) == *getCharsMKC_HEAD()) {
			changeHeader = false;
			ui.status->addWidget(new QLabel(tr("还原模式")));
		}
		else if (*_getMKFileHeader(fileWString) == *getCharsMKC_HEAD()) {
			changeHeader = false;
			ui.status->addWidget(new QLabel(tr("该文件无需操作")));
		}
		else {
			changeHeader = true;
			ui.status->addWidget(new QLabel(tr("换头模式")));
		}
		parent = fileInfo->absolutePath();
	}
	else {
		// show file not found
		ui.status->showMessage("该文件不存在!");
	}
}
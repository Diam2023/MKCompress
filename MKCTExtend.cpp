#include "MKCTExtend.h"
#include "MKCTDLL.hpp"

MKCTExtend::MKCTExtend(mkc::MKCompressFlag* flag, std::vector<QString>* inputFileList, QString outputFile, QString password)
{
	this->flag = flag;
	this->inputFileList = inputFileList;
	this->outputFile = outputFile;
	this->password = password;

	if (this->inputFileList->size() == 1) {
		inputFile = this->inputFileList->at(mkc::FIRST_ELEMENT);
	}
}

void MKCTExtend::run()
{
	mkc::MKCompressResult* compressResult = new mkc::MKCompressResult{ true, EIO };
	mkc::MKCompressResult* changeHeaderResult = new mkc::MKCompressResult{ true, EIO };
	emit sendMsg(tr("正在运行..."));
	emit sendMsg(tr("\n"));

	if (flag->forward) {
		if (flag->compress) {
			compressResult = compress();
			if (compressResult->error) {
				emit sendRunComplete(!compressResult->error, tr(""));
				emit sendMsg(tr("压缩失败"));
				emit sendMsg(tr("\n"));
				emit sendMsg(tr("错误代码: compress ") + QString::number(int(compressResult->errValue)));
				emit sendMsg(tr("\n"));
				return;
			}
			else {
				emit sendMsg(tr("压缩成功"));
				emit sendMsg(tr("\n"));
			}
		}
		if (flag->changeHeader) {
			changeHeaderResult = changeHeader();
			if (changeHeaderResult->error) {
				emit sendRunComplete(!changeHeaderResult->error, tr(""));
				emit sendMsg(tr("换头失败"));
				emit sendMsg(tr("\n"));
				emit sendMsg(tr("错误代码: changeHeader ") + QString::number(int(changeHeaderResult->errValue)));
				emit sendMsg(tr("\n"));
				return;
			}
			else {
				emit sendMsg(tr("换头成功"));
				emit sendMsg(tr("\n"));
			}
		}
	}
	else {
		if (flag->changeHeader) {
			changeHeaderResult = changeHeader();
			if (changeHeaderResult->error) {
				emit sendRunComplete(!changeHeaderResult->error, tr(""));
				emit sendMsg(tr("还原失败"));
				emit sendMsg(tr("\n"));
				emit sendMsg(tr("错误代码: changeHeader ") + QString::number(int(changeHeaderResult->errValue)));
				emit sendMsg(tr("\n"));
				return;
			}
			else {
				emit sendMsg(tr("还原成功"));
				emit sendMsg(tr("\n"));
			}
		}
		if (flag->compress) {
			compressResult = compress();
			if (compressResult->error) {
				emit sendRunComplete(!compressResult->error, tr(""));
				emit sendMsg(tr("解压失败"));
				emit sendMsg(tr("\n"));
				emit sendMsg(tr("错误代码: compress ") + QString::number(int(compressResult->errValue)));
				emit sendMsg(tr("\n"));
				return;
			}
			else {
				emit sendMsg(tr("解压成功"));
				emit sendMsg(tr("\n"));
			}
		}
	}

	if ((flag->compress && !compressResult->error) || (flag->changeHeader && !changeHeaderResult->error)) {
		emit sendRunComplete(true, tr(""));
		emit sendMsg(tr("运行成功"));
		emit sendMsg(outputFile);
		emit sendMsg(tr("\n"));
	}

	delete compressResult;
	delete changeHeaderResult;
	exec();
}

mkc::MKCompressResult* MKCTExtend::compress()
{
	mkc::MKCompressResult* result = new mkc::MKCompressResult{ false, 0 };

	try
	{
		// load 7za.dll
		bit7z::Bit7zLibrary lib{ L"7za.dll" };

		if (flag->forward) {

			auto opf = std::make_unique<QFileInfo>(outputFile);

			auto dir = std::make_unique<QDir>(opf->path());

			QString opfFinal = dir->absoluteFilePath(opf->baseName()) + mkc::SEVENZ_TYPE;

			// get compressor incetance
			auto compressor = std::make_unique<bit7z::BitCompressor>(lib, bit7z::BitFormat::SevenZip);
			if (password != tr("")) {
				// set password
				const std::wstring pwd = mkc::qStringToWString(password);
				compressor->setPassword(pwd);
			}

			// update mode
			compressor->setUpdateMode(true);

			std::wstring outputFileWString = mkc::qStringToWString(opfFinal);

			if (!dir->exists()) {
				dir->mkdir(opf->path());
				// debug
				// qDebug() << tr("mkdir");
			}

			// iterator input list
			for (auto iterator = inputFileList->begin(); iterator != inputFileList->end(); iterator++) {
				auto info = std::make_unique<QFileInfo>(*iterator);
				if (info->exists()) {
					if (info->isFile()) {
						compressor->compressFile(mkc::qStringToWString(*iterator), outputFileWString);
						emit sendMsg(tr("compress: ") + *iterator);
					}
					else {
						compressor->compressDirectory(mkc::qStringToWString(*iterator), outputFileWString);
						emit sendMsg(tr("compress: ") + *iterator);
					}
				}
				else {
					emit sendMsg(tr("ignor: ") + *iterator);
				}
			}
			inputFile = opfFinal;

			delete compressor.release();
			compressor = nullptr;
		}
		else {
			// create a smart pointer for decompress
			auto extractor = std::make_unique<bit7z::BitExtractor>(lib, bit7z::BitFormat::SevenZip);
			if (password != tr("")) {
				// set password
				const std::wstring pwd = mkc::qStringToWString(password);
				extractor->setPassword(pwd);
			}

			const std::wstring inputFileWString = mkc::qStringToWString(inputFile);
			const std::wstring outputFileWString = mkc::qStringToWString(outputFile);

			extractor->extract(inputFileWString, outputFileWString);

			delete extractor.release();
			extractor = nullptr;
		}
	}
	catch (const bit7z::BitException& ex)
	{
		// synchronize to conslot
		qDebug() << ex.what();
		emit sendMsg(ex.what());
	}
	return result;
}

mkc::MKCompressResult* MKCTExtend::changeHeader()
{
	mkc::MKCompressResult* result = new mkc::MKCompressResult{ false, 0 };
	// first change header
	// write data
	if (flag->forward) {
		auto opf = std::make_unique<QFileInfo>(outputFile);
		if (!opf->exists()) {
			if (!QFile::copy(inputFile, outputFile)) {
				result->error = true;
				result->errValue = ENOENT;
				emit sendMsg(tr("file copy error: ") + QString::number(int(result->errValue)));
			}
		}
		else {
			emit sendMsg(tr("output file exist"));
		}

		auto ipf = std::make_unique<QFileInfo>(inputFile);

		std::string suffixName = ipf->completeSuffix().toStdString();
		const char* suffix = suffixName.c_str();

		delete ipf.release();
		ipf = nullptr;

		delete opf.release();
		opf = nullptr;

		const wchar_t* outputFileWChar = reinterpret_cast<const wchar_t*>(outputFile.utf16());
		
		// define error value
		errno_t errValue;

		// IMPORTANT

		// add origin suffix to file
		errValue = _pushLastBytes(outputFileWChar, suffix);
		if (errValue != 0) {
			result->error = true;
			result->errValue = errValue;
			emit sendMsg(tr("popLastBytes error: ") + QString::number(int(result->errValue)));
		}

		// change file header
		errValue = _changeHeaderTo(outputFileWChar);
		if (errValue != 0) {
			result->error = true;
			result->errValue = errValue;
			emit sendMsg(tr("changeHeaderTo error: ") + QString::number(int(result->errValue)));
		}

	}
	else {

		auto ipf = std::make_unique<QFileInfo>(inputFile);

		auto dir = std::make_unique<QDir>(outputFile);
		auto opfTmp = dir->absoluteFilePath(ipf->fileName());

		auto opf = std::make_unique<QFileInfo>(opfTmp);

		auto opfp = std::make_unique<QDir>(outputFile);

		if (!opfp->exists()) {
			opfp->mkdir(outputFile);
			// debug
			// qDebug() << tr("mkdir");
		}

		delete opfp.release();
		opfp = nullptr;

		if (!opf->exists()) {
			if (!QFile::copy(inputFile, opfTmp)) {
				result->error = true;
				result->errValue = ENOENT;
				// debug
				// qDebug() << tr("file copy error");
				emit sendMsg(tr("file copy error: ") + QString::number(int(result->errValue)));
			}
		}
		else {
			result->error = true;
			result->errValue = EIO;
			// debug
			emit sendMsg(tr("output file exist"));
			// qDebug() << tr("output file exist");
		}

		const wchar_t* outputFileWChar = reinterpret_cast<const wchar_t*>(opfTmp.utf16());

		errno_t errValue;
		// get origin suffix to file
		char* suffix;

		// IMPORTANT

		// change file header back
		errValue = _changeHeaderBack(outputFileWChar);

		// get last bytes
		suffix = _popLatsBytes(outputFileWChar);


		if (errValue != 0) {
			result->error = true;
			result->errValue = errValue;
			emit sendMsg(tr("changeHeaderBack error: ") + QString::number(int(result->errValue)));
		}

		QString opfFinal = dir->absoluteFilePath(opf->baseName()) + tr(".") + QString::QString(suffix);
		if (!QFile::rename(opfTmp, opfFinal)) {
			result->error = true;
			result->errValue = EIO;
			emit sendMsg(tr("file rename error: ") + QString::number(int(result->errValue)));
		}

		delete dir.release();
		dir = nullptr;

		delete opf.release();
		opf = nullptr;

		delete ipf.release();
		ipf = nullptr;

		inputFile = opfFinal;
	}

	return result;
}

QString MKCTExtend::toString(std::vector<QString>* list)
{
	QString result;

	for (auto iterator = list->begin(); iterator != list->end(); iterator++) {
		auto element = *iterator;
		result += tr(" ") + element;
	}

	return result;
}
